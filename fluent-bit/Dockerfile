ARG FLB_VERSION

FROM fluent/fluent-bit:${FLB_VERSION}

LABEL source=git@github.com:kyma-project/kyma.git
CMD ["/fluent-bit/bin/fluent-bit", "-c", "/fluent-bit/etc/fluent-bit.conf"]
