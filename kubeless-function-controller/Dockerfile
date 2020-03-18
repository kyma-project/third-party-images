FROM kubeless/function-controller:v1.0.5 as builder

FROM alpine:3.11

LABEL source=git@github.com:kyma-project/kyma.git

RUN  apk add --no-cache ca-certificates
COPY --from=builder /kubeless-function-controller /kubeless-function-controller

ENTRYPOINT ["/kubeless-function-controller"]
