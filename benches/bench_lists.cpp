#include <benchmark/benchmark.h>
#include <forward_list>
#include <random>
#include <papi.h>
#include <iostream>
#include <plf_list.h>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

struct alignas(16) Order
{
    double price;
    double fee;
};

static void BM_vector_iter_sum(benchmark::State &state)
{
    std::vector<Order> orders(state.range(0));
    for (int i = 0; i < state.range(0); ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders[i] = order;
    }

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        std::cerr << "PAPI library initialization failed!" << std::endl;
        return;
    }

    int event_set = PAPI_NULL;
    long long values[2];


    PAPI_create_eventset(&event_set);
    PAPI_add_event(event_set, PAPI_L1_DCM);
    PAPI_add_event(event_set, PAPI_L2_DCM);

    PAPI_start(event_set);

    for (auto _ : state)
    {
        double sum = 0;
        for (int i = 0; i < orders.size(); ++i)
        {
            Order order = orders[i];
            sum += (order.price) * (order.fee);
        }
    }

    PAPI_stop(event_set, values);

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


BENCHMARK(BM_vector_iter_sum)->Arg(1000);
// BENCHMARK(BM_forwardlist_iter_sum)->Arg(1000);
// BENCHMARK(BM_unroll_list_iter_sum)->Arg(1000);

BENCHMARK_MAIN();