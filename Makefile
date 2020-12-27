DOCKER_IMAGE = emscripten_builder
CURRENT_DIR = $(shell pwd)

build_docker:
	docker build -t $(DOCKER_IMAGE) .

web_reload:
	docker run --rm -v $(CURRENT_DIR):/src -u $(shell id -u):$(shell id -g) $(DOCKER_IMAGE) emcmake cmake -Bcmake-build-web -DPLATFORM=Web

web_build: web_reload
	docker run --rm -v $(CURRENT_DIR):/src -u $(shell id -u):$(shell id -g) $(DOCKER_IMAGE) cmake --build cmake-build-web -j 4