## deps
```
sudo apt-get install libpapi-dev
```

## deps
PAPI: >= 7.2 (https://github.com/icl-utk-edu/papi)

## run

options
-DUSE_AVX=ON
```
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j4
taskset -c 31 ./bench_lists
```

perf stat -e L1-dcache-load-misses,L1-dcache-store-misses ./your_program


/usr/bin/c++   -g -MD -MT CMakeFiles/lists.dir/lists/lists.cpp.o -MF CMakeFiles/lists.dir/lists/lists.cpp.o.d -mavx2 -o CMakeFiles/lists.dir/lists/lists.cpp.o -c /home/okxdex/data/zkdex-pap/workspace/cliff/cpp_labs/lists/lists.cpp

perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,L2-dcache-load-misses,L2-dcache-stores ./lists
perf stat -e cache-misses ./lists

perf stat -e l2_rqsts.all_demand_miss ./lists