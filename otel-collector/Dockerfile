ARG GOLANG_VERSION

FROM golang:${GOLANG_VERSION} as build
ARG  OTEL_VERSION

WORKDIR /app
COPY builder-config.yaml .
ENV OTEL_VERSION ${OTEL_VERSION}
RUN sed -i s/OTEL_VERSION/${OTEL_VERSION}/g builder-config.yaml

RUN go install go.opentelemetry.io/collector/cmd/builder@v${OTEL_VERSION}
RUN builder --config=builder-config.yaml --name=otelcol --output-path=.

FROM gcr.io/distroless/base-debian11

COPY --from=build /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/ca-certificates.crt
COPY --from=build /app/otelcol /

ENTRYPOINT ["/otelcol"]
