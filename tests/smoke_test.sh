#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP_DIR=$(mktemp -d)
PIDS=()
BASE_PORT=${PORT_BASE:-43120}

cleanup() {
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null || true
        wait "$pid" 2>/dev/null || true
    done
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT

start_server() {
    "$@" >"$TMP_DIR/server.log" 2>&1 &
    PIDS+=("$!")
    sleep 0.1
}

start_server_in() {
    local directory=$1
    shift
    (cd "$directory" && exec "$@") >"$TMP_DIR/server.log" 2>&1 &
    PIDS+=("$!")
    sleep 0.1
}

stop_server() {
    local index=$((${#PIDS[@]} - 1))
    local pid=${PIDS[$index]}
    kill "$pid" 2>/dev/null || true
    wait "$pid" 2>/dev/null || true
    unset 'PIDS[index]'
}

start_server "$ROOT/bin/tcp_hello_server" "$((BASE_PORT + 1))"
"$ROOT/bin/tcp_hello_client" 127.0.0.1 "$((BASE_PORT + 1))" | grep -q "Hello"
stop_server

start_server "$ROOT/bin/tcp_echo_server" "$((BASE_PORT + 2))"
printf 'echo smoke test\n' | "$ROOT/bin/tcp_echo_client" 127.0.0.1 "$((BASE_PORT + 2))" | grep -q "echo smoke test"
stop_server

start_server "$ROOT/bin/udp_time_server" "$((BASE_PORT + 3))"
"$ROOT/bin/udp_time_client" 127.0.0.1 "$((BASE_PORT + 3))" | grep -Eq '[0-9]{4}'
stop_server

start_server_in "$ROOT/examples/tcp/file-transfer" "$ROOT/bin/tcp_file_server" "$((BASE_PORT + 4))"
(
    cd "$TMP_DIR"
    printf 'test.txt\n' | "$ROOT/bin/tcp_file_client" 127.0.0.1 "$((BASE_PORT + 4))"
)
cmp "$ROOT/examples/tcp/file-transfer/test.txt" "$TMP_DIR/test.txt"
stop_server

start_server_in "$ROOT/examples/tcp/file-transfer" "$ROOT/bin/udp_file_server" "$((BASE_PORT + 5))"
(
    cd "$TMP_DIR"
    printf 'test.txt\nquit\n' | "$ROOT/bin/udp_file_client" 127.0.0.1 "$((BASE_PORT + 5))"
)
cmp "$ROOT/examples/tcp/file-transfer/test.txt" "$TMP_DIR/output.txt"
stop_server

echo "All networking smoke tests passed."
