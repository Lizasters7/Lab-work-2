# Lab Work 2: Performance of `std::any_of`

This project measures how fast `std::any_of` works in different modes.
`std::any_of` checks whether at least one element of an array matches a condition.

The program compares three ways to run it:

1. The standard library `std::any_of` without an execution policy.
2. The standard library `std::any_of` with execution policies: `seq`, `par`, `unseq`, `par_unseq`.
3. My own parallel algorithm `myAnyOf`, which splits the work between K threads.

All experiments are run in two Release builds: with maximum optimization (`/O2`) and without optimization (`/Od`).

## Technologies

- **C++20**
- **MSVC** compiler (Visual Studio Build Tools)
- **CMake** to build the project
- **`<execution>`** for execution policies
- **`<thread>`** (`std::thread`) for my own parallel algorithm
- **`<random>`** (`std::mt19937`, `std::uniform_int_distribution`) to generate test data
- **`<chrono>`** (`std::chrono::steady_clock`) to measure time
- **`<filesystem>`** and **`<fstream>`** to save and load data files

## Method

### 1. Test data

The program generates 4 arrays of random integers with sizes 1 000, 100 000, 1 000 000 and 10 000 000.
All numbers are between 0 and 999. The random generator uses a fixed seed, so the data is always the same.

The arrays are saved to text files in the `data` folder once.
All experiments then load the same files, so every mode is tested on the same data.

### 2. Condition (predicate)

The condition is `x > 999`. It is never true, because all numbers are 999 or smaller.
This is the worst case for `any_of`: it has to check every element of the array.
Because of this, the measured time does not depend on where a matching element happens to be.

### 3. Time measurement

Each task runs 7 times. The program sorts the times and takes the median (the middle value).
The median is used because one slow run (for example, when the system is busy) does not change it much.

### 4. My own parallel algorithm

`myAnyOf` works in four steps:

1. Split the array into K parts of almost equal size.
2. Start one `std::thread` for each part. Each thread checks its part with the normal `std::any_of`.
3. Wait for all threads with `join()`.
4. Combine the results of all parts with a sequential library algorithm.

Before the measurements, the program checks that `myAnyOf` gives the same answer as `std::any_of` for different data, conditions and values of K.

The program tests K from 1 to 2 × (number of processor threads), and also 4×, 8×, 16× and 32× the number of processor threads.
For each array size it prints a table of times and the best K, and compares the best K with the number of processor threads.

### 5. Optimization levels

The same code is built twice:

- `/O2` — maximum optimization
- `/Od` — no optimization

The results of both builds are saved to `results/output_O2.txt` and `results/output_Od.txt`.

## Project structure

| File | What it does |
| --- | --- |
| `main.cpp` | Starts the program and runs all experiments |
| `sequenceGenerator.h`, `sequenceGenerator.cpp` | Generates random arrays |
| `sequenceStore.h`, `sequenceStore.cpp` | Saves arrays to files and loads them back |
| `timer.h`, `timer.cpp` | Measures the median time of a task |
| `myAnyOf.h` | My own parallel `any_of` with K threads |
| `experiments.h`, `experiments.cpp` | Runs the experiments and prints the tables |
| `CMakeLists.txt` | Build settings |
| `results/` | Program output used for the report |

## Data file format

```
first line: the number of elements N (N >= 0)
then: N integers, separated by spaces or line breaks
```

Example:

```
5
12
7
999
0
431
```

## How to build and run

Run these commands from the project folder (where `CMakeLists.txt` is).

Build with maximum optimization:

```
cmake -S . -B build-O2
cmake --build build-O2 --config Release
```

Build without optimization:

```
cmake -S . -B build-Od -DCMAKE_CXX_FLAGS_RELEASE="/Od /DNDEBUG"
cmake --build build-Od --config Release
```

Run and save the output:

```
.\build-O2\Release\Lab-work-2.exe > results\output_O2.txt
.\build-Od\Release\Lab-work-2.exe > results\output_Od.txt
```

On the first run, the program creates the `data` folder with the test files.

## Main results

- On small arrays (1 000 – 100 000 elements), extra threads make the work slower, because starting a thread takes more time than checking the numbers.
- On large arrays (1 – 10 million elements), the best K is close to the number of processor threads (12 on the test machine).
- When there are many more threads than the processor can run at once, the time grows linearly with K (about 0.035 ms per thread).
- `par` and `par_unseq` are faster only on large arrays. `seq` and `unseq` work like the call without a policy.
- `/O2` makes the sequential `any_of` 6–8 times faster than `/Od`.
