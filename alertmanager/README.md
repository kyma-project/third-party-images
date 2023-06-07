# Alertmanager Docker Image

The official image is located [here](https://github.com/prometheus/alertmanager/blob/main/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t alertmanager:local --build-arg ALERTMANAGER_VERSION=XXX --build-arg GOLANG_VERSION=XXX --build-arg ALPINE_VERSION=XXX
```
