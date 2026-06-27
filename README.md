# High-Performance Limit Order Book Reconstruction

This repository contains a low-latency C++ project that reconstructs a Market-by-Price (MBP10) / Limit Order Book from Databento Market-by-Order (MBO) CSV data. The code was written with a performance-first mindset: minimize parsing overhead, keep the hot path cache-friendly, and preserve correctness while iterating toward sub-microsecond per-row processing.

## Project Snapshot

- `High Performance Limit Orderbook Reconstruction`
- `Dec 2025 - Present`
- `Low Latency Development`
- Implemented an ultra-low-latency MBP10 engine in C++, directly aggregating MBO updates.
- Developed a custom fast I/O class using a large fixed-size read buffer, manual pointer advancement, and zero-copy parsing.
- Evaluated multiple data-structure designs, including a linked-list-based Top-10 book with a priority queue for deeper levels and lazy deletion via unordered maps, before selecting a contiguous-array-based approach for superior latency.
- Achieved approximately `800 ns` per MBO update end-to-end latency on an Apple Silicon `M1` CPU, approaching hardware-limited performance for full MBP recomputation per event.
- Technologies / Tools Used: `C++`, `C++ STL`

## STAR Summary

### Situation

High-frequency market data arrives as a dense stream of order-level events. To make that data useful for research, backtesting, and execution analysis, it must be transformed into a reconstructed order book quickly and consistently. The challenge is that the reconstruction process is sequential by nature: each row can depend on the state created by the rows before it.

### Task

Build a C++ engine that can:

- read Databento-style MBO CSV files efficiently,
- reconstruct the book state row by row,
- output top-of-book / top-10 MBP snapshots,
- validate correctness against reference outputs,
- and measure performance under repeated runs.

The target benchmark for this project is approximately 800 nanoseconds per row processed on an Apple Silicon M1 MacBook Pro.

### Action

The repository explores several implementation strategies and compares them against one another.

#### 1. Baseline reconstruction path

The core reconstruction logic lives in `order.h` and `order.cpp`. The `Order` type stores the fields required to interpret a market event, while the `BruteForceMBP` implementation maintains bid/ask state and writes MBP output rows.

The baseline entry points in `main_bruteforce.cpp` and `main.cpp` show two parsing styles:

- a manual row parser that reads fields directly from each CSV line,
- and a faster mmap-based parser that uses `FastReader` to avoid repeated file I/O overhead.

#### 1a. Algorithm evolution inside the book engine

The repo went through a few reconstruction strategies before settling on the fastest one.

- The earlier prototype used a linked-list-based Top-10 book with priority queues for deeper levels and lazy deletion via unordered maps. This is the more general design and appears in the `MBPK` and `MBPK_O` experiments in `order.h`.
- The final performance-oriented path uses the contiguous-array-backed `BruteForceMBP` implementation. It keeps the top levels in fixed-size arrays and updates them directly as events arrive, which keeps the hot path simple and reduces overhead.

For this project, the contiguous-array brute-force approach worked out better. The linked-list / lazy-deletion style was useful as a correctness-oriented prototype, but the fixed-array design was faster and a better fit for the narrow top-10 reconstruction problem.

#### 2. Fast file ingestion

`fast_reader.h` implements the high-performance reader used in the optimized path. It maps the file into memory with `mmap`, walks the buffer directly, and provides specialized helpers for reading integers, prices, characters, and string views.

This avoids the overhead of `getline`, repeated string splitting, and most temporary allocations.

#### 3. Optimized CSV parsing experiment

`convert.cpp` shows an alternate parsing experiment using the header-only CSV library in `csv.h` / `csv.hpp`. It demonstrates how the same data can be ingested with a template-based parser and compared against the custom reader approach.

#### 4. Threaded comparison and pipeline design

`main_threaded.cpp` and `threaded_reconstruction.h` implement a three-stage pipeline:

- reader thread: parses the input file,
- processor thread: updates the order book,
- writer thread: aggregates formatted output.

The repo also includes `parallel_file_processor.h`, which documents where multithreading is actually a good fit: independent files or independent instruments. That distinction matters because a single order book stream is inherently sequential, so naive threading does not automatically improve throughput.

