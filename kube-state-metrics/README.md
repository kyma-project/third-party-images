# KSM Docker Image

The official image is located [here](https://github.com/kubernetes/kube-state-metrics/blob/master/Dockerfile).

This custom image mitigates the security vulnerabilities that the official image has.

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t kube-state-metrics:local --build-arg KSM_VERSION=XXX --build-arg GOLANG_VERSION=XXX
```
