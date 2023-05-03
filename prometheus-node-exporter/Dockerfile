ARG GOLANG_VERSION
ARG BUSYBOX_VERSION
ARG NODE_EXPORTER_VERSION
FROM golang:${GOLANG_VERSION} as builder

RUN curl -LO https://github.com/prometheus/node_exporter/archive/refs/tags/v${NODE_EXPORTER_VERSION}.tar.gz \
    && tar -xvzf node_exporter-${NODE_EXPORTER_VERSION}.linux-amd64.tar.gz  \
    && cp node_exporter-${NODE_EXPORTER_VERSION}.linux-amd64 /tmp/ \
WORKDIR /tmp
RUN make build

FROM europe-docker.pkg.dev/kyma-project/prod/external/busybox:${BUSYBOX_VERSION}
LABEL source=git@github.com:kyma-project/kyma.git

COPY --from=builder /tmp/node_exporter /bin/node_exporter

EXPOSE      9100
USER        nobody
ENTRYPOINT  [ "/bin/node_exporter" ]

