# Builds the static Go image (1.18.1-alpine3.15) to execute in a Kubernetes job
FROM eu.gcr.io/kyma-project/external/golang@sha256:f94174c5262af3d8446833277aa27af400fd1a880277d43ec436df06ef3bb8ab as build

ENV GO111MODULE=auto
ENV DISTRIBUTION_DIR /go/src/github.com/docker/distribution
ENV BUILDTAGS include_oss include_gcs

ARG GOOS=linux
ARG GOARCH=amd64
ARG GOARM=6

RUN set -ex \
    && apk upgrade --no-cache \
    && apk add --no-cache make git file

WORKDIR $DISTRIBUTION_DIR
RUN git clone -b release/2.7 https://github.com/distribution/distribution $DISTRIBUTION_DIR

RUN CGO_ENABLED=0 make PREFIX=/go clean binaries && file ./bin/registry | grep "statically linked"

# Build result image base on alpine:3.15.4
FROM eu.gcr.io/kyma-project/external/alpine@sha256:a777c9c66ba177ccfea23f2a216ff6721e78a662cd17019488c417135299cd89

RUN set -ex \
    && apk upgrade --no-cache \
    && apk add --no-cache ca-certificates apache2-utils

COPY --from=build /go/src/github.com/docker/distribution/cmd/registry/config-dev.yml /etc/docker/registry/config.yml
COPY --from=build /go/src/github.com/docker/distribution/bin/registry /bin/registry

VOLUME ["/var/lib/registry"]
EXPOSE 5000

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["registry"]
CMD ["serve", "/etc/docker/registry/config.yml"]
