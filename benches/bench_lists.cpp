#include <benchmark/benchmark.h>
#include <forward_list>
#include <random>

std::random_device rd;  // Seed generator
std::mt19937 gen(rd()); // Mersenne Twister engine
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
        Order order{random_price,random_fee};
        orders[i] = order;
    }

    for (auto _ : state)
    {
        double sum = 0;
        for (int i = 0; i < orders.size(); ++i) {
            Order order = orders[i];
            sum += (order.price)*(order.fee);
        }
    }
}

static void BM_forwardlist_iter_sum(benchmark::State &state)
{

    std::forward_list<Order> orders;
    for (int i = 0; i < state.range(0); ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price,random_fee};
        orders.push_front(order);
    }
    for (auto _ : state)
    {
        double sum = 0;
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            sum += (it->price)*(it->fee);
        }
    }
}

BENCHMARK(BM_vector_iter_sum)->Arg(1000);
BENCHMARK(BM_forwardlist_iter_sum)->Arg(1000);

BENCHMARK_MAIN();