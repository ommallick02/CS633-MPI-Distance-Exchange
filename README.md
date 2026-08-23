# CS633-MPI-Distance-Exchange

A parallel computing project implementing distance-based data exchange using MPI (Message Passing Interface) point-to-point communication on HPC clusters. This assignment explores parallel communication patterns, performance scalability, and optimization techniques.

**Course:** CS633 (Parallel Computing)  
**Institution:** IIT Kanpur  
**Academic Year:** 2025-2026 (Semester 7)  
**Group:** 35

---

## Project Overview

This project implements a distributed-memory parallel algorithm where MPI processes exchange data across varying communication distances. Each process acts simultaneously as both a sender (communicating with right neighbors) and a receiver (processing data from left neighbors), executing different computations on each communication path.

**Key Objective:** Benchmark and analyze the performance characteristics of this communication pattern across different process counts (8, 16, 32) and data sizes (262k, 1M doubles).

---

## Algorithm Description

### Communication Pattern

Each MPI rank communicates with two distant neighbors:
- **D1 path:** Sends data to rank + D1, receives results back
- **D2 path:** Sends data to rank + D2, receives results back

### Computation

When a rank receives data from left neighbors (rank - D1 or rank - D2):

1. **D1 computation:** Element-wise squaring of input array
   ```
   output[i] = input[i] * input[i]
   ```

2. **D2 computation:** Element-wise logarithm followed by scaling
   ```
   output[i] = log(input[i])
   scaled_result = scaled * output[i]
   ```

### Execution Flow

```
Initialization → T Iterations → Reduction & Output
```

For each iteration:
1. Rank sends its data buffer to right neighbors (rank + D1, rank + D2)
2. Simultaneously, rank receives from left neighbors (rank - D1, rank - D2)
3. Received data is processed via the respective computation path
4. Results are returned to the sender
5. Timing information accumulated

---

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| **Language** | C with MPI |
| **Communication** | MPI point-to-point (MPI_Send/MPI_Recv) |
| **Data Type** | Double-precision floating-point (64-bit) |
| **Process Counts (P)** | 8, 16, 32 |
| **Data Sizes (M)** | 262,144 doubles (~2 MB), 1,048,576 doubles (~8 MB) |
| **Iterations (T)** | 10 |
| **Communication Distances** | D1 = 2, D2 = 4 |
| **Repetitions per Config** | 5 runs for statistical analysis |

---

## Compilation

### Using Makefile (Recommended)

```bash
make
```

This compiles `src/src.c` into an executable named `execute` using the MPI C compiler.

### Manual Compilation

```bash
mpicc -O2 -Wall -lm -o execute src/src.c
```

**Flags explained:**
- `-O2` — Level 2 compiler optimization
- `-Wall` — Enable all common compiler warnings
- `-lm` — Link the math library (for log function)
- `mpicc` — MPI-aware C compiler wrapper

---

## Execution

### Local Execution (Non-MPI clusters)

```bash
mpirun -np <P> ./execute <M> <D1> <D2> <T> <SEED>
```

**Parameters:**
- `<P>` — Number of MPI processes
- `<M>` — Number of doubles per array
- `<D1>`, `<D2>` — Communication distances
- `<T>` — Number of iterations
- `<SEED>` — Random seed for reproducibility

**Example:**
```bash
mpirun -np 8 ./execute 262144 2 4 10 1000
```

### SLURM Job Submission (HPC clusters)

Submit SLURM scripts from `job_scripts/`:

```bash
sbatch job_scripts/p8.sh    # Run with 8 processes
sbatch job_scripts/p16.sh   # Run with 16 processes
sbatch job_scripts/p32.sh   # Run with 32 processes
```

Scripts automatically handle:
- Job allocation and naming
- Output/error redirection
- Multiple data sizes and iterations
- SLURM environment configuration

---

## Output Format

Each program execution outputs three space-separated values:

```
<max_D1_result> <max_D2_result> <execution_time_seconds>
```

**Example output:**
```
99936.000000 1416360.081587 0.440566
```

---

## Performance Analysis

### Collected Metrics

- **Execution time** for various (P, M) configurations
- **Speedup** — Serial vs. parallel execution time ratio
- **Efficiency** — Speedup / number of processes
- **Communication overhead** — Latency and bandwidth analysis

### Results Summary

Performance data from `analysis/timing_data.txt`:

| Process Count | Data Size (M) | Avg Time (sec) | Speedup | Efficiency |
|---------------|---------------|----------------|---------|------------|
| 8 | 262,144 | ~0.083 | ~1.0 | 12.5% |
| 8 | 1,048,576 | ~0.331 | ~1.0 | 12.5% |
| 16 | 262,144 | ~0.177 | ~0.47 | 2.9% |
| 16 | 1,048,576 | ~0.708 | ~0.47 | 2.9% |
| 32 | 262,144 | ~0.410 | ~0.20 | 0.6% |
| 32 | 1,048,576 | ~1.139 | ~0.29 | 0.9% |

**Key Observations:**
- Execution time scales linearly with data size across all process counts
- Speedup decreases as process count increases (communication overhead dominates)
- Communication latency becomes the bottleneck for larger P
- D1 and D2 paths show consistent maximum values (independent of P)

