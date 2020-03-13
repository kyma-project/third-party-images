FROM alpine:3.11

LABEL source=git@github.com:kyma-project/kyma.git
COPY --from=minio/mc:RELEASE.2019-10-09T22-54-57Z /usr/bin/mc /usr/bin/mc

ENTRYPOINT ["mc"]
