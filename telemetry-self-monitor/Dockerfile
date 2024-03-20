ARG PROMETHEUS_VERSION
ARG ALPINE_VERSION

FROM node:alpine${ALPINE_VERSION} as builder
RUN apk add --no-cache git make musl-dev go curl bash

# Configure Go
ENV GOROOT /usr/lib/go
ENV GOPATH /go
ENV PATH /go/bin:$PATH

RUN mkdir -p ${GOPATH}/src ${GOPATH}/bin
WORKDIR /
RUN curl -LO https://github.com/prometheus/prometheus/archive/refs/tags/v${PROMETHEUS_VERSION}.tar.gz  && \
    tar xvzf v${PROMETHEUS_VERSION}.tar.gz

WORKDIR /prometheus-${PROMETHEUS_VERSION}
COPY plugins.yml plugins.yml
RUN make build

FROM alpine:${ALPINE_VERSION}

RUN mkdir /etc/prometheus

COPY --from=builder /prometheus-${PROMETHEUS_VERSION}/prometheus /bin/prometheus
COPY --from=builder /prometheus-${PROMETHEUS_VERSION}/console_libraries /etc/prometheus/console_libraries
COPY --from=builder /prometheus-${PROMETHEUS_VERSION}/consoles /etc/prometheus/consoles

RUN chown -R nobody:nobody  /etc/prometheus

USER       nobody
EXPOSE     9090

ENTRYPOINT [ "/bin/prometheus" ]
CMD        [ "--config.file=/etc/prometheus/prometheus.yml", \
             "--storage.tsdb.path=/prometheus", \
             "--web.console.libraries=/etc/prometheus/console_libraries", \
             "--web.console.templates=/etc/prometheus/consoles"  ]
