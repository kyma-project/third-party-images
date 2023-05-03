ARG GOLANG_VERSION
ARG BUSYBOX_VERSION
FROM golang:${GOLANG_VERSION} as builder

ARG CONFIGMAP_RELOAD_VERSION
WORKDIR /
RUN curl -LO https://github.com/jimmidyson/configmap-reload/archive/refs/tags/v${CONFIGMAP_RELOAD_VERSION}.tar.gz && \
    tar xvzf v${CONFIGMAP_RELOAD_VERSION}.tar.gz
WORKDIR /configmap-reload-${CONFIGMAP_RELOAD_VERSION}
RUN GO111MODULE=on CGO_ENABLED=0 go build --installsuffix cgo -ldflags="-s -w -extldflags '-static'" -a -o configmap-reload ./configmap-reload.go

ARG BUSYBOX_VERSION
FROM europe-docker.pkg.dev/kyma-project/prod/external/busybox:${BUSYBOX_VERSION}
LABEL source=git@github.com:kyma-project/kyma.git
ARG CONFIGMAP_RELOAD_VERSION

USER 65534

COPY --from=builder /configmap-reload-${CONFIGMAP_RELOAD_VERSION}/configmap-reload /

ENTRYPOINT ["/configmap-reload"]
