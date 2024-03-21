ARG ALPINE_VERSION

FROM node:alpine${ALPINE_VERSION} as builder
RUN apk add --no-cache git make musl-dev go curl bash

# Configure Go
ENV GOROOT /usr/lib/go
ENV GOPATH /go
ENV PATH /go/bin:$PATH

ARG PROMETHEUS_VERSION

RUN mkdir -p ${GOPATH}/src ${GOPATH}/bin
WORKDIR /
RUN curl -LO https://github.com/prometheus/prometheus/archive/refs/tags/v${PROMETHEUS_VERSION}.tar.gz
RUN tar xvzf v${PROMETHEUS_VERSION}.tar.gz

WORKDIR /prometheus-${PROMETHEUS_VERSION}
COPY plugins.yml plugins.yml
RUN make build

FROM scratch

ARG PROMETHEUS_VERSION

COPY --from=builder /prometheus-${PROMETHEUS_VERSION}/prometheus /bin/prometheus

USER       nobody
EXPOSE     9090

ENTRYPOINT [ "/bin/prometheus" ]
