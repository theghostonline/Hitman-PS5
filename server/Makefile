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

SRCS := src/main.c src/config.c src/log.c src/util.c src/http.c src/db.c src/handlers.c src/patcher.c
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

test: $(HOST)
	python3 -I test/flow_test.py $(HOST)

# Payload Manager 0.6.x keeps uploads in /data/pldmgr/payloads/local/<name>/ and wants its request
# marker on every change. A new instance asks the running one to exit before it binds the port.
PM      := http://$(PS5_HOST):$(PM_PORT)
PM_HDRS := -H "X-PLDMGR-Request: 1" -H "Origin: $(PM)"

deploy: $(ELF)
	@mode=$$(curl -fsS $(PM_HDRS) "$(PM)/manage:check?filename=$(ELF)" | grep -q '"file_exists":true' && echo replace || echo new); \
	curl -fsS -X POST $(PM_HDRS) -H "Content-Type: application/octet-stream" --data-binary @$(ELF) \
	  "$(PM)/manage:upload?filename=$(ELF)&mode=$$mode" && echo " uploaded ($$mode)"
	@curl -fsS -X POST $(PM_HDRS) "$(PM)/loadpayload:/data/pldmgr/payloads/local/hitman-server/$(ELF)" && echo " started"
	@sleep 3; curl -fsS "http://$(PS5_HOST):8000/healthz"; echo

clean:
	rm -rf build $(ELF)

.PHONY: all host test deploy clean
