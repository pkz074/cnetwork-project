# C Network Programming Examples

A collection of small client-server programs that demonstrate core TCP and UDP socket programming in C. The examples cover connection-oriented services, datagram services, concurrent servers, hostname resolution, and file transfer.

## What is included

| Example | Transport | Demonstrates |
| --- | --- | --- |
| Hello service | TCP | Client connection and a one-way server response |
| Echo service | TCP | Bidirectional I/O and a process-per-client server |
| File transfer | TCP | Status framing and chunked binary transfer |
| Time service | UDP | Request-response datagrams and hostname lookup |
| File transfer | UDP | A simple protocol data unit with data, final, and error types |

## Build

Requirements: a POSIX-like environment, a C11-compatible compiler, and `make`.

```sh
make
```

Executables are written to `bin/`. Build one example with a specific target, such as `make bin/tcp_echo_server`, or remove generated files with `make clean`.

Run the localhost smoke tests for all five client-server pairs:

```sh
make test
```

## Run the examples

Each service uses port `3000` by default. Start its server in one terminal, then its client in another.

### TCP hello

```sh
./bin/tcp_hello_server [port]
./bin/tcp_hello_client 127.0.0.1 [port]
```

### TCP echo

```sh
./bin/tcp_echo_server [port]
./bin/tcp_echo_client 127.0.0.1 [port]
```

Enter text in the client and press `Ctrl-D` to close the session.

### TCP file transfer

```sh
cd examples/tcp/file-transfer
../../../bin/tcp_file_server [port]
../../../bin/tcp_file_client 127.0.0.1 [port]
```

The server makes files from its current directory available. The client prompts for a filename and writes the response to its own current directory. Run the client from a different directory when both programs are on the same machine to avoid overwriting the source file.

### UDP time

```sh
./bin/udp_time_server [port]
./bin/udp_time_client 127.0.0.1 [port]
```

### UDP file transfer

```sh
cd examples/udp/file-transfer
../../../bin/udp_file_server [port]
../../../bin/udp_file_client 127.0.0.1 [port]
```

The client writes received data to `output.txt`. Type `quit` to exit.

## Repository structure

```text
.
├── examples/
│   ├── tcp/
│   │   ├── hello/
│   │   ├── echo/
│   │   └── file-transfer/
│   └── udp/
│       ├── time/
│       └── file-transfer/
├── Makefile
└── README.md
```

## Scope

These programs focus on socket fundamentals rather than production protocol design. They assume trusted peers and do not provide authentication, encryption, path isolation, UDP retransmission, or congestion control.
