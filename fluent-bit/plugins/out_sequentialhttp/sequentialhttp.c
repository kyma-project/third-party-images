/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2022 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <fluent-bit/flb_output_plugin.h>
#include <fluent-bit/flb_output.h>
#include <fluent-bit/flb_http_client.h>
#include <fluent-bit/flb_pack.h>
#include <fluent-bit/flb_str.h>
#include <fluent-bit/flb_time.h>
#include <fluent-bit/flb_utils.h>
#include <fluent-bit/flb_pack.h>
#include <fluent-bit/flb_sds.h>
#include <fluent-bit/flb_gzip.h>
#include <msgpack.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "sequentialhttp.h"
#include "sequentialhttp_conf.h"

#include <fluent-bit/flb_callback.h>

static int cb_http_init(struct flb_output_instance *ins,
                        struct flb_config *config, void *data)
{
    struct flb_out_sequentialhttp *ctx = NULL;
    (void) data;

    ctx = flb_http_conf_create(ins, config);
    if (!ctx) {
        return -1;
    }

    /* Set the plugin context */
    flb_output_set_context(ins, ctx);

    /*
     * This plugin instance uses the HTTP client interface, let's register
     * it debugging callbacks.
     */
    flb_output_set_http_debug_callbacks(ins);

    return 0;
}

