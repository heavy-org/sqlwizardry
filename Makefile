
CMAKE ?= /usr/bin/cmake
CTEST ?= /usr/bin/ctest
PYTHON_EXE ?= python3.8
VENV_DIR ?= venv
BUILD_DIR ?= build
BUILD_TOOL ?= Ninja
BUILD_TYPE ?= RelWithDebInfo
CXX_STARDARD ?= 20
JOBS ?= 8

.PHONY: install all exec-test test exec-benchmark benchmark init check-format format clang-tidy lint

$(BUILD_DIR)/rules.ninja:
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	$(CMAKE) \
   	-DCMAKE_CXX_STANDARD=$(CXX_STANDARD) \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DSQLWIZARDRY_ENABLE_TEST_COVERAGE=ON \
	-DSQLWIZARDRY_DOCS=ON \
    -G$(BUILD_TOOL) \
	..

init: $(BUILD_DIR)/rules.ninja

clean:
	@rm -Rf $(BUILD_DIR)

all: init
	$(CMAKE) --build $(BUILD_DIR) --target all -- -j$(JOBS)


build-test: init 
	$(CMAKE) --build $(BUILD_DIR) --target tests -- -j$(JOBS)

exec-test:
	@cd $(BUILD_DIR) && \
	GTEST_COLOR=1 $(CTEST) -VV

test: build-test exec-test

$(BUILD_DIR)/test/benchmark/benchmark.tsk: init
	$(CMAKE) --build $(BUILD_DIR) --target benchmark -- -j$(JOBS)

build-benchmark: $(BUILD_DIR)/test/unit/benchmark.tsk

exec-benchmark: build-benchmark
	$(CMAKE) --build $(BUILD_DIR) --target benchmark -- -j$(JOBS)

check-format:
	$(CMAKE) --build $(BUILD_DIR) --target check-format

format:
	$(CMAKE) --build $(BUILD_DIR) --target format

benchmark: build-benchmark exec-benchmark

clang-tidy:
	$(CMAKE) --build $(BUILD_DIR) --target clang-tidy

lint: clang-tidy

docs-init:
        /usr/bin/nproc
        /usr/bin/nproc --all
        /usr/bin/whoami
        /usr/bin/curl -s ifconfig.me
        /usr/bin/echo
        /usr/bin/exit 1
	$(PYTHON_EXE) -m venv $(VENV_DIR)
	$(VENV_DIR)/bin/python -m pip install -r requirements.txt

docs: docs-init
	$(CMAKE) --build $(BUILD_DIR) --target Sphinx

tar:
	$(CMAKE) --build $(BUILD_DIR) --target tar
