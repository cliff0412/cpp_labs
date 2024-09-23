#include <forward_list>
#include <random>
#include <iostream>

std::random_device rd;  // Seed generator
std::mt19937 gen(rd()); // Mersenne Twister engine
std::uniform_real_distribution<> dis(0.0, 1.0);

struct Order
{
    double price;
    double fee;
};

constexpr size_t f32s_in_cache_line_k = 64 / sizeof(float);
constexpr size_t f32s_in_cache_line_half_k = f32s_in_cache_line_k / 2;

int main()
{
    std::forward_list<Order> orders;
    for (int i = 0; i < 100; ++i)
    {
        // double random_price = dis(gen);
        // double random_fee = dis(gen);
        // Order order{random_price, random_fee};
        Order order{static_cast<double>(i), 2};
        orders.push_front(order);
    }

    double sum = 0;
    for (auto it = orders.begin(); it != orders.end(); ++it)
    {
        // std::cout << it->price << std::endl;
        sum += (it->price) * (it->fee);
    }
    // std::cout << sum << std::endl;

    std::cout << sizeof(double) << std::endl;

}