static int http_post(struct flb_out_sequentialhttp *ctx,
                     const void *body, size_t body_len,
                     const char *tag, int tag_len)
{
    int ret;
    int out_ret = FLB_OK;
    int compressed = FLB_FALSE;
    size_t b_sent;
    void *payload_buf = NULL;
    size_t payload_size = 0;
    struct flb_upstream *u;
    struct flb_upstream_conn *u_conn;
    struct flb_http_client *c;
    struct mk_list *head;
    struct flb_config_map_val *mv;
    struct flb_slist_entry *key = NULL;
    struct flb_slist_entry *val = NULL;

    /* Get upstream context and connection */
    u = ctx->u;
    u_conn = flb_upstream_conn_get(u);
    if (!u_conn) {
        flb_plg_error(ctx->ins, "no upstream connections available to %s:%i",
                      u->tcp_host, u->tcp_port);
        return FLB_RETRY;
    }

    /* Map payload */
    payload_buf = (void *) body;
    payload_size = body_len;

    /* Should we compress the payload ? */
    if (ctx->compress_gzip == FLB_TRUE) {
        ret = flb_gzip_compress((void *) body, body_len,
                                &payload_buf, &payload_size);
        if (ret == -1) {
            flb_plg_error(ctx->ins,
                          "cannot gzip payload, disabling compression");
        }
        else {
            compressed = FLB_TRUE;
        }
    }

    /* Create HTTP client context */
    c = flb_http_client(u_conn, FLB_HTTP_POST, ctx->uri,
                        payload_buf, payload_size,
                        ctx->host, ctx->port,
                        ctx->proxy, 0);


    if (c->proxy.host) {
        flb_plg_debug(ctx->ins, "[http_client] proxy host: %s port: %i",
                      c->proxy.host, c->proxy.port);
    }

    /* Allow duplicated headers ? */
    flb_http_allow_duplicated_headers(c, ctx->allow_dup_headers);

    /*
     * Direct assignment of the callback context to the HTTP client context.
     * This needs to be improved through a more clean API.
     */
    c->cb_ctx = ctx->ins->callback;

    /* Append headers */
    if ((ctx->out_format == FLB_PACK_JSON_FORMAT_JSON) ||
        (ctx->out_format == FLB_PACK_JSON_FORMAT_STREAM) ||
        (ctx->out_format == FLB_PACK_JSON_FORMAT_LINES) ||
        (ctx->out_format == FLB_HTTP_OUT_GELF)) {
        flb_http_add_header(c,
                            FLB_HTTP_CONTENT_TYPE,
                            sizeof(FLB_HTTP_CONTENT_TYPE) - 1,
                            FLB_HTTP_MIME_JSON,
                            sizeof(FLB_HTTP_MIME_JSON) - 1);
    }
    else {
        flb_http_add_header(c,
                            FLB_HTTP_CONTENT_TYPE,
                            sizeof(FLB_HTTP_CONTENT_TYPE) - 1,
                            FLB_HTTP_MIME_MSGPACK,
                            sizeof(FLB_HTTP_MIME_MSGPACK) - 1);
    }

    if (ctx->header_tag) {
        flb_http_add_header(c,
                            ctx->header_tag,
                            flb_sds_len(ctx->header_tag),
                            tag, tag_len);
    }

    /* Content Encoding: gzip */
    if (compressed == FLB_TRUE) {
        flb_http_set_content_encoding_gzip(c);
    }

    /* Basic Auth headers */
    if (ctx->http_user && ctx->http_passwd) {
        flb_http_basic_auth(c, ctx->http_user, ctx->http_passwd);
    }

    flb_http_add_header(c, "User-Agent", 10, "Fluent-Bit", 10);

    flb_config_map_foreach(head, mv, ctx->headers) {
        key = mk_list_entry_first(mv->val.list, struct flb_slist_entry, _head);
        val = mk_list_entry_last(mv->val.list, struct flb_slist_entry, _head);

        flb_http_add_header(c,
                            key->str, flb_sds_len(key->str),
                            val->str, flb_sds_len(val->str));
    }

    ret = flb_http_do(c, &b_sent);
    if (ret == 0) {
        /*
         * Only allow the following HTTP status:
         *
         * - 200: OK
         * - 201: Created
         * - 202: Accepted
         * - 203: no authorative resp
         * - 204: No Content
         * - 205: Reset content
         */
        if (c->resp.status < 200 || c->resp.status > 205) {
            if (ctx->log_response_payload &&
                c->resp.payload && c->resp.payload_size > 0) {
                flb_plg_error(ctx->ins, "%s:%i, HTTP status=%i\n%s",
                              ctx->host, ctx->port,
                              c->resp.status, c->resp.payload);
            }
            else {
                flb_plg_error(ctx->ins, "%s:%i, HTTP status=%i",
                              ctx->host, ctx->port, c->resp.status);
            }
            // In case of 4XX error we have unrecoverable error
            if (c->resp.status >=400 && c->resp.status < 500) {
                flb_plg_error(ctx->ins, "%s:%i, Unrecoverable error HTTP status=%i",
                              ctx->host, ctx->port, c->resp.status);
                out_ret = FLB_ERROR;
            // Incase of others we can still try
            } else {
                out_ret = FLB_RETRY;
            }
            
        }
        else {
            if (ctx->log_response_payload &&
                c->resp.payload && c->resp.payload_size > 0) {
                flb_plg_info(ctx->ins, "%s:%i, HTTP status=%i\n%s",
                             ctx->host, ctx->port,
                             c->resp.status, c->resp.payload);
            }
            else {
                flb_plg_info(ctx->ins, "%s:%i, HTTP status=%i",
                             ctx->host, ctx->port,
                             c->resp.status);
            }
        }
    }
    else {
        flb_plg_error(ctx->ins, "could not flush records to %s:%i (http_do=%i)",
                      ctx->host, ctx->port, ret);
        out_ret = FLB_RETRY;
    }

    /*
     * If the payload buffer is different than incoming records in body, means
     * we generated a different payload and must be freed.
     */
    if (payload_buf != body) {
        flb_free(payload_buf);
    }

    /* Destroy HTTP client context */
    flb_http_client_destroy(c);

    /* Release the TCP connection */
    flb_upstream_conn_release(u_conn);

    return out_ret;
}

