FROM alpine:20200917

# Commit details

ARG commit
ENV IMAGE_COMMIT=$commit
LABEL io.kyma-project.third-party-images.commit=$commit

LABEL source=git@github.com:kyma-project/third-party-images.git

ENV HELM_2_VERSION="v2.16.12"
ENV HELM_3_VERSION="v3.3.4"
ENV KUBECTL_VERSION="v1.19.4"

RUN apk --no-cache upgrade &&\
    apk --no-cache add openssl coreutils curl bash jq grep &&\
    wget https://storage.googleapis.com/kubernetes-release/release/${KUBECTL_VERSION}/bin/linux/amd64/kubectl -O /usr/local/bin/kubectl &&\
    chmod +x /usr/local/bin/kubectl &&\
    wget https://storage.googleapis.com/kubernetes-helm/helm-${HELM_2_VERSION}-linux-amd64.tar.gz -O - | tar -xzO linux-amd64/helm > /usr/local/bin/helm &&\
    chmod +x /usr/local/bin/helm &&\
    wget https://get.helm.sh/helm-${HELM_3_VERSION}-linux-amd64.tar.gz -O - | tar -xzO linux-amd64/helm > /usr/local/bin/helm3 &&\
    chmod +x /usr/local/bin/helm3

ENTRYPOINT ["/bin/bash"]
