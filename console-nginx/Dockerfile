FROM alpine:3.13.1

LABEL source git@github.com:kyma-project/console.git

RUN apk add --update nginx=1.18.0-r13 && rm -rf /var/cache/apk/*
RUN touch /var/run/nginx.pid && \
  chown -R nginx:nginx /var/run/nginx.pid

EXPOSE 80

ENTRYPOINT ["nginx", "-g", "daemon off;"]