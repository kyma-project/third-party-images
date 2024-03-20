FROM node:alpine3.18 as builder
RUN apk add --no-cache git make musl-dev go curl bash

# Configure Go
ENV GOROOT /usr/lib/go
ENV GOPATH /go
ENV PATH /go/bin:$PATH

RUN mkdir -p ${GOPATH}/src ${GOPATH}/bin
WORKDIR /
RUN curl -LO https://github.com/prometheus/prometheus/archive/refs/tags/v2.50.1.tar.gz  && \
    tar xvzf v2.50.1.tar.gz

WORKDIR /prometheus-2.50.1
RUN make build

FROM alpine:3.18.3

RUN mkdir /prometheus

COPY --from=builder /prometheus-2.50.1/prometheus /prometheus/prometheus
COPY --from=builder /prometheus-2.50.1/console_libraries /prometheus/console_libraries
COPY --from=builder /prometheus-2.50.1/consoles /prometheus/consoles

RUN chown -R nobody:nobody  /prometheus

WORKDIR /prometheus

USER       nobody
EXPOSE     9090
ENTRYPOINT [ "./prometheus" ]
CMD        [ "--config.file=prometheus.yml", \
             "--storage.tsdb.path=/prometheus", \
             "--web.console.libraries=./console_libraries", \
             "--web.console.templates=./consoles"  ]
