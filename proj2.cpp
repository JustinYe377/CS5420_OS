#include <algorithm>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using Time = long long;

struct Task {
    int pid;
    Time arrival;
    Time burst;
    Time remaining;
    Time start = -1;
    Time end = -1;
};

struct Segment {
    int pid;
    Time start;
    Time end;
};

std::vector<Task> readTasks(const std::string& path) {
    std::ifstream input(path);
    if (!input) throw std::runtime_error("cannot open input file");

    int count;
    if (!(input >> count) || count <= 0) {
        throw std::runtime_error("first value must be a positive process count");
    }

    std::vector<Task> tasks;
    std::unordered_set<int> pids;
    for (int i = 0; i < count; ++i) {
        int pid;
        Time arrival, burst;
        if (!(input >> pid >> arrival >> burst)) {
            throw std::runtime_error("process count must match the number of task rows");
        }
        if (!pids.insert(pid).second || arrival < 0 || burst <= 0) {
            throw std::runtime_error("PIDs must be unique, arrivals nonnegative, and bursts positive");
        }
        tasks.push_back({pid, arrival, burst, burst});
    }

    std::string extra;
    if (input >> extra) {
        throw std::runtime_error("process count must match the number of task rows");
    }
    return tasks;
}

std::vector<Segment> simulate(std::vector<Task>& tasks, const std::string& policy, Time quantum) {
    std::vector<std::size_t> arrivals(tasks.size());
    std::iota(arrivals.begin(), arrivals.end(), 0);
    std::stable_sort(arrivals.begin(), arrivals.end(), [&](std::size_t a, std::size_t b) {
        return tasks[a].arrival < tasks[b].arrival;
    });

    std::deque<std::size_t> ready;
    std::vector<Segment> segments;
    Time time = 0, used = 0, segmentStart = 0;
    std::size_t nextArrival = 0, completed = 0;
    const std::size_t none = tasks.size();
    std::size_t running = none;

    while (completed < tasks.size()) {
        while (nextArrival < tasks.size() && tasks[arrivals[nextArrival]].arrival == time) {
            std::size_t index = arrivals[nextArrival++];
            ready.push_back(index);
            std::cout << "Time " << time << ": PID " << tasks[index].pid << " arrived\n";
        }

        if (running != none && policy == "RR" && used == quantum) {
            segments.push_back({tasks[running].pid, segmentStart, time});
            std::cout << "Time " << time << ": PID " << tasks[running].pid << " quantum expired\n";
            ready.push_back(running);
            running = none;
        }

        if (running == none && !ready.empty()) {
            if (policy == "SJF") {
                auto chosen = std::min_element(ready.begin(), ready.end(), [&](std::size_t a, std::size_t b) {
                    if (tasks[a].burst != tasks[b].burst) return tasks[a].burst < tasks[b].burst;
                    if (tasks[a].arrival != tasks[b].arrival) return tasks[a].arrival < tasks[b].arrival;
                    return a < b;
                });
                running = *chosen;
                ready.erase(chosen);
            } else {
                running = ready.front();
                ready.pop_front();
            }
            if (tasks[running].start == -1) tasks[running].start = time;
            segmentStart = time;
            used = 0;
            std::cout << "Time " << time << ": PID " << tasks[running].pid << " selected\n";
        }

        if (running == none) {
            std::cout << '[' << time << ", " << time + 1 << "): idle\n";
        } else {
            std::cout << '[' << time << ", " << time + 1 << "): PID " << tasks[running].pid << " running\n";
            --tasks[running].remaining;
            ++used;
        }

        ++time;
        if (running != none && tasks[running].remaining == 0) {
            tasks[running].end = time;
            segments.push_back({tasks[running].pid, segmentStart, time});
            std::cout << "Time " << time << ": PID " << tasks[running].pid << " completed\n";
            ++completed;
            running = none;
        }
    }
    return segments;
}

void printStatistics(const std::vector<Task>& tasks, const std::vector<Segment>& segments) {
    std::cout << "\nExecution intervals\n"
              << "PID  Start  End  Running\n";
    for (const auto& segment : segments) {
        std::cout << std::setw(3) << segment.pid << std::setw(7) << segment.start
                  << std::setw(5) << segment.end << std::setw(9) << segment.end - segment.start << '\n';
    }

    std::vector<std::size_t> order(tasks.size());
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
        return tasks[a].end < tasks[b].end;
    });

    std::cout << "\nPID  Arrival  Start  End  Running  Waiting\n";
    Time totalWaiting = 0;
    for (std::size_t index : order) {
        const Task& task = tasks[index];
        Time waiting = task.end - task.arrival - task.burst;
        totalWaiting += waiting;
        std::cout << std::setw(3) << task.pid << std::setw(9) << task.arrival
                  << std::setw(7) << task.start << std::setw(5) << task.end
                  << std::setw(9) << task.burst << std::setw(9) << waiting << '\n';
    }
    double average = static_cast<double>(totalWaiting) / tasks.size();
    std::cout << "Average Waiting Time: " << std::setprecision(10) << average << " ms\n";
}

int main(int argc, char* argv[]) {
    const std::string usage = "Usage: ./proj2 input_file [FCFS|RR|SJF] [time_quantum]";
    try {
        if (argc < 3) throw std::runtime_error(usage);
        std::string policy = argv[2];
        if (policy != "FCFS" && policy != "RR" && policy != "SJF") {
            throw std::runtime_error(usage);
        }
        if (argc != (policy == "RR" ? 4 : 3)) throw std::runtime_error(usage);

        Time quantum = 0;
        if (policy == "RR") {
            std::size_t used = 0;
            quantum = std::stoll(argv[3], &used);
            if (used != std::string(argv[3]).size() || quantum <= 0) {
                throw std::runtime_error("time quantum must be a positive integer");
            }
        }

        auto tasks = readTasks(argv[1]);
        std::cout << "Policy: " << policy;
        if (policy == "RR") std::cout << " (quantum = " << quantum << " ms)";
        std::cout << '\n';
        auto segments = simulate(tasks, policy, quantum);
        printStatistics(tasks, segments);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
