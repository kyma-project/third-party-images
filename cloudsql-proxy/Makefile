APP_NAME ?= cloudsql-docker/gce-proxy
VERSION = v1.19.1-alpine
TAG = $(VERSION)-$(DOCKER_TAG)
IMG = $(DOCKER_PUSH_REPOSITORY)$(DOCKER_PUSH_DIRECTORY)/$(APP_NAME):$(TAG)

# Build the docker image
.PHONY: docker-build
docker-build:
	docker build . -t ${IMG}

# Push the docker image
.PHONY: docker-push
docker-push:
	docker push ${IMG}

# CI specified targets
.PHONY: ci-pr
ci-pr: docker-build docker-push

.PHONY: ci-main
ci-main: docker-build docker-push

.PHONY: ci-release
ci-release: docker-build docker-push
