FROM jaegertracing/jaeger-operator:1.22.1 as builder

FROM scratch

ENV OPERATOR=/usr/local/bin/jaeger-operator \
    USER_UID=1001 \
    USER_NAME=jaeger-operator

COPY --from=builder ${OPERATOR} ${OPERATOR}
COPY --from=builder /scripts/ /scripts/

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/usr/local/bin/jaeger-operator"]

USER ${USER_UID}
