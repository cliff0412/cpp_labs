#include <forward_list>
#include <random>
#include <iostream>
#include <immintrin.h>
#include <omp.h>
#include <chrono>
#include <thread>

std::random_device rd;  // Seed generator
std::mt19937 gen(rd()); // Mersenne Twister engine
std::uniform_real_distribution<> dis(0.0, 1.0);

struct Order
{
    double price;
    double fee;
};

double avx_multiply_sum(const double *a, const double *b, size_t size)
{
    // AVX register to store the sum
    __m256d sum = _mm256_setzero_pd();

    // Process four doubles at a time
    size_t i;
    for (i = 0; i + 3 < size; i += 4)
    {
        // Load four double values from each array
        __m256d vec_a = _mm256_loadu_pd(a + i); // Load unaligned data from a[i] to a[i+3]
        __m256d vec_b = _mm256_loadu_pd(b + i); // Load unaligned data from b[i] to b[i+3]

        // Multiply the two vectors
        __m256d vec_mul = _mm256_mul_pd(vec_a, vec_b);

        // Add the result to the sum
        sum = _mm256_add_pd(sum, vec_mul);
    }

    // Horizontal addition of the 256-bit register (four doubles) into a single double
    double result[4];
    _mm256_storeu_pd(result, sum);
    double final_sum = result[0] + result[1] + result[2] + result[3];

    // Handle the remaining elements (if the size is not a multiple of 4)
    for (; i < size; ++i)
    {
        final_sum += a[i] * b[i];
    }

    return final_sum;
}

void avx_add(double *a, double *b, double *result, size_t size)
{
    // Use OpenMP to parallelize the SIMD operations across multiple cores
    // #pragma omp parallel for
    for (size_t i = 0; i < size; i += 4)
    {
        // Load four double elements into AVX registers
        __m256d vec_a = _mm256_loadu_pd(&a[i]);
        __m256d vec_b = _mm256_loadu_pd(&b[i]);

        // Perform element-wise addition
        __m256d vec_res = _mm256_mul_pd(vec_a, vec_b);

        // Store the result back into the result array
        _mm256_storeu_pd(&result[i], vec_res);
    }
}

void iterate_vector(size_t size)
{

    std::vector<Order> orders(size);
    for (int i = 0; i < size; ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders[i] = order;
    }

    for (int i = 0; i < size; i++)
    {
        double sum = 0;
        for (int i = 0; i < orders.size(); ++i)
        {
            Order order = orders[i];
            sum += (order.price) * (order.fee);
        }
    }
}

void iterate_forward_list(size_t size)
{
    std::forward_list<Order> orders;
    for (int i = 0; i < size; ++i)
    {
        double random_price = dis(gen);
        double random_fee = dis(gen);
        Order order{random_price, random_fee};
        orders.push_front(order);
    }

    double sum = 0;
    for (auto it = orders.begin(); it != orders.end(); ++it)
    {
        sum += (it->price) * (it->fee);
    }
    std::cout << sum << std::endl;
}
int main()
{

    // iterate_forward_list(1000);
    // iterate_vector(1000);
    // std::cout << sizeof(double) << std::endl;

    // size_t size = 100;
    // double a[size];
    // double b[size];
    // for(int i =0; i < size; ++i) {
    //     a[i] = static_cast<double>(1);
    //     b[i] = static_cast<double>(1);
    // }
    // double sum = avx_multiply_sum(a,b,size);
    // std::cout << "sum: " << sum << std::endl;

    //     const size_t size = 1024; // Ensure this is a multiple of 4 for AVX
    //     double a[size], b[size], result[size];

    //     // Initialize the arrays with some values
    //     for (size_t i = 0; i < size; ++i) {
    //         a[i] = 1;
    //         b[i] = 1;
    //     }

    //      auto start = std::chrono::high_resolution_clock::now();
    //     avx_add(a, b, result, 100);
    //     auto end = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    //     std::cout << "Time elapsed: " << duration.count() << " ns" << std::endl;

    //     auto start2 = std::chrono::high_resolution_clock::now();
    //     avx_add(a, b, result, 1000);
    //     auto end2 = std::chrono::high_resolution_clock::now();
    //     auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2);

    //     std::cout << "Time elapsed: " << duration2.count() << " ns" << std::endl;

    // // Output the first few results to verify correctness
    // for (size_t i = 0; i < 1024; ++i) {
    //     std::cout << "result[" << i << "] = " << result[i] << std::endl;
    // }

    return 0;
}