#### 5. Correctness validation

`validator.cpp` compares generated MBP output against reference data such as `mbp.csv`. This is important because performance improvements only matter if the reconstructed book remains correct.

#### 6. Benchmarking and repeated runs

Several executables repeat the same workload many times and print elapsed time in nanoseconds, microseconds, milliseconds, and seconds. This makes it easy to compare baseline, optimized, and threaded variants using the same dataset.

### Result

The project achieves approximately 800 ns per MBO update end-to-end on an Apple Silicon M1 MacBook Pro when running against the provided Databento MBO CSV data. The repo also preserves multiple output variants for comparison, including:

- `mbp10.csv`
- `mbp10_orig.csv`
- `mbp10_st.csv`
- `mbp10_mt.csv`

The main outcome is a working reconstruction pipeline that is fast enough for low-latency experimentation while still being explicit about tradeoffs between speed, simplicity, and concurrency.

### Challenges

The main implementation challenges were:

- keeping the book state correct while processing a stream of dependent events,
- handling order actions such as adds, cancels, and trades without corrupting the top levels,
- parsing CSV data fast enough without introducing allocation-heavy abstractions,
- managing price representation safely by storing prices as scaled integers instead of floating-point values in the hot path,
- and dealing with the fact that multithreading is not automatically beneficial for a single sequential order stream.

Another practical challenge was validating that optimized implementations still matched the reference output. That is why the repository keeps both performance-oriented code and comparison utilities side by side.

## Repository Layout

- `order.h` / `order.cpp`: core data model and MBP reconstruction logic.
- `fast_reader.h`: memory-mapped fast CSV reader.
- `main_bruteforce.cpp`: baseline reconstruction benchmark.
- `main_threaded.cpp`: threaded comparison benchmark.
- `threaded_reconstruction.h`: pipeline-based threaded implementation.
- `parallel_file_processor.h`: examples of where parallelism is a better fit.
- `convert.cpp`: CSV parser experiment using `csv.h` / `csv.hpp`.
- `validator.cpp`: output comparison utility.
- `mbo.csv`: sample MBO input.
- `mbp.csv`, `mbp10.csv`, `mbp10_orig.csv`, `mbp10_st.csv`, `mbp10_mt.csv`: reference and generated outputs.
- `makefile`: build rules for the available executables.

## How It Works

1. Read the MBO CSV row by row.
2. Parse the event fields needed for reconstruction.
3. Update the bid/ask state in the MBP engine.
4. Emit a new MBP row whenever the top-of-book state changes.
5. Repeat until the file is fully processed.

The implementation stores prices as integers scaled by $10^9$ so that the hot path avoids floating-point drift. Output formatting converts the stored integer price back to a decimal representation when writing CSV rows.

## Performance Notes

- Benchmark hardware: Apple Silicon M1 MacBook Pro.
- Measured throughput: approximately 800 ns per row.
- Primary optimizations: memory-mapped file input, lightweight parsing, reduced temporary allocations, and compact state updates.
- Observation: the sequential reconstruction workload often benefits more from algorithmic and parsing optimizations than from threading.

## Build and Run

### Prerequisites

- C++ compiler with C++23 support.
- `make`.

### Build

```bash
make
```

The default make target builds both the original single-threaded reconstruction binary and the threaded comparison binary.

### Run

```bash
make run_orig
```

or

```bash
make run_mt
```

You can also run the binaries directly after building:

```bash
./reconstruction
./reconstruction_mt
```

## Input Format

The project expects Databento MBO CSV input with fields such as:

- `ts_recv`
- `ts_event`
- `rtype`
- `publisher_id`
- `instrument_id`
- `action`
- `side`
- `price`
- `size`
- `channel_id`
- `order_id`
- `flags`
- `ts_in_delta`
- `sequence`
- `symbol`

## Notes

- The repository contains multiple approaches because the project was used to compare parsing and reconstruction strategies.
- The threaded design is included as an experiment and benchmark comparator, not as a universal replacement for the sequential book update logic.
- The included validator is useful when you want to confirm that a faster implementation still reproduces the same output as the reference file.
