FROM debian:testing-slim as builder

# Fluent Bit version
ENV FLB_VERSION 1.4.1
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get -y dist-upgrade && \
    apt-get install -y --no-install-recommends \
      build-essential \
      cmake \
      make \
      curl \
      unzip \
      libssl-dev \
      libasl-dev \
      libsasl2-dev \
      pkg-config \
      libsystemd-dev \
      zlib1g-dev \
      ca-certificates \
      flex \
      bison \
      file

RUN mkdir -p /fluent-bit/bin /fluent-bit/etc /fluent-bit/log /tmp/src/
RUN curl -sSL https://github.com/fluent/fluent-bit/archive/v${FLB_VERSION}.tar.gz | \
    tar zx --strip=1 -C /tmp/src/

RUN rm -rf /tmp/src/build/*

WORKDIR /tmp/src/build/
RUN cmake -DFLB_DEBUG=Off \
          -DFLB_TRACE=Off \
          -DFLB_JEMALLOC=On \
          -DFLB_BUFFERING=On \
          -DFLB_TLS=On \
          -DFLB_SHARED_LIB=Off \
          -DFLB_EXAMPLES=Off \
          -DFLB_HTTP_SERVER=On \
          -DFLB_IN_SYSTEMD=On \
          -DFLB_OUT_KAFKA=On ..

RUN make -j $(getconf _NPROCESSORS_ONLN)
RUN install bin/fluent-bit /fluent-bit/bin/

# Configuration files
WORKDIR /tmp/src/
RUN mkdir /fluent-bit/lib
RUN cp conf/fluent-bit.conf \
       conf/parsers.conf \
       conf/parsers_java.conf \
       conf/parsers_extra.conf \
       conf/parsers_openstack.conf \
       conf/parsers_cinder.conf \
       conf/plugins.conf \
       /fluent-bit/etc/


FROM golang:1.13 as lokibuilder

ENV LOKI_VERSION 1.3.0

RUN mkdir -p /tmp/src /loki
RUN curl -L https://github.com/grafana/loki/archive/v${LOKI_VERSION}.tar.gz | \
    tar zx --strip=1 -C /tmp/src/
WORKDIR /tmp/src
RUN make clean && make BUILD_IN_CONTAINER=false fluent-bit-plugin 
RUN cp /tmp/src/cmd/fluent-bit/out_loki.so /loki

FROM debian:testing-slim

RUN apt-get update && \
    apt-get -y dist-upgrade && \
    apt-get install -y --no-install-recommends \
        libsasl2-2 \
        libssl1.1 \
        ca-certificates && \
    rm -rf /var/lib/apt/lists

COPY --from=builder /fluent-bit /fluent-bit
COPY --from=lokibuilder /loki/out_loki.so /fluent-bit/lib

EXPOSE 2020

LABEL source=git@github.com:kyma-project/kyma.git

CMD ["/fluent-bit/bin/fluent-bit", "-e", "/fluent-bit/lib/out_loki.so", "-c", "/fluent-bit/etc/fluent-bit.conf"]
