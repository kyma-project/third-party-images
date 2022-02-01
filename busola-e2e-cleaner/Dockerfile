FROM alpine:3.13.6

RUN apk add --update nodejs npm

WORKDIR /app

COPY . /app

RUN npm i --prefix=app

ENTRYPOINT ["node", "index.js"]
