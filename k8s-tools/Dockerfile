# result image base on alpine:3.16.2
FROM alpine@sha256:bc41182d7ef5ffc53a40b044e725193bc10142a1243f395ee852a8d9730fc2ad

# Commit details

ARG commit
ENV IMAGE_COMMIT=$commit
LABEL io.kyma-project.third-party-images.commit=$commit

LABEL source=git@github.com:kyma-project/third-party-images.git

ENV KUBECTL_VERSION="v1.23.9"

RUN apk --no-cache upgrade &&\
    apk --no-cache add wget openssl coreutils curl bash jq grep inotify-tools &&\
    wget https://storage.googleapis.com/kubernetes-release/release/${KUBECTL_VERSION}/bin/linux/amd64/kubectl -O /usr/local/bin/kubectl &&\
    chmod +x /usr/local/bin/kubectl

ENTRYPOINT ["/bin/bash"]
