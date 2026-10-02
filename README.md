# Lab 05: CPU Scheduling Simulator

## Files

This submission contains six files: `README.md`, `proj2.py`, `Makefile`, `input.1`, `input.2`, and `input.3`. This README also serves as the lab report.

## Compile and run

Python 3.8 or newer is required. The program uses only the standard library, so no compilation or package installation is needed. Open a terminal in this folder:

```text
python proj2.py input.1 FCFS
python proj2.py input.1 SJF
python proj2.py input.1 RR 5
```

Replace `python` with `python3` if that is its name on your system. The general command is:

```text
python proj2.py input_file [FCFS|RR|SJF] [time_quantum]
```

Choose exactly one algorithm. The quantum is required only for RR and must be a positive integer. Invalid arguments or input produce an error and exit status 1.

With GNU Make installed, use:

```text
make
make fcfs
make sjf
make rr QUANTUM=5
make run INPUT=input.3 ALGORITHM=RR QUANTUM=2
make test
make clean
```

`make` checks Python syntax by generating bytecode. `make test` runs all nine combinations below and stops if a command fails; compare its averages with the expected-results table. `make clean` removes generated Python caches. Override the interpreter with `PYTHON=python3`, for example `make test PYTHON=python3`. Direct Python commands work without Make.

## Input format

The first line is a positive process count. Exactly that many rows follow, each containing integer `pid arrival_time burst_time` values. PIDs must be unique, arrivals nonnegative, and bursts positive. All times are in milliseconds. Blank lines are ignored. For example:

```text
4
0 0 12
1 2 4
2 3 1
3 4 2
```

## 3 Test Input 

Input 1 
```text
4
0 0 12
1 2 4
2 3 1
3 4 2
```

Input 2 
```text
3
10 2 3
11 2 1
12 8 2
```

Input 3 
```text
3
20 0 4
21 2 2
22 2 1
```
## Result screenshot

![Case 1 FCFS output](screenshots/case1-fcfs.png)


![Case 1 RR output](screenshots/case1-rr.png)



![Case 1 SJF output](screenshots/case1-sjf.png)