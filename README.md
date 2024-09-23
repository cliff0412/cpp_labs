## deps
```
sudo apt-get install libpapi-dev
```

## run
```
cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j4
./bench_lists
```