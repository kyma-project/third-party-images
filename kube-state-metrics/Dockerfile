FROM golang:1.19 as builder

ARG KSM_VERSION

WORKDIR /src/ksm
ADD https://github.com/kubernetes/kube-state-metrics/archive/refs/tags/v${KSM_VERSION}.tar.gz /ksm.tar.gz
RUN tar -xz --strip-components=1 --directory=/src/ksm --file=/ksm.tar.gz

# fix compatibility issue with golang 1.19
RUN go get github.com/emicklei/go-restful/v3@v3.9.0
RUN go mod vendor

RUN make build-local

FROM scratch
COPY --from=builder /src/ksm/kube-state-metrics /

USER nobody

ENTRYPOINT ["/kube-state-metrics", "--port=8080", "--telemetry-port=8081"]

EXPOSE 8080 8081