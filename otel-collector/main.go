package main

import (
	"github.com/open-telemetry/opentelemetry-collector-contrib/receiver/opencensusreceiver"
	"go.opentelemetry.io/collector/exporter/loggingexporter"
	"go.opentelemetry.io/collector/exporter/otlpexporter"
	"go.opentelemetry.io/collector/exporter/otlphttpexporter"
	"go.opentelemetry.io/collector/processor/batchprocessor"
	"go.opentelemetry.io/collector/processor/memorylimiterprocessor"
	"go.opentelemetry.io/collector/receiver/otlpreceiver"
	"log"

	"go.opentelemetry.io/collector/component"
	"go.opentelemetry.io/collector/service"
)

func main() {
	factories, err := components()
	if err != nil {
		log.Fatalf("failed to build components: %v", err)
	}

	info := component.BuildInfo{
		Command:     "otelcol",
		Description: "Custom OpenTelemetry Collector distribution for Kyma",
		Version:     "0.65.0",
	}

	if err := run(service.CollectorSettings{
		BuildInfo: info,
		Factories: factories,
	}); err != nil {
		log.Fatal(err)
	}
}

func run(params service.CollectorSettings) error {
	return runInteractive(params)
}

func runInteractive(params service.CollectorSettings) error {
	// creates default config provider
	cmd := service.NewCommand(params)
	if err := cmd.Execute(); err != nil {
		log.Fatalf("collector server run finished with error: %v", err)
	}

	return nil
}

func components() (component.Factories, error) {
	var err error
	factories := component.Factories{}

	if err != nil {
		return component.Factories{}, err
	}

	// Initialize receivers:
	// 1. Opencensus
	// 2. Otlp/Otlp http
	factories.Receivers, err = component.MakeReceiverFactoryMap(
		opencensusreceiver.NewFactory(),
		otlpreceiver.NewFactory())
	if err != nil {
		return component.Factories{}, err
	}

	// Initialize Exporters
	// 1. Logging exporter
	// 2. Otlp
	// 3. Otlp http
	factories.Exporters, err = component.MakeExporterFactoryMap(
		loggingexporter.NewFactory(),
		otlpexporter.NewFactory(),
		otlphttpexporter.NewFactory(),
	)
	if err != nil {
		return component.Factories{}, err
	}

	// Initialize Processors
	// 1. Batch processor
	// 2. Memory limit processor
	factories.Processors, err = component.MakeProcessorFactoryMap(
		batchprocessor.NewFactory(),
		memorylimiterprocessor.NewFactory(),
	)

	if err != nil {
		return component.Factories{}, err
	}

	return factories, nil
}
