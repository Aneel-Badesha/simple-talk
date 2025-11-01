# simple-talk
Simple Terminal — a small multi-threaded UDP send/receive demo that uses a simple List API.

## Overview

`simple-talk` is a tiny example program that demonstrates a threaded design for reading
keyboard input, sending UDP messages to a remote peer, receiving UDP messages on a local
port, and printing received messages to the terminal. The program components are split
into small threads:

- `keyb_in` — reads lines from stdin and enqueues them for sending.
- `udp_out` — waits for messages to become available and sends them to a remote host/port.
- `udp_in` — listens on a local UDP port and enqueues received messages for display.
- `screen_out` — dequeues received messages and writes them to stdout.

The code expects a `List` API (see `list/list.h`) for inter-thread message queues.

## Requirements

- POSIX-compatible environment (Linux, macOS). On Windows, use WSL, Cygwin, or MinGW with
	pthreads and BSD sockets compatibility.
- A C compiler (gcc/clang) and the standard POSIX headers/libraries (pthread, sockets).

Note: The repository currently contains `list/list.h` (the API) but no `list.c` implementation.
You must provide or compile an implementation of the List API (functions such as
`List_create`, `List_prepend`, `List_trim`, etc.) when building.

## Build

From the repository root, assuming you have a `list.c` implementation in `list/`:

```sh
gcc -o simple-talk main.c threads/*.c list/*.c -pthread -Wall -Wextra
```

If your `list` implementation is provided as a separate library, adjust the command to
link against that library instead.

## Usage

Run the program with three arguments:

```
./simple-talk <localport> <remote_name> <remoteport>
```

Example (listen on local port 6060 and send to remote host `example.com` port 6060):

```
./simple-talk 6060 example.com 6060
```

Type messages and press Enter to send. A message consisting of a single `!` followed by a
newline (`!\n`) is treated as a termination signal: the threads will issue cancellation
requests and the program will exit.

## Files

- `main.c` — program entry; sets up the send/receive lists and starts the four subsystems.
- `list/list.h` — List API used by the threads. You need a corresponding `list.c` to build.
- `threads/keyb_in.c`, `.h` — reads stdin in a dedicated thread and schedules outgoing messages.
- `threads/udp_out.c`, `.h` — resolves remote address and sends queued messages via UDP.
- `threads/udp_in.c`, `.h` — binds a UDP socket to the local port and enqueues incoming messages.
- `threads/screen_out.c`, `.h` — prints received messages and handles the termination signal.

## Known issues & notes

- The project uses POSIX `pthread` and UNIX socket APIs. It will not build on native Win32
	without a POSIX compatibility layer (use WSL or similar).
- The `List` implementation (source) is not included in the repository; add it to `list/` or
	link with an existing implementation before building.
