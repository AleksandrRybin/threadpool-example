# Threadpool example

C++11 simple threadpool example

## Build

### Dependicies

- CMake > 3.16
- Boost > 1.65
- C++11 compatible compiler

### How to configure and build

1. Clone repository

```git clone <url>```

2. Enter project directory

```cd threadpool-example```

3. Run CMake

```cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=RELEASE```

4. Build

```cmake --build build```

*build* folder now contains *threadpool_demo* executable - demo application for threadpool usage

## Run

### Demo application

Demo application computes matrix x vector multiplication
in parallel with help of threadpool.
Matrix and vector are generated randomly using uniform distribution.

Program options:

- **threads_num,t** - number of used threads
- **verbose,v** - print matrix, vector and result
- **matrix_rows** - number of rows in matrix
- **matrix_cols** - number of cols in matrix

## How to use library

1. Create threadpool with desired number of threads

```cpp
threadpool_example::ThreadPool thp(threadsNum);
```

2. Push task to threadpool and get future result

```cpp
auto future = thp.enqueue([](){
        std::cout << "I am a task";
        return 0;
    });
```

3. Optionaly wait for future result

```cpp
std::cout << "result is " << future.get();
```
