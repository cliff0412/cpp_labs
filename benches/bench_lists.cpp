#include <benchmark/benchmark.h>
#include <forward_list>
#include <papi.h>
#include <iostream>
#include <plf_list.h>
#include <immintrin.h>
#include <chrono>
#include <thread>
#include "lists.h"
#include "util.h"

#define PAPI_START()                                                   \
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)       \
    {                                                                  \
        std::cerr << "PAPI_library_init failed!" << std::endl;         \
        return;                                                        \
    }                                                                  \
    int event_set = PAPI_NULL;                                         \
    long long values[2];                                               \
    int retval = 0;                                                    \
    if ((retval = PAPI_create_eventset(&event_set)) != PAPI_OK)        \
    {                                                                  \
        std::cerr << "PAPI_create_eventset failed!" << std::endl;      \
        return;                                                        \
    }                                                                  \
    if ((retval = PAPI_add_event(event_set, PAPI_L1_DCM)) != PAPI_OK)  \
    {                                                                  \
        std::cerr << "PAPI_add_event failed! " << retval << std::endl; \
        return;                                                        \
    }                                                                  \
    if ((retval = PAPI_add_event(event_set, PAPI_L2_DCM)) != PAPI_OK)  \
    {                                                                  \
        std::cerr << "PAPI_add_event failed! " << retval << std::endl; \
        return;                                                        \
    }                                                                  \
    if ((retval = PAPI_start(event_set)) != PAPI_OK)                   \
    {                                                                  \
        std::cerr << "PAPI_start failed!" << std::endl;                \
        return;                                                        \
    }

#define PAPI_END()                                                   \
    if ((retval = PAPI_stop(event_set, values)) != PAPI_OK)          \
    {                                                                \
        std::cerr << "PAPI_stop failed!" << std::endl;               \
        return;                                                      \
    }                                                                \
    std::cout << "L1 Data Cache Misses: " << values[0] << std::endl; \
    std::cout << "L2 Data Cache Misses: " << values[1] << std::endl; \
    PAPI_cleanup_eventset(event_set);                                \
    PAPI_destroy_eventset(&event_set);

#ifdef __USE_AVX__

double avx_multiply_sum(const double *a, const double *b, size_t size)
{
    __m256d sum = _mm256_setzero_pd();
    size_t i;
    for (i = 0; i + 3 < size; i += 4)
    {
        __m256d vec_a = _mm256_loadu_pd(a + i);
        __m256d vec_b = _mm256_loadu_pd(b + i);
        __m256d vec_mul = _mm256_mul_pd(vec_a, vec_b);
        sum = _mm256_add_pd(sum, vec_mul);
    }

    double result[4];
    _mm256_storeu_pd(result, sum);
    double final_sum = result[0] + result[1] + result[2] + result[3];

    for (; i < size; ++i)
    {
        final_sum += a[i] * b[i];
    }

    return final_sum;
}
static void BM_vector_avx_iter_sum(benchmark::State &state)
{
    // std::vector<Order> orders(state.range(0));
    double prices[state.range(0)];
    double fees[state.range(0)];
    for (int i = 0; i < state.range(0); ++i)
    {

        prices[i] = gen_random_double();
        fees[i] = gen_random_double();
        // Order order{random_price, random_fee};
        // orders[i] = order;
    }
    PAPI_START();
    for (auto _ : state)
    {
        double sum = avx_multiply_sum(prices, fees, state.range(0));
        benchmark::DoNotOptimize(sum);
    }
    PAPI_END();
}
#endif

