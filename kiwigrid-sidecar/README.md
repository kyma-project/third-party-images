# Kiwigrid Sidecar Docker Image

The official image is located [here](https://github.com/kiwigrid/k8s-sidecar/blob/master/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t k8s-sidecar:local --build-arg KIWIGRID_SIDECAR_VERSION=XXX --build-arg PYTHON_VERSION=XXX --build-arg ALPINE_VERSION=XXX
```
