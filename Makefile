CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
BIN_DIR := bin

TARGETS := \
	$(BIN_DIR)/tcp_hello_server \
	$(BIN_DIR)/tcp_hello_client \
	$(BIN_DIR)/tcp_echo_server \
	$(BIN_DIR)/tcp_echo_client \
	$(BIN_DIR)/tcp_file_server \
	$(BIN_DIR)/tcp_file_client \
	$(BIN_DIR)/udp_time_server \
	$(BIN_DIR)/udp_time_client \
	$(BIN_DIR)/udp_file_server \
	$(BIN_DIR)/udp_file_client

.PHONY: all clean test

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/tcp_hello_server: examples/tcp/hello/hello_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tcp_hello_client: examples/tcp/hello/hello_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tcp_echo_server: examples/tcp/echo/echo_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tcp_echo_client: examples/tcp/echo/echo_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tcp_file_server: examples/tcp/file-transfer/file_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/tcp_file_client: examples/tcp/file-transfer/file_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/udp_time_server: examples/udp/time/time_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/udp_time_client: examples/udp/time/time_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/udp_file_server: examples/udp/file-transfer/udp_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/udp_file_client: examples/udp/file-transfer/udp_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)

test: all
	./tests/smoke_test.sh
