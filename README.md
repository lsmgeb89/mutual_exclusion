# Mutual Exclusion

## Summary
  * Implemented `n`-thread mutual exclusion by using three different approaches: a tournament-based algorithm, [test-and-set][tas] instruction and [test-test-and-set][ttas] instruction
  * Implemented a simple model (`n` thread concurrently adding a shared counter) to test correctness and performance of three implementations with various test input parameters
  * Compared the averaged performance of the three implementations as a function of number of threads (varied from one to the number of logical cores in the machine)

## Project Information
  * Course: Introduction to Multicore Programming (CS 6301)
  * Professor: [Neeraj Mittal][mittal]
  * Semester: Fall 2016
  * Programming Language: C++
  * Build Tool: [CMake][cmake]

[tas]: https://en.wikipedia.org/wiki/Test-and-set
[ttas]: https://en.wikipedia.org/wiki/Test_and_test-and-set
[mittal]: http://cs.utdallas.edu/people/faculty/mittal-neeraj
[cmake]: https://cmake.org
