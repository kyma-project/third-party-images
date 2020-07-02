FROM kiwigrid/k8s-sidecar:0.1.157 as builder

FROM python:3.9.0b3-alpine3.12

ENV         PYTHONUNBUFFERED=1
WORKDIR     /app
RUN echo "kubernetes==10.0.0" >> requirements.txt
RUN echo "requests==2.22.0" >> requirements.txt

COPY        --from=builder /app/* /app/
RUN         apk add --no-cache gcc && \
	    pip install -r requirements.txt && \
	    apk del -r gcc && \
            rm -rf /var/cache/apk/* requirements.txt

USER 65534:65534

LABEL source=git@github.com:kyma-project/kyma.git

CMD         [ "python", "-u", "/app/sidecar.py" ]
