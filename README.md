# Parallel Client-Server Merge Sort with Thread Pool

## 1. Project Overview

This project implements a distributed client-server system for parallel execution of the Merge Sort algorithm. The system is designed to demonstrate principles of multithreading, task scheduling, and performance scaling using a custom Thread Pool.

The architecture follows a **Client–Server communication model over TCP sockets**, where the server performs computationally intensive sorting operations and returns results to the client.

Additionally, the system includes a **benchmarking mode** for evaluating performance scalability across different thread configurations.

---

## 2. System Architecture

### 2.1 Client

The client is responsible for:

- Accepting user input (array size, thread count, and array values)
- Sending requests to the server via TCP socket
- Receiving and displaying sorted results

Communication format: MERGE_SORT Optional benchmark mode: 1

---

### 2.2 Server

The server is responsible for:

- Accepting multiple client connections
- Dispatching each client request to a Thread Pool
- Parsing input commands
- Executing Merge Sort in parallel
- Returning sorted results

Each client request is handled independently in a worker thread.

---

### 2.3 Thread Pool

A custom Thread Pool is used to manage concurrency.

Key properties:

- Fixed number of worker threads
- Task queue with mutex synchronization
- Condition variable for thread signaling
- Futures for task synchronization

This avoids thread creation overhead and allows efficient reuse of threads.

---

## 3. Parallel Algorithm Design

### 3.1 Parallel Merge Sort Strategy

The algorithm uses a **chunk-based parallelization approach**:

1. The array is divided into equal chunks
2. Each chunk is sorted independently using `std::sort`
3. Sorted chunks are merged iteratively

This design avoids recursive thread explosion and ensures controlled parallelism.

---

### 3.2 Why Not Fully Recursive Parallelism?

A recursive parallel merge sort was initially implemented but caused:

- Thread starvation
- Deadlocks due to blocking futures inside worker threads
- Excessive task overhead

Therefore, a **bounded parallel model** was adopted.

---

## 4. Thread Pool Design

The Thread Pool follows a producer-consumer model:

- Workers wait on a condition variable
- Tasks are pushed into a shared queue
- Workers execute tasks asynchronously
- Destructor joins all threads safely

### Key guarantee:

No dynamic thread creation occurs during execution.

---

## 5. Performance Evaluation

### 5.1 Benchmark Setup

The system supports automated benchmarking with:

- Fixed array size
- Random input generation
- Multiple thread configurations
- Multiple runs per configuration

---

### 5.2 Measured Metrics

- Execution time (ms)
- Average runtime over multiple runs
- Speedup ratio

---

### 5.3 Speedup Formula

Speedup is defined as: Where:

- T1 = execution time using 1 thread
- Tp = execution time using p threads

---

### 5.4 Amdahl’s Law

The theoretical speedup limit is given by: Sp = 1 / ((1 - α) + α / p)

Where:

- α is the parallel portion of the algorithm
- p is number of threads

This explains diminishing returns for higher thread counts.

---

## 6. Results (Example)

| Threads | Avg Time (ms) | Speedup |
|---------|---------------|---------|
| 1       | 28.4096 ms    | 1.0     |
| 2       | 15.5671 ms    | 1.82498 |
| 4       | 11.2412 ms    | 2.52727 |
| 8       | 13.1538       | 2.52727 |