static int http_gelf(struct flb_out_sequentialhttp *ctx,
                     const char *data, uint64_t bytes,
                     const char *tag, int tag_len)
{
    flb_sds_t s;
    flb_sds_t tmp = NULL;
    msgpack_unpacked result;
    size_t off = 0;
    size_t size = 0;
    msgpack_object root;
    msgpack_object map;
    msgpack_object *obj;
    struct flb_time tm;
    int ret;

    size = bytes * 1.5;

    /* Allocate buffer for our new payload */
    s = flb_sds_create_size(size);
    if (!s) {
        return FLB_RETRY;
    }

    msgpack_unpacked_init(&result);
    while (msgpack_unpack_next(&result, data, bytes, &off) ==
           MSGPACK_UNPACK_SUCCESS) {

        if (result.data.type != MSGPACK_OBJECT_ARRAY) {
            continue;
        }

        root = result.data;
        if (root.via.array.size != 2) {
            continue;
        }

        flb_time_pop_from_msgpack(&tm, &result, &obj);
        map = root.via.array.ptr[1];

        tmp = flb_msgpack_to_gelf(&s, &map, &tm, &(ctx->gelf_fields));
        if (!tmp) {
            flb_plg_error(ctx->ins, "error encoding to GELF");
            flb_sds_destroy(s);
            msgpack_unpacked_destroy(&result);
            return FLB_ERROR;
        }

        /* Append new line */
        tmp = flb_sds_cat(s, "\n", 1);
        if (!tmp) {
            flb_plg_error(ctx->ins, "error concatenating records");
            flb_sds_destroy(s);
            msgpack_unpacked_destroy(&result);
            return FLB_RETRY;
        }
        s = tmp;
    }

    ret = http_post(ctx, s, flb_sds_len(s), tag, tag_len);
    flb_sds_destroy(s);
    msgpack_unpacked_destroy(&result);

    return ret;
}

/* 
It's basically a slightly modified version of standard Fluent Bit flb_pack_msgpack_to_json_format function.
It unpacks msgpack records one-by-one, converts them to JSON and sends to the HTTP backend.
*/
static int pack_msgpack_to_json_format_and_send_sequentially(struct flb_out_sequentialhttp *ctx,
                                                             const char *data, uint64_t bytes,
                                                             int json_format, int date_format,
                                                             flb_sds_t date_key, const char *tag,
                                                             int tag_len)
{
    int i;
    int len;
    int ok = MSGPACK_UNPACK_SUCCESS;
    int records = 0;
    int map_size;
    size_t off = 0;
    char time_formatted[32];
    size_t s;
    flb_sds_t out_js;
    msgpack_unpacked result;
    msgpack_object root;
    msgpack_object map;
    msgpack_sbuffer tmp_sbuf;
    msgpack_packer tmp_pck;
    msgpack_object *obj;
    msgpack_object *k;
    msgpack_object *v;
    struct tm tm;
    struct flb_time tms;
    int ret = FLB_OK;
    int ret_temp = FLB_ERROR;

    records = flb_mp_count(data, bytes);
    if (records <= 0)
    {
        return FLB_ERROR;
    }

    /* For json lines and streams mode we need a pre-allocated buffer */
    // if (json_format == FLB_PACK_JSON_FORMAT_LINES ||
    //     json_format == FLB_PACK_JSON_FORMAT_STREAM) {
    //     out_buf = flb_sds_create_size(bytes * 1.25);
    //     if (!out_buf) {
    //         flb_errno();
    //         return NULL;
    //     }
    // }

    /* Create temporary msgpack buffer */
    msgpack_sbuffer_init(&tmp_sbuf);
    msgpack_packer_init(&tmp_pck, &tmp_sbuf, msgpack_sbuffer_write);

    if (json_format == FLB_PACK_JSON_FORMAT_JSON)
    {
        msgpack_pack_array(&tmp_pck, records);
    }
    msgpack_unpacked_init(&result);
    while (msgpack_unpack_next(&result, data, bytes, &off) == ok)
    {
        /* Each array must have two entries: time and record */
        root = result.data;
        if (root.via.array.size != 2)
        {
            continue;
        }

        /* Unpack time */
        flb_time_pop_from_msgpack(&tms, &result, &obj);

        /* Get the record/map */
        map = root.via.array.ptr[1];
        map_size = map.via.map.size;

        if (date_key != NULL)
        {
            msgpack_pack_map(&tmp_pck, map_size + 1);
        }
        else
        {
            msgpack_pack_map(&tmp_pck, map_size);
        }

        if (date_key != NULL)
        {
            /* Append date key */
            msgpack_pack_str(&tmp_pck, flb_sds_len(date_key));
            msgpack_pack_str_body(&tmp_pck, date_key, flb_sds_len(date_key));

            /* Append date value */
            switch (date_format)
            {
            case FLB_PACK_JSON_DATE_DOUBLE:
                msgpack_pack_double(&tmp_pck, flb_time_to_double(&tms));
                break;
            case FLB_PACK_JSON_DATE_ISO8601:
                /* Format the time, use microsecond precision not nanoseconds */
                gmtime_r(&tms.tm.tv_sec, &tm);
                s = strftime(time_formatted, sizeof(time_formatted) - 1,
                             FLB_PACK_JSON_DATE_ISO8601_FMT, &tm);

                len = snprintf(time_formatted + s,
                               sizeof(time_formatted) - 1 - s,
                               ".%06" PRIu64 "Z",
                               (uint64_t)tms.tm.tv_nsec / 1000);
                s += len;
                msgpack_pack_str(&tmp_pck, s);
                msgpack_pack_str_body(&tmp_pck, time_formatted, s);
                break;
            case FLB_PACK_JSON_DATE_EPOCH:
                msgpack_pack_uint64(&tmp_pck, (long long unsigned)(tms.tm.tv_sec));
                break;
            }
        }
        /* Append remaining keys/values */
        for (i = 0; i < map_size; i++)
        {
            k = &map.via.map.ptr[i].key;
            v = &map.via.map.ptr[i].val;
            msgpack_pack_object(&tmp_pck, *k);
            msgpack_pack_object(&tmp_pck, *v);
        }

        /*
         * If the format is the original msgpack style, just continue since
         * we don't care about separator or JSON convertion at this point.
         */

        /* Encode current record into JSON in a temporary variable */
        out_js = flb_msgpack_raw_to_json_sds(tmp_sbuf.data, tmp_sbuf.size);
        if (!out_js)
        {
            msgpack_sbuffer_destroy(&tmp_sbuf);
            return FLB_ERROR;
        }

        ret_temp = http_post(ctx, out_js, flb_sds_len(out_js), tag, tag_len);
        if (ret_temp == FLB_RETRY) {
            if (ret == FLB_RETRY || ret == FLB_OK ) {
                ret = FLB_RETRY;
            }
        } else if (ret_temp == FLB_ERROR) {
            ret = FLB_ERROR;
        }

        /* Release temporary json sds buffer */
        flb_sds_destroy(out_js);
    }

    /* if one log fails, the whole chunk has to be retried */
    if (ret == FLB_RETRY) {
        msgpack_sbuffer_clear(&tmp_sbuf);
    }

    msgpack_unpacked_destroy(&result);
    return ret;
}
static void cb_http_flush(const void *data, size_t bytes,
                          const char *tag, int tag_len,
                          struct flb_input_instance *i_ins,
                          void *out_context,
                          struct flb_config *config)
{
    int ret = FLB_ERROR;
    flb_sds_t json;
    struct flb_out_sequentialhttp *ctx = out_context;
    (void) i_ins;

