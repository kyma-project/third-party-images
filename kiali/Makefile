APP_NAME ?= kiali
KIALI_VERSION = 1.33.1
TAG = $(KIALI_VERSION)-$(DOCKER_TAG)
IMG = $(DOCKER_PUSH_REPOSITORY)$(DOCKER_PUSH_DIRECTORY)/$(APP_NAME):$(TAG)
IMG_DEFAULT_TAG = $(DOCKER_PUSH_REPOSITORY)$(DOCKER_PUSH_DIRECTORY)/$(APP_NAME):$(DOCKER_TAG)

# Use generic makefile
COMMON_DIR = $(realpath $(shell pwd)/../)/common
include $(COMMON_DIR)/generic_makefile.mk

# override docker-build
docker-build:
	docker build . -t ${IMG} --build-arg KIALI_VERSION=$(KIALI_VERSION)
