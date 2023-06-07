# Grafana Docker Image

The official image is located [here](https://github.com/grafana/grafana/blob/v7.5.12/packaging/docker/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t grafana:local --build-arg GRAFANA_VERSION=XXX --build-arg GOLANG_VERSION=XXX --build-arg ALPINE_VERSION=XXX --build-arg NODE_VERSION=XXX
```
