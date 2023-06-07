# Loki Docker Image

The official Docker image is located [here](https://github.com/grafana/loki/blob/v2.2.1/cmd/loki/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t loki:local --build-arg LOKI_VERSION=XXX --build-arg GOLANG_VERSION=XXX --build-arg ALPINE_VERSION=XXX
```
