# Fluent Bit Docker Image

This image is based on the official [Fluent Bit Docker Image](https://github.com/fluent/fluent-bit-docker-image).

We add modifications so that we use the latest `debian:testing-slim` variant, as opposed to the upstream distroless image, which is always `debian:stable`-based.

The custom plugin has the following customizations:

* Loki output plugin
* Sequential HTTP output plugin

## Sequential HTTP Output Plugin
The standard HTTP output plugin `out-http` sends out records in batch. This is a problem for some consuming services in SKR environment (e.g. SAP Audit Service) 
The `out-sequentialhttp` is a drop-in replacement that sends out records sequentially (one-by-one).

### Code Modifications
The code of `out-sequentialhttp` is almost identical to `out-http`. The only difference is how msgpack to JSON transcoding / sending to the HTTP backend is done in the `cb_http_flush` function.  

### Functional Testing

You can test the `out-sequentialhttp` plugin on an SKR cluster by deploying a mock HTTP server and making the `sequentialhttp` plugin send the logs to the mock.

1. Deploy a mock server in the `kyma-system` namespace:
```
kubectl create -f https://raw.githubusercontent.com/istio/istio/master/samples/httpbin/httpbin.yaml
```

2. Edit the fluentbit configuration (`logging-fluent-bit-config` configmap) to send dummy logs to the mock server by replacing the following plugins in the `extra.conf` section:
* `http` output plugin by the `sequentialhttp` plugin
* `tail` input plugin by the `dummy` plugin in order to simulate sending audit log at a high rate (otherwise no batching would occur).
```
      [INPUT]
              Name              dummy
              Tag               dex.log
              Rate              10
              Dummy             {"log":"{\"level\":\"info\",\"msg\":\"login successful: connector \"xsuaa\"\", \"username\":\"john.doe@sap.com\", \"preferred_username\":\"\", \"email\":\"john.doe@sap.com\", \"groups\":[\"tenantID=56b23cc1-d021-4344-9c24-bace8883b864\"],\"time\":\"2021-01-11T10:29:31Z\"}"}
      [OUTPUT]
              Name             sequentialhttp
              Match            dex.*
              Retry_Limit      False
              Host             httpbin
              Port             8000
              URI              /anything
              Header           Content-Type application/json
              Format           json_stream
              tls              off
```

3. Edit the fluentbit daemonset to make it use a custom image: `eu.gcr.io/kyma-project/incubator/pr/fluent-bit:1.5.7-PR-48`

4. Make dex generate logs (for example by logging into Kyma console multiple times)Check the logs of the fluentbit pod which run on the same node with dex. You should see log entries produced by `sequentialhttp`. Ensure that every requests delivers exactly one log entry.

5. Edit the fluentbit configuration (`logging-fluent-bit-config` configmap) and replace `sequentialhttp` with `http` (the old plugin). Restart the pod you observed in the previous step. Check the logs. You should see log entries produced by `http` and the requests will contain batched log entries. 

### Load Testing
Edit the fluentbit configuration (`logging-fluent-bit-config` configmap) to send dummy audit logs at high rate by replacing the existing `tail` plugin with the following `dummy` plugin:
```
    [INPUT]
            Name              dummy
            Tag               dex.log
            Rate              10
            Dummy             {"log":"{\"level\":\"info\",\"msg\":\"login successful: connector \"xsuaa\"\", \"username\":\"john.doe@sap.com\", \"preferred_username\":\"\", \"email\":\"john.doe@sap.com\", \"groups\":[\"tenantID=56b23cc1-d021-4344-9c24-bace8883b864\"],\"time\":\"2021-01-11T10:29:31Z\"}"}
```
Observe memory usage of the pod in Grafana and make sure that memory consumption does not grow over time.
