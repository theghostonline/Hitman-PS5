# hitman-server: HITMAN 3 (PS5) local online service.
#
#   make                 PS5 payload, hitman-server.elf (ps5-payload-sdk v0.43+ with the jansson/sqlite3 ports)
#   make host            the same server for macOS/Linux, build/hitman-server-host
#   make test            end-to-end protocol test against the host build
#   make deploy          upload to Payload Manager and start it (PS5_HOST, PM_PORT)

# The payload links the SDK's ported jansson and sqlite3, so use the first SDK that has them.
SDK_CANDIDATES := $(PS5_PAYLOAD_SDK) $(HOME)/ps5sdk/opt/ps5-payload-sdk
PS5_SDK ?= $(patsubst %/target/user/homebrew/include/jansson.h,%,$(firstword \
             $(foreach s,$(SDK_CANDIDATES),$(wildcard $(s)/target/user/homebrew/include/jansson.h))))
LLVM_CONFIG     ?= $(firstword $(wildcard /opt/homebrew/opt/llvm/bin/llvm-config /usr/local/opt/llvm/bin/llvm-config) llvm-config)
export LLVM_CONFIG

PS5_HOST ?= 192.168.0.37
PM_PORT  ?= 8084

ELF  := hitman-server.elf
HOST := build/hitman-server-host

SRCS := src/main.c src/config.c src/log.c src/util.c src/http.c src/proxy.c src/db.c src/handlers.c src/patcher.c
DEPS := $(SRCS) src/hs.h build/schema_sql.h

CFLAGS ?= -O2 -g
CFLAGS += -std=gnu11 -Wall -Wextra -Wno-unused-parameter -Werror -Isrc -Ibuild

# Host libraries: SQLite from the system, jansson from Homebrew or the distro.
HOST_CFLAGS  ?= $(shell pkg-config --cflags jansson 2>/dev/null || echo -I/opt/homebrew/include)
HOST_LDFLAGS ?= $(shell pkg-config --libs-only-L jansson 2>/dev/null || echo -L/opt/homebrew/lib)

all: $(ELF)

build/schema_sql.h: db/schema.sql
	@mkdir -p build
	{ printf 'static const char SCHEMA_SQL[] =\n'; \
	  sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/^/  "/' -e 's/$$/\\n"/' $<; \
	  printf ';\n'; } > $@

$(ELF): $(DEPS)
	@test -n "$(PS5_SDK)" || { echo "no ps5-payload-sdk with the jansson/sqlite3 ports; set PS5_SDK"; exit 1; }
	$(PS5_SDK)/bin/prospero-clang $(CFLAGS) -isystem $(PS5_SDK)/target/user/homebrew/include \
	  -o $@ $(SRCS) -ljansson -lsqlite3 -lSceUserService

host: $(HOST)

$(HOST): $(DEPS)
	$(CC) $(CFLAGS) $(HOST_CFLAGS) -o $@ $(SRCS) $(HOST_LDFLAGS) -ljansson -lsqlite3 -lpthread

test: $(HOST) stubtest
	python3 -I test/flow_test.py $(HOST)
	python3 -I test/proxy_test.py $(HOST)

# The add-on entitlement stub (x86stub.h) runs on the host: x86-64 natively, Rosetta on arm64.
stubtest: build/stub_test
	./build/stub_test

build/stub_test: test/stub_test.c src/x86stub.h
	@mkdir -p build
	$(CC) -arch x86_64 -O1 -g -fno-omit-frame-pointer -Wall -Wextra -Werror -Isrc -o $@ test/stub_test.c

# Backs up the console's ELF/config/db/log, uploads and verifies the ELF (Payload Manager 0.6.x,
# payloads/local/hitman-server/), the config (CONFIG=path, optional) and the H3 package, then
# starts it; the new instance takes over from the running one. See ../scripts/deploy-console.sh,
# which also has "rollback <backup dir>".
deploy: $(ELF)
	PS5_HOST=$(PS5_HOST) PM_PORT=$(PM_PORT) ../scripts/deploy-console.sh deploy $(CONFIG)

clean:
	rm -rf build $(ELF)

.PHONY: all host test stubtest deploy clean
