FROM golang:1.13-alpine as builder

ENV KIALI_VERSION 1.14.0

WORKDIR /go/src/github.com/kiali

RUN apk update && apk upgrade && \
    apk add --no-cache bash git openssh curl

RUN git clone --branch v${KIALI_VERSION} https://github.com/kiali/kiali.git

RUN curl https://glide.sh/get | awk '{gsub("get TAG https://glide.sh/version", "TAG=v0.13.1", $$0); print}' | sh
RUN cd kiali && CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -ldflags="-X main.version=${KIALI_VERSION}" -o /kiali

RUN apk add --update nodejs nodejs-npm

RUN mkdir /console
RUN curl -s $(npm view @kiali/kiali-ui@${KIALI_VERSION} dist.tarball) \
    | tar zxf - --strip-components=2 --directory /console

FROM scratch

ENV KIALI_HOME=/opt/kiali \
    PATH=$KIALI_HOME:$PATH

WORKDIR $KIALI_HOME

COPY --from=builder ./kiali $KIALI_HOME/
COPY --from=builder ./console $KIALI_HOME/console/

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/opt/kiali/kiali"]
