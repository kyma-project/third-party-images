ARG GOLANG_VERSION

FROM golang:${GOLANG_VERSION} as builder

ARG NODE_EXPORTER_VERSION
WORKDIR /
RUN curl -Lo /node_exporter.tar.gz https://github.com/prometheus/node_exporter/archive/refs/tags/v${NODE_EXPORTER_VERSION}.tar.gz && \
    tar -xvzf /node_exporter.tar.gz
WORKDIR /node_exporter-${NODE_EXPORTER_VERSION}
RUN make build

ARG BUSYBOX_VERSION

FROM scratch
LABEL source=git@github.com:kyma-project/kyma.git

ARG NODE_EXPORTER_VERSION
COPY --from=builder /node_exporter-${NODE_EXPORTER_VERSION}/node_exporter /bin/node_exporter

EXPOSE      9100
USER        nobody
ENTRYPOINT  [ "/bin/node_exporter" ]

