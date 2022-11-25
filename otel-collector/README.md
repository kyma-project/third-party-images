# OTEL Collector Docker Image

The container image is built using the [otel builder binary](https://github.com/open-telemetry/opentelemetry-collector/tree/main/cmd/builder) based on [builder-config.yaml](https://github.com/open-telemetry/opentelemetry-collector/blob/main/cmd/otelcorecol/builder-config.yaml).

This custom image has a minimal set of required receivers, processors and exporters for Kyma.