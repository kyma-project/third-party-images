# OTEL Collector Docker Image

The Docker image is built using the [otel builder binary](https://github.com/open-telemetry/opentelemetry-collector/tree/main/cmd/builder) based on [builder-config.yaml](https://github.com/open-telemetry/opentelemetry-collector/blob/main/cmd/otelcorecol/builder-config.yaml).

This custom image has been created to mitigate the security vulnerabilities and have required set of receivers, processors and exporters.