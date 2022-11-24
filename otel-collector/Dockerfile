ARG ALPINE_VERSION=3.16
ARG GOLANG_VERSION=1.19

FROM golang:${GOLANG_VERSION} as build
WORKDIR /app
COPY main.go main.go 
COPY go.mod go.mod
COPY go.sum go.sum

COPY config.yaml config.yaml

RUN go mod download

RUN go build -ldflags '-s -w' -o otelcol main.go

FROM gcr.io/distroless/base-debian11
COPY --from=build /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/ca-certificates.crt
COPY --from=build /app/otelcol /

EXPOSE 4317/tcp 55678/tcp

ENTRYPOINT ["/otelcol"]