static void BM_vector_iter_sum(benchmark::State &state)
{
    std::vector<Order> orders(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
    {

        Order order{gen_random_double(), gen_random_double()};
        orders[i] = order;
    }
    PAPI_START();
    for (auto _ : state)
    {
        double sum = 0;
        for (int i = 0; i < orders.size(); ++i)
        {
            Order order = orders[i];
            sum += (order.price) * (order.fee);
        }
        benchmark::DoNotOptimize(sum);
    }
    PAPI_END();
}

static void BM_forwardlist_iter_sum(benchmark::State &state)
{

    std::forward_list<Order> orders;
    for (int i = 0; i < state.range(0); ++i)
    {
        Order order{gen_random_double(), gen_random_double()};
        orders.push_front(order);
    }

    PAPI_START();
    for (auto _ : state)
    {
        double sum = 0;
        for (auto it = orders.begin(); it != orders.end(); ++it)
        {
            sum += (it->price) * (it->fee);
        }
        benchmark::DoNotOptimize(sum); // if not set, the compiler will think sum is not used; hence optimized away
    }
    PAPI_END();
}

static float rm_factor = 0.1;
static float insert_factor = 0.1;

#define UNROLL_LIST_ORDER_SETUP(size)                                                          \
    plf::list<Order> orders;                                                                   \
    for (int i = 0; i < size; ++i)                                                             \
    {                                                                                          \
        orders.push_back(Order{static_cast<double>(i), static_cast<double>(i)});               \
    }                                                                                          \
    size_t rm_size = size * rm_factor;                                                         \
    std::vector<int> idx_rm = gen_random_int(0, size, rm_size);                                \
    for (auto it = idx_rm.begin(); it != idx_rm.end(); it++)                                   \
    {                                                                                          \
        orders.remove(Order{static_cast<double>(*it), static_cast<double>(*it)});              \
    }                                                                                          \
    size_t insert_size = (size - rm_size);                                                     \
    std::vector<int> idx_insert = gen_random_int(0, insert_size, insert_size * insert_factor); \
    std::sort(idx_insert.begin(), idx_insert.end());                                           \
    auto it = orders.begin();                                                                  \
    it++;                                                                                      \
    int i = 0;                                                                                 \
    for (auto it = orders.begin(); it != orders.end(); ++it)                                   \
    {                                                                                          \
        auto find_idx = std::find(idx_insert.begin(), idx_insert.end(), i);                    \
        if (find_idx != idx_insert.end())                                                      \
        {                                                                                      \
            orders.insert(it, Order{static_cast<double>(i), static_cast<double>(i)});          \
        }                                                                                      \
        i++;                                                                                   \
    }

static void BM_unroll_list_iter_sum(benchmark::State &state)
{
    UNROLL_LIST_ORDER_SETUP(state.range(0));
    PAPI_START();
    for (auto _ : state)
    {
        double sum = 0;
        for (auto it = orders.begin(); it != orders.end(); ++it)
        {
            sum += (it->price) * (it->fee);
        }
        benchmark::DoNotOptimize(sum);
    }
    PAPI_END();
}

static void BM_unroll_list_iter_sum_avx(benchmark::State &state)
{
#if defined(__x86_64__) || defined(_M_X64)
    UNROLL_LIST_ORDER_SETUP(state.range(0))
    PAPI_START();
    for (auto _ : state)
    {
        auto it_avx = orders.begin();
        double *start_ptr = &it_avx->price;
        double *last_ptr = start_ptr;

        while (it_avx != orders.end())
        {
            int continuous_num = 1;
            bool continuous = true;
            while (continuous && it_avx != orders.end())
            {
                it_avx++;
                double *curr_ptr = &it_avx->price;

                size_t offset = curr_ptr - last_ptr;

                if (offset != 4)
                {
                    // std::cout << "apply avx: " << continuous_num << std::endl;
                    // for (double *j = start_ptr; j <= last_ptr; j = j + 4)
                    // {
                    //     std::cout << *j << " " << *(j + 1) << std::endl;
                    // }
                    double sum = apply_order_avx_sum(start_ptr, continuous_num);
                    // std::cout << "sum: " << sum << std::endl;
                    continuous = false;
                    start_ptr = curr_ptr;
                }
                else
                {
                    continuous_num++;
                }
                last_ptr = curr_ptr;
            }
        }
    }
    PAPI_END();
#endif
}

static size_t SIZE = 10000;
BENCHMARK(BM_vector_iter_sum)->Arg(SIZE);
BENCHMARK(BM_forwardlist_iter_sum)->Arg(SIZE);
BENCHMARK(BM_unroll_list_iter_sum)->Arg(SIZE);
#ifdef __USE_AVX__
BENCHMARK(BM_vector_avx_iter_sum)->Arg(SIZE);
BENCHMARK(BM_unroll_list_iter_sum_avx)->Arg(SIZE);
#endif
BENCHMARK_MAIN();