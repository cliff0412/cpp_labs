#include <benchmark/benchmark.h>
#include <forward_list>
#include <random>
#include <papi.h>
#include <iostream>
#include <plf_list.h>
#include <immintrin.h>
#include <chrono>
#include <thread>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

struct alignas(16) Order
{
    double price;
    double fee;
};

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
        double random_price = dis(gen);
        double random_fee = dis(gen);
        prices[i] = random_price;
        fees[i] = random_fee;
        // Order order{random_price, random_fee};
        // orders[i] = order;
    }

    for (auto _ : state)
    {
        avx_multiply_sum(prices, fees, state.range(0));
    }
}
#endif

static void BM_vector_iter_sum(benchmark::State &state)
{

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        std::cerr << "PAPI_library_init failed!" << std::endl;
        return;
    }

    int event_set = PAPI_NULL;
    long long values[2];

    int retval = 0;
    if ((retval = PAPI_create_eventset(&event_set)) != PAPI_OK)
    {
        std::cerr << "PAPI_create_eventset failed!" << std::endl;
        return;
    }

    if ((retval = PAPI_add_event(event_set, PAPI_L1_DCM)) != PAPI_OK)
    {
        std::cerr << "PAPI_add_event failed! " << retval << std::endl;
        return;
    }
    if ((retval = PAPI_add_event(event_set, PAPI_L2_DCM)) != PAPI_OK)
    {
        std::cerr << "PAPI_add_event failed! " << retval << std::endl;
        return;
    }

    if ((retval = PAPI_start(event_set)) != PAPI_OK)
    {
        std::cerr << "PAPI_start failed!" << std::endl;
        return;
    }

    std::vector<Order> orders(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders[i] = order;
    }

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

    if ((retval = PAPI_stop(event_set, values)) != PAPI_OK)
    {
        std::cerr << "PAPI_stop failed!" << std::endl;
        return;
    }

    std::cout << "L1 Data Cache Misses: " << values[0] << std::endl;
    std::cout << "L2 Data Cache Misses: " << values[1] << std::endl;

    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
}

static void BM_forwardlist_iter_sum(benchmark::State &state)
{

    std::forward_list<Order> orders;
    for (int i = 0; i < state.range(0); ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders.push_front(order);
    }

    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        std::cerr << "PAPI library initialization failed!" << std::endl;
        return;
    }

    int event_set = PAPI_NULL;
    long long values[2]; // Array to hold the count values

    // Create an event set and add cache miss events
    PAPI_create_eventset(&event_set);
    PAPI_add_event(event_set, PAPI_L1_DCM); // Level 1 data cache misses
    // PAPI_add_event(event_set, PAPI_L2_DCM); // Level 2 data cache misses

    // Start counting events
    PAPI_start(event_set);
    for (auto _ : state)
    {
        double sum = 0;
        for (auto it = orders.begin(); it != orders.end(); ++it)
        {
            sum += (it->price) * (it->fee);
        }
    }

    // Stop counting events
    PAPI_stop(event_set, values);

    // Output the results
    std::cout << "L1 Data Cache Misses: " << values[0] << std::endl;
    std::cout << "L2 Data Cache Misses: " << values[1] << std::endl;

    // Clean up
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
}

static void BM_unroll_list_iter_sum(benchmark::State &state)
{

    plf::list<Order> orders;
    for (int i = 0; i < state.range(0); ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders.push_front(order);
    }

    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        std::cerr << "PAPI library initialization failed!" << std::endl;
        return;
    }

    int event_set = PAPI_NULL;
    long long values[2]; // Array to hold the count values

    // Create an event set and add cache miss events
    PAPI_create_eventset(&event_set);
    PAPI_add_event(event_set, PAPI_L1_DCM); // Level 1 data cache misses
    PAPI_add_event(event_set, PAPI_L2_DCM); // Level 2 data cache misses

    // Start counting events
    PAPI_start(event_set);
    for (auto _ : state)
    {
        double sum = 0;
        for (auto it = orders.begin(); it != orders.end(); ++it)
        {
            sum += (it->price) * (it->fee);
        }
    }

    // Stop counting events
    PAPI_stop(event_set, values);

    // Output the results
    std::cout << "L1 Data Cache Misses: " << values[0] << std::endl;
    std::cout << "L2 Data Cache Misses: " << values[1] << std::endl;

    // Clean up
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
}

BENCHMARK(BM_vector_iter_sum)->Arg(10000);
// BENCHMARK(BM_vector_iter_sum)->Arg(1000000);
// BENCHMARK(BM_forwardlist_iter_sum)->Arg(1000);
// BENCHMARK(BM_unroll_list_iter_sum)->Arg(1000);
#ifdef __USE_AVX__
// BENCHMARK(BM_vector_avx_iter_sum)->Arg(1000);
#endif
BENCHMARK_MAIN();