FROM quay.io/kiali/kiali:v1.17.0 as builder

FROM alpine:3.11

ENV KIALI_HOME=/opt/kiali \
    PATH=$KIALI_HOME:$PATH

WORKDIR $KIALI_HOME

COPY --from=builder $KIALI_HOME/ $KIALI_HOME/
COPY --from=builder $KIALI_HOME/console/ $KIALI_HOME/console/

RUN apk add --no-cache bash

RUN chgrp -R 0 $KIALI_HOME/console && \
    chmod -R g=u $KIALI_HOME/console

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/opt/kiali/kiali"]
