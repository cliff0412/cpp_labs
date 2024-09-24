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
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_AVX=ON .. && make -j4
taskset -c 31 ./bench_lists
```

```
perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores ./lists
perf stat -e all_data_cache_accesses -e r430729 ls # r stands for raw, followed by a unique id to a PMC event
```