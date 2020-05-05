FROM golang:1.13-alpine as builder

ENV JAEGER_OPERATOR_VERSION 1.17.1

WORKDIR /go/src/github.com/jaegertracing

RUN apk update && apk upgrade && \
    apk add --no-cache bash git openssl make

RUN git clone --branch v${JAEGER_OPERATOR_VERSION} --single-branch --depth 1 https://github.com/jaegertracing/jaeger-operator.git

RUN cd jaeger-operator && GOOS=linux GOARCH=amd64 CGO_ENABLED=0 GO111MODULE=on go build -o /jaeger-operator

FROM scratch

ENV OPERATOR=/usr/local/bin/jaeger-operator \
    USER_UID=1001 \
    USER_NAME=jaeger-operator

COPY --from=builder ./jaeger-operator ${OPERATOR}
COPY --from=builder /go/src/github.com/jaegertracing/jaeger-operator/scripts/* /scripts/

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/usr/local/bin/jaeger-operator"]

USER ${USER_UID}
