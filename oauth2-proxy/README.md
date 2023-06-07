# oauth2-proxy Docker Image

The official Docker image is located [here](https://github.com/oauth2-proxy/oauth2-proxy/blob/v7.2.1/Dockerfile).

This custom image has been created to mitigate the security vulnerabilities the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t oauth2-proxy:local --build-arg OAUTH2_PROXY_VERSION=XXX --build-arg GOLANG_VERSION=XXX --build-arg ALPINE_VERSION=XXX
```
