# memtrace-lite

Lightweight memory usage tracer for Linux processes with minimal overhead.

## Overview

`memtrace-lite` hooks into a running Linux process and tracks heap allocations, frees, and peak memory usage — with negligible runtime cost compared to heavier tools like Valgrind.

## Requirements

- Linux (kernel 4.x+)
- GCC or Clang
- `libelf` and `libdwarf` (optional, for symbol resolution)

## Installation

```bash
git clone https://github.com/yourname/memtrace-lite.git
cd memtrace-lite && make && sudo make install
```

## Usage

Prefix any command with `memtrace-lite` to begin tracing:

```bash
memtrace-lite ./your_program --args
```

Attach to an already-running process by PID:

```bash
memtrace-lite --pid 1234
```

### Example Output

```
[memtrace-lite] PID: 1234 | ./your_program
──────────────────────────────────────────
 Total allocations : 4,821
 Total frees       : 4,819
 Leaked blocks     : 2  (48 bytes)
 Peak heap usage   : 3.2 MB
 Trace duration    : 2.04s
──────────────────────────────────────────
[memtrace-lite] Done.
```

### Options

| Flag | Description |
|------|-------------|
| `--pid <PID>` | Attach to existing process |
| `--output <file>` | Write report to file |
| `--threshold <bytes>` | Only report leaks above size |
| `--verbose` | Show per-allocation details |

## Building from Source

```bash
make          # build release binary
make debug    # build with debug symbols
make test     # run unit tests
```

## License

MIT © 2024 Your Name. See [LICENSE](LICENSE) for details.