    if ((ctx->out_format == FLB_PACK_JSON_FORMAT_JSON) ||
        (ctx->out_format == FLB_PACK_JSON_FORMAT_STREAM) ||
        (ctx->out_format == FLB_PACK_JSON_FORMAT_LINES)) {

        /* 
        Original out-http plugin uses flb_pack_msgpack_to_json_format to pack records as a comma-separated JSON document.
        The sequential version sends records one-by-one instead
        */

        // json = flb_pack_msgpack_to_json_format(data, bytes,
        //                                        ctx->out_format,
        //                                        ctx->json_date_format,
        //                                        ctx->date_key);
        // if (json != NULL) {
        // ret = http_post(ctx, json, flb_sds_len(json), tag, tag_len);
        // flb_sds_destroy(json);
        // }

        ret = pack_msgpack_to_json_format_and_send_sequentially(ctx, data, bytes,
                                                                ctx->out_format,
                                                                ctx->json_date_format,
                                                                ctx->date_key, tag,
                                                                tag_len);
    }
    else if (ctx->out_format == FLB_HTTP_OUT_GELF) {
        ret = http_gelf(ctx, data, bytes, tag, tag_len);
    }
    else {
        ret = http_post(ctx, data, bytes, tag, tag_len);
    }

    FLB_OUTPUT_RETURN(ret);
}

static int cb_http_exit(void *data, struct flb_config *config)
{
    struct flb_out_sequentialhttp *ctx = data;

    flb_http_conf_destroy(ctx);
    return 0;
}

