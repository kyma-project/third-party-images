ARG ALPINE_VERSION
ARG GOLANG_VERSION

FROM golang:${GOLANG_VERSION} as build

ARG LOKI_VERSION

WORKDIR /src/loki
ADD https://github.com/grafana/loki/archive/refs/tags/v${LOKI_VERSION}.tar.gz /loki.tar.gz
RUN tar -xz --strip-components=1 --directory=/src/loki --file=/loki.tar.gz

# fix compatibility issue with golang 1.19
RUN go get github.com/json-iterator/go@v1.1.12
RUN go mod vendor

RUN make NETGO_CHECK='' clean && make BUILD_IN_CONTAINER=false NETGO_CHECK='' loki

FROM alpine:${ALPINE_VERSION}

RUN apk add --no-cache ca-certificates libcap

COPY --from=build /src/loki/cmd/loki/loki /usr/bin/loki
COPY --from=build /src/loki/cmd/loki/loki-docker-config.yaml /etc/loki/local-config.yaml
COPY --from=build /etc/nsswitch.conf /etc/nsswitch.conf

RUN addgroup -g 10001 -S loki && \
    adduser -u 10001 -S loki -G loki
RUN mkdir -p /loki/rules && \
    mkdir -p /loki/tmprules && \
    chown -R loki:loki /etc/loki /loki

USER loki
EXPOSE 3100
ENTRYPOINT [ "/usr/bin/loki" ]
CMD ["-config.file=/etc/loki/local-config.yaml"]