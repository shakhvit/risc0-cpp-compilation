ROOT_DIR:=$(strip $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

ifeq ($(OS),Windows_NT)
$(error Windows not supported)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        MACHINE = linux
    endif
    ifeq ($(UNAME_S),Darwin)
        MACHINE = osx
    endif

    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        MACHINE := $(MACHINE)-x86_64
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        MACHINE := $(MACHINE)-arm64
    endif
endif

default: execute

.PHONY: platform
platform:
	cargo +risc0 rustc -p zkvm-platform --target riscv32im-risc0-zkvm-elf --lib --crate-type staticlib --release --target-dir ./guest/out/platform

# Regenerate the header file. Assumes cbindgen installed locally.
.PHONY: headers
headers:
    # Note: this is currently broken on main, as there is a FFI incompatible type
	cbindgen --crate zkvm-platform -c ./platform/cbindgen.toml -o ./guest/platform.h

.PHONY: gcc
gcc:
	scripts/install-toolchain.sh

.PHONY: guest
guest: gcc platform

	${ROOT_DIR}riscv32im-${MACHINE}/bin/riscv32-unknown-elf-g++ ${CPPFLAGS}\
		-nostartfiles ./guest/main.cpp -o ./guest/out/main \
		-L${ROOT_DIR}external/libs/libsodium/lib -lsodium -I${ROOT_DIR}external/libs/libsodium/include \
		-L${ROOT_DIR}external/libs/libutf8proc/lib -lutf8proc -I${ROOT_DIR}external/libs/libutf8proc/include \
		-L${ROOT_DIR}guest/out/platform/riscv32im-risc0-zkvm-elf/release -lzkvm_platform -T ./guest/riscv32im-risc0-zkvm-elf.ld


.PHONY: execute
execute: guest
	RISC0_DEV_MODE=true cargo run -p c-guest-host

.PHONY: prove
prove: guest
	cargo run -p c-guest-host