---

## Visualization

### Generate Performance Plots

```bash
cd analysis
python plot.py
```

This generates `../results/plot.jpeg` showing:
- Box plots of execution times
- Comparison across process counts and data sizes
- Statistical distribution of timing measurements
- Identification of outliers and variance

---

## Project Structure

```
CS633-mpi-distance-exchange/
├── README.md                    # This file
├── .gitignore                   # Git ignore patterns
├── Makefile                     # Build automation
├── RESULTS.md                   # Performance analysis & findings
│
├── src/
│   └── src.c                    # Main MPI program source code
│
├── job_scripts/
│   ├── p8.sh                    # SLURM script for 8 processes
│   ├── p16.sh                   # SLURM script for 16 processes
│   └── p32.sh                   # SLURM script for 32 processes
│
├── analysis/
│   ├── plot.py                  # Python visualization script
│   └── timing_data.txt          # Raw performance measurements
│
├── results/
│   ├── plot.jpeg                # Generated performance visualization
│   └── output.txt               # Sample program output
│
└── docs/
    └── report.pdf               # Complete course project report
```

---

## Requirements

### System Requirements
- MPI library (OpenMPI or MPICH) — must be installed and accessible
- GCC or compatible C compiler
- SLURM workload manager (for cluster execution, optional for local runs)

### Software
- **C compiler:** GCC (gcc/mpicc)
- **Python:** 3.6+ (for visualization: matplotlib, numpy)
- **Make:** GNU Make (optional, for Makefile)

### Installation (Ubuntu/Debian)

```bash
# Install MPI
sudo apt-get install libopenmpi-dev openmpi-bin

# Install build tools
sudo apt-get install build-essential

# Install Python dependencies
pip install matplotlib numpy
```

### Installation (HPC Cluster)

```bash
# Load modules (cluster-specific)
module load mpi
module load gcc
module load python
```

---

## Key Learning Outcomes

Through this project, students gain understanding of:

1. **MPI Communication Basics**
   - Point-to-point communication primitives (Send, Recv)
   - Blocking vs. non-blocking operations
   - Message deadlock prevention and synchronization

2. **Parallel Performance Analysis**
   - Speedup and efficiency calculations
   - Communication bottleneck identification
   - Scalability limitations and Amdahl's Law implications

3. **HPC Programming Practices**
   - SLURM job submission and batch processing
   - Performance measurement and profiling
   - Reproducibility through seed management

4. **Code Optimization**
   - Compiler optimization flags (-O2)
   - Memory layout and cache locality
   - Communication pattern analysis

---

## Performance Insights

### Communication Overhead

The distance-based communication pattern exhibits:
- **Latency-bound regime** for small message sizes
- **Bandwidth-limited regime** for large message sizes
- **Serialization bottleneck** as P increases (circular topology constraints)

### Scalability Bottleneck

Performance does not scale linearly due to:
1. **Fixed communication distance** (D1=2, D2=4) creates serialization
2. **Increasing network contention** with more processes
3. **Limited message overlap** capability in point-to-point communication

### Optimization Strategies Attempted

- Compiler optimizations (-O2)
- Data structure alignment for cache efficiency
- SLURM resource allocation tuning
- Random seed management for reproducible runs

---

## How to Reproduce Results

### Quick Start

```bash
# Clone the repository
git clone https://github.com/ommallick02/CS633-mpi-distance-exchange.git
cd CS633-mpi-distance-exchange

# Compile
make

# Run single experiment
mpirun -np 8 ./execute 262144 2 4 10 1000

# Submit SLURM jobs (if on HPC cluster)
sbatch job_scripts/p8.sh
sbatch job_scripts/p16.sh
sbatch job_scripts/p32.sh

# Generate visualization
cd analysis
python plot.py
```

### Detailed Analysis

See `RESULTS.md` for:
- Raw timing data and statistical summaries
- Speedup and efficiency calculations
- Detailed performance analysis
- Interpretation of results

---

## Citation

If using this project for reference or comparison, please cite:

```
CS633 MPI Distance Exchange
Group 35, Parallel Computing (CS633)
IIT Kanpur, 2025-2026
GitHub: https://github.com/ommallick02/CS633-mpi-distance-exchange
```

---

## Troubleshooting

### Issue: `mpicc not found`

**Solution:** Install MPI development libraries:
```bash
sudo apt-get install libopenmpi-dev openmpi-bin
```

### Issue: Compilation error with `-lm` flag

**Solution:** Ensure the math library link appears after the source file:
```bash
mpicc -O2 -o execute src/src.c -lm  # Correct order
```

### Issue: SLURM job hangs or times out

**Solution:** Check SLURM partition availability and adjust `--time`:
```bash
squeue  # View current jobs
sinfo   # Check partition status
```

### Issue: Different output on multiple runs

**Solution:** Ensure same random seed is used. Seeds are set in SLURM scripts as `SEED=1000`.

---

## References

- **MPI Documentation:** https://www.open-mpi.org/doc/
- **SLURM Documentation:** https://slurm.schedmd.com/
- **Parallel Computing Concepts:** *Introduction to Parallel Computing* by Blaise Barney (LLNL)
- **Performance Analysis:** *High Performance Computing* by Charles Severance