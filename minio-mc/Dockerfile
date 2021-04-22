FROM alpine:3.13.5

LABEL source=git@github.com:kyma-project/kyma.git
COPY --from=minio/mc:RELEASE.2019-04-03T17-59-57Z /usr/bin/mc /usr/bin/mc

ENTRYPOINT ["mc"]
