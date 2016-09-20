### Compile and run a batch session on TACC
### Firstly, Please modify your parameters in tacc_batch.sh
./build_run_on_tacc.sh

### Compile and run on csgrads1.utdallas.edu
./build_run_on_csgrads1.sh

### Compile and Run by hand
### parameter of lock_compare: 'thread_num' 'loop_num' 'test_times'
### 'thread_num': Indicate the max threads will be tested. The program will test from 1 ~ 'thread_num' threads.
### 'loop_num': loop number of each thread
### 'test_times': repeating times of each test
cd src && make && ./lock_compare 16 1000000 16
