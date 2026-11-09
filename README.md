# Lab 05: CPU Scheduling Simulator

Implements FCFS, non-preemptive SJF, and Round Robin scheduling in C++17.

## Build and run

```text
make
./proj2 input.1 FCFS
./proj2 input.1 SJF
./proj2 input.1 RR 5
```

Usage: `./proj2 input_file [FCFS|RR|SJF] [time_quantum]`

The input starts with the process count, followed by `pid arrival_time burst_time` rows. The RR time quantum must be a positive integer.

## Test cases

Three test files are included: `input.1`, `input.2`, and `input.3`.

```text
make test
```