/* Configuration properties map */
static struct flb_config_map config_map[] = {
    {
     FLB_CONFIG_MAP_STR, "proxy", NULL,
     0, FLB_FALSE, 0,
     "Specify an HTTP Proxy. The expected format of this value is http://host:port. "
     },
    {
     FLB_CONFIG_MAP_BOOL, "allow_duplicated_headers", "true",
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, allow_dup_headers),
     "Specify if duplicated headers are allowed or not"
    },
    {
     FLB_CONFIG_MAP_BOOL, "log_response_payload", "true",
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, log_response_payload),
     "Specify if the response paylod should be logged or not"
    },
    {
     FLB_CONFIG_MAP_STR, "http_user", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, http_user),
     "Set HTTP auth user"
    },
    {
     FLB_CONFIG_MAP_STR, "http_passwd", "",
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, http_passwd),
     "Set HTTP auth password"
    },
    {
     FLB_CONFIG_MAP_STR, "header_tag", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, header_tag),
     "Set a HTTP header which value is the Tag"
    },
    {
     FLB_CONFIG_MAP_STR, "format", NULL,
     0, FLB_FALSE, 0,
     "Set desired payload format: json, json_stream, json_lines, gelf or msgpack"
    },
    {
     FLB_CONFIG_MAP_STR, "json_date_format", NULL,
     0, FLB_FALSE, 0,
     "Specify the format of the date. Supported formats are 'double' and 'iso8601'"
    },
    {
     FLB_CONFIG_MAP_STR, "json_date_key", "date",
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, json_date_key),
     "Specify the name of the date field in output"
    },
    {
     FLB_CONFIG_MAP_STR, "compress", NULL,
     0, FLB_FALSE, 0,
     "Set payload compression mechanism. Option available is 'gzip'"
    },
    {
     FLB_CONFIG_MAP_SLIST_1, "header", NULL,
     FLB_CONFIG_MAP_MULT, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, headers),
     "Add a HTTP header key/value pair. Multiple headers can be set"
    },
    {
     FLB_CONFIG_MAP_STR, "uri", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, uri),
     "Specify an optional HTTP URI for the target web server, e.g: /something"
    },

    /* Gelf Properties */
    {
     FLB_CONFIG_MAP_STR, "gelf_timestamp_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, gelf_fields.timestamp_key),
        "Specify the key to use for 'timestamp' in gelf format"
    },
    {
     FLB_CONFIG_MAP_STR, "gelf_host_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, gelf_fields.host_key),
     "Specify the key to use for the 'host' in gelf format"
    },
    {
     FLB_CONFIG_MAP_STR, "gelf_short_message_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, gelf_fields.short_message_key),
     "Specify the key to use as the 'short' message in gelf format"
    },
    {
     FLB_CONFIG_MAP_STR, "gelf_full_message_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, gelf_fields.full_message_key),
     "Specify the key to use for the 'full' message in gelf format"
    },
    {
     FLB_CONFIG_MAP_STR, "gelf_level_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_sequentialhttp, gelf_fields.level_key),
     "Specify the key to use for the 'level' in gelf format"
    },
    {
     FLB_CONFIG_MAP_STR, "body_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_http, body_key),
     "Specify the key which contains the body"
    },
    {
     FLB_CONFIG_MAP_STR, "headers_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_http, headers_key),
     "Specify the key which contains the headers"
    },
    {
     FLB_CONFIG_MAP_STR, "body_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_http, body_key),
     "Specify the key which contains the body"
    },
    {
     FLB_CONFIG_MAP_STR, "headers_key", NULL,
     0, FLB_TRUE, offsetof(struct flb_out_http, headers_key),
     "Specify the key which contains the headers"
    },

    /* EOF */
    {0}
};

/* Plugin reference */
struct flb_output_plugin out_sequentialhttp_plugin = {
    .name        = "sequentialhttp",
    .description = "Sequential HTTP Output",
    .cb_init     = cb_http_init,
    .cb_pre_run  = NULL,
    .cb_flush    = cb_http_flush,
    .cb_exit     = cb_http_exit,
    .config_map  = config_map,
    .flags       = FLB_OUTPUT_NET | FLB_IO_OPT_TLS,
    .workers     = 2
};
