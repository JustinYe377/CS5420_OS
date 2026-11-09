#!/usr/bin/env python3
"""Lab 05: simulate FCFS, nonpreemptive SJF, and Round Robin in milliseconds."""

import sys
from collections import deque
from dataclasses import dataclass


@dataclass
class Task:
    pid: int
    arrival: int
    burst: int
    remaining: int
    start: int = -1
    end: int = -1


# Read the process count and validate each task before simulation.
def read_tasks(path):
    with open(path, encoding="utf-8") as source:
        lines = [line.split() for line in source if line.strip()]
    if not lines or len(lines[0]) != 1:
        raise ValueError("first line must contain the process count")
    count = int(lines[0][0])
    if count <= 0 or len(lines) != count + 1:
        raise ValueError("positive process count must match the number of task rows")
    tasks, seen = [], set()
    for row in lines[1:]:
        if len(row) != 3:
            raise ValueError("each task row must contain pid arrival_time burst_time")
        pid, arrival, burst = map(int, row)
        if pid in seen or arrival < 0 or burst <= 0:
            raise ValueError("PIDs must be unique, arrivals nonnegative, and bursts positive")
        seen.add(pid)
        tasks.append(Task(pid, arrival, burst, burst))
    return tasks


# Advance one millisecond at a time, admitting arrivals before RR requeueing.
def simulate(tasks, policy, quantum=None):
    arrivals = sorted(range(len(tasks)), key=lambda i: (tasks[i].arrival, i))
    ready = deque()
    time = next_arrival = completed = used = 0
    running = None
    segments = []
    segment_start = 0
    while completed < len(tasks):
        while next_arrival < len(tasks) and tasks[arrivals[next_arrival]].arrival == time:
            index = arrivals[next_arrival]
            ready.append(index)
            print(f"Time {time}: PID {tasks[index].pid} arrived")
            next_arrival += 1
        if running is not None and policy == "RR" and used == quantum:
            segments.append((tasks[running].pid, segment_start, time))
            print(f"Time {time}: PID {tasks[running].pid} quantum expired")
            ready.append(running)
            running = None
        if running is None and ready:
            if policy == "SJF":
                running = min(ready, key=lambda i: (tasks[i].burst, tasks[i].arrival, i))
                ready.remove(running)
            else:
                running = ready.popleft()
            task = tasks[running]
            if task.start == -1:
                task.start = time
            segment_start, used = time, 0
            print(f"Time {time}: PID {task.pid} selected")
        if running is None:
            print(f"[{time}, {time + 1}): idle")
        else:
            task = tasks[running]
            print(f"[{time}, {time + 1}): PID {task.pid} running")
            task.remaining -= 1
            used += 1
        time += 1
        if running is not None and tasks[running].remaining == 0:
            task.end = time
            segments.append((task.pid, segment_start, time))
            print(f"Time {time}: PID {task.pid} completed")
            completed += 1
            running = None
    return segments


# Print dispatch intervals and waiting times derived from completion times.
def print_statistics(tasks, segments):
    print("\nExecution intervals")
    print("PID  Start  End  Running")
    for pid, start, end in segments:
        print(f"{pid:3} {start:6} {end:4} {end - start:8}")
    print("\nPID  Arrival  Start  End  Running  Waiting")
    for task in sorted(tasks, key=lambda task: task.end):
        waiting = task.end - task.arrival - task.burst
        print(f"{task.pid:3} {task.arrival:8} {task.start:6} {task.end:4} {task.burst:8} {waiting:8}")
    average = sum(task.end - task.arrival - task.burst for task in tasks) / len(tasks)
    print(f"Average Waiting Time: {average:g} ms")


# Validate command-line arguments and run the requested scheduler.
def main(argv):
    usage = "Usage: python proj2.py input_file [FCFS|RR|SJF] [time_quantum]"
    try:
        if len(argv) < 3 or argv[2] not in ("FCFS", "RR", "SJF"):
            raise ValueError(usage)
        policy = argv[2]
        if len(argv) != (4 if policy == "RR" else 3):
            raise ValueError(usage)
        quantum = int(argv[3]) if policy == "RR" else None
        if quantum is not None and quantum <= 0:
            raise ValueError("time quantum must be a positive integer")
        tasks = read_tasks(argv[1])
        print(f"Policy: {policy}" + (f" (quantum = {quantum} ms)" if quantum else ""))
        print_statistics(tasks, simulate(tasks, policy, quantum))
        return 0
    except (OSError, ValueError) as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
