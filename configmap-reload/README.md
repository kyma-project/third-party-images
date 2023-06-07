# Configmap-reload Image

The official image is located [here](https://github.com/jimmidyson/configmap-reload/blob/main/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t configmap-reload:local --build-arg CONFIGMAP_RELOAD_VERSION=XXX --build-arg GOLANG_VERSION=XXX --build-arg BUSYBOX_VERSION=XXX
```
