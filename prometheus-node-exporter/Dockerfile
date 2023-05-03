ARG GOLANG_VERSION
FROM golang:${GOLANG_VERSION} as builder

ARG NODE_EXPORTER_VERSION
WORKDIR /
RUN curl -Lo /node_exporter https://github.com/prometheus/node_exporter/archive/refs/tags/v${NODE_EXPORTER_VERSION}.tar.gz && \
    ls -la / && \
    tar -xvzf node_exporter.tar.gz
WORKDIR /node_exporter
RUN make build

ARG BUSYBOX_VERSION
FROM europe-docker.pkg.dev/kyma-project/prod/external/busybox:${BUSYBOX_VERSION}
LABEL source=git@github.com:kyma-project/kyma.git

COPY --from=builder /node_exporter/node_exporter /bin/node_exporter

EXPOSE      9100
USER        nobody
ENTRYPOINT  [ "/bin/node_exporter" ]

