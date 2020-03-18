FROM kubeless/function-image-builder:v1.0.5 as builder

FROM node:13.10.1-alpine3.11

LABEL source=git@github.com:kyma-project/kyma.git

RUN apk update && apk upgrade
RUN apk add --no-cache ca-certificates skopeo 

COPY --from=builder /imbuilder /imbuilder
COPY --from=builder /entrypoint.sh /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
