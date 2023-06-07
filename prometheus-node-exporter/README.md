# Prometheus Node Exporter Docker Image

The official image is located [here](https://github.com/prometheus/node_exporter/blob/master/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t node-exporter:local --build-arg NODE_EXPORTER_VERSION=XXX --build-arg GOLANG_VERSION=XXX
```
