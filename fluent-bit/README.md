# Fluent Bit Docker Image

This image is based on the official [Fluent Bit Docker Image](https://github.com/fluent/fluent-bit-docker-image).

Changes are made to be based on on the latest debian:testing-slim in contrast to distroless, that is always debian:stable based.

The custom plugin pugin has following customizations:
* loki output plugin
* sequential http output plugin

## Sequential HTTP Output Plugin
The standard HTTP output plugin `out-http` sends out records in batch. This is a problem for some consuming services in SKR environent (e.g. SAP Audit Service) 
The `out-sequentialhttp` is a drop-in replacement that sends out records sequentially (one-by-one).

## Code Modifications
The code of `out-sequentialhttp` is almost identical to `out-http`. The only difference is how msgpack to JSON transcoding / sending to the HTTP backend is done in the `cb_http_flush` function.  

### Example Configuration
```
[SERVICE]
    Flush         3
    Log_Level     info
    Daemon        off
    HTTP_Server   On
    HTTP_Listen   0.0.0.0
    HTTP_Port     2020

[INPUT]
    Name              dummy
    Tag               *
    Rate              1
    Dummy             {"log":"level-info","msg":"succesful", "username":"username", "email":"email@email.com", "time":"2020-11-25T12:37:22Z"}

[OUTPUT]
    Name             sequentialhttp
    Match            *
    Retry_Limit      False
    Host             mock
    Port             8081
    URI              /audit-log
    Header           Content-Type application/json
    HTTP_User        user
    HTTP_Passwd      pass
    Format           json_stream
    tls              on
    tls.verify       off
```
