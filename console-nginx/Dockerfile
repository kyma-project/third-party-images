FROM alpine:3.13.5

LABEL source git@github.com:kyma-project/console.git
LABEL application nginx 

RUN apk --no-cache upgrade &&\
    apk --no-cache add nginx=1.18.0-r13
    
RUN touch /var/run/nginx.pid && \
  chown -R nginx:nginx /var/run/nginx.pid

EXPOSE 80

ENTRYPOINT ["nginx", "-g", "daemon off;"]
