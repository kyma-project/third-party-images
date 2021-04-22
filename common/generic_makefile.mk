# Genric makefile
# Requiured variables:
# IMG - name of the image, with docker push repository, full path and tag
# IMG_DEFAULT_TAG - name of the image, with docker push repository, full path and DOCKER_TAG to satisfy Protescan requirements


# Build the docker image
.PHONY: docker-build
docker-build:
	docker build . -t ${IMG}

# Push the docker image
.PHONY: docker-push
docker-push:
	docker push ${IMG} ${IMG_DEFAULT_TAG}

# add second tag to the image
.PHONY: ci-tag
ci-tag:
	docker tag ${IMG} ${IMG_DEFAULT_TAG}

# CI specified targets
.PHONY: ci-pr
ci-pr: docker-build ci-tag docker-push

.PHONY: ci-main
ci-main: docker-build docker-push

.PHONY: ci-release
ci-release: docker-build docker-push
