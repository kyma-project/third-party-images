ARG GOLANG_VERSION
ARG BUSYBOX_VERSION
FROM golang:${GOLANG_VERSION} as builder

ARG ALERTMANAGER_VERSION
WORKDIR /
RUN curl -LO https://github.com/prometheus/alertmanager/archive/refs/tags/v${ALERTMANAGER_VERSION}.tar.gz && \
    tar xvzf v${ALERTMANAGER_VERSION}.tar.gz
WORKDIR /alertmanager-${ALERTMANAGER_VERSION}
RUN make build

FROM europe-docker.pkg.dev/kyma-project/prod/external/busybox:${BUSYBOX_VERSION}
LABEL source=git@github.com:kyma-project/kyma.git

ARG ALERTMANAGER_VERSION

COPY --from=builder /alertmanager-${ALERTMANAGER_VERSION}/amtool /bin/amtool
COPY --from=builder /alertmanager-${ALERTMANAGER_VERSION}/alertmanager /bin/alertmanager
COPY --from=builder /alertmanager-${ALERTMANAGER_VERSION}/examples/ha/alertmanager.yml /etc/alertmanager/alertmanager.yml

RUN mkdir -p /alertmanager && \
    chown -R nobody:nobody etc/alertmanager /alertmanager

USER       nobody
EXPOSE     9093
VOLUME     [ "/alertmanager" ]
WORKDIR    /alertmanager
ENTRYPOINT [ "/bin/alertmanager" ]
CMD        [ "--config.file=/etc/alertmanager/alertmanager.yml", \
             "--storage.path=/alertmanager" ]

