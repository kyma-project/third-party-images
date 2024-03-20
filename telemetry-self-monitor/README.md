# Telemetry Self Monitor Docker Image

The container image is built based on latest [prometheus lts image](https://prometheus.io/docs/introduction/release-cycle/).

Additionally, there is [plugins.yml](./plugins.yml) which contains the list of the plugins required. 

## Build locally

To build the image locally, execute the following command, entering the proper versions taken from the `envs` file:
```
docker build -t telemetry-self-monitor:local --build-arg PROMETHEUS_VERSION=XXX --build-arg ALPINE_VERSION=XXX .
```
