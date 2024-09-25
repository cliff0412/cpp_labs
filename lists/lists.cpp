#ifndef _LISTS_CPP_
#define _LISTS_CPP_

#include <forward_list>
#include <random>
#include <iostream>
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif
// #include <omp.h>
#include <chrono>
#include <thread>
#include <plf_list.h>
#include "lists.h"

std::random_device rd;  // Seed generator
std::mt19937 gen(rd()); // Mersenne Twister engine
std::uniform_real_distribution<> dis(0.0, 1.0);

#if defined(__x86_64__) || defined(_M_X64)
void test_avx()
{
    double arr[4] = {1, 2, 3, 4};
    __m256d sum = _mm256_setzero_pd();

    __m256d vec_p = _mm256_load_pd(arr);
    __m256d vec_f = _mm256_load_pd(arr);
    __m256d vec_mul = _mm256_mul_pd(vec_p, vec_f);
    sum = _mm256_add_pd(sum, vec_mul);

    double result[4];
    _mm256_storeu_pd(result, sum);
    double final_sum = result[0] + result[1] + result[2] + result[3];
    // std::cout << "final sum: " << final_sum << std::endl;
}

double apply_order_avx_sum(const double *p, size_t size)
{
    size_t i = 0;
    double final_sum = 0;
    if (size >= 4)
    {
        __m256d sum = _mm256_setzero_pd();
        while (i + 4 <= size)
        {
            double p_arr[4] = {*(p + i * 4), *(p + (i + 1) * 4), *(p + (i + 2) * 4), *(p + (i + 3) * 4)};
            double f_arr[4] = {*(p + i * 4 + 1), *(p + (i + 1) * 4 + 1), *(p + (i + 2) * 4 + 1), *(p + (i + 3) * 4 + 1)};
            __m256d vec_p = _mm256_loadu_pd(p_arr);
            __m256d vec_f = _mm256_loadu_pd(f_arr);
            __m256d vec_mul = _mm256_mul_pd(vec_p, vec_f);
            sum = _mm256_add_pd(sum, vec_mul);
            i += 4;
        }
        double result[4];
        _mm256_storeu_pd(result, sum);
        final_sum = result[0] + result[1] + result[2] + result[3];
    }

    while (i < size)
    {
        final_sum += *(p + i * 4) * *(p + 1 + i * 4);
        i++;
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
#endif
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
    // std::cout << sum << std::endl;
}


// int main()
// {
//     size_t size = 1000;
//     float rm_factor = 0.1;
//     float insert_factor = 0.1;

//     plf::list<Order> orders;
//     for (int i = 0; i < size; ++i)
//     {
//         orders.push_back(Order{static_cast<double>(i), static_cast<double>(i)});
//     }

//     size_t rm_size = size * rm_factor;
//     std::vector<int> idx_rm = gen_random_int(0, size, rm_size);
//     for (auto it = idx_rm.begin(); it != idx_rm.end(); it++)
//     {
//         std::cout << "remove: " << *it << std::endl;
//         orders.remove(Order{static_cast<double>(*it), static_cast<double>(*it)});
//     }

//     size_t insert_size = (size - rm_size);
//     std::vector<int> idx_insert = gen_random_int(0, insert_size, insert_size * insert_factor);
//     std::sort(idx_insert.begin(), idx_insert.end());

//     auto it = orders.begin();
//     it++;

//     int i = 0;
//     for (auto it = orders.begin(); it != orders.end(); ++it)
//     {
//         auto find_idx = std::find(idx_insert.begin(), idx_insert.end(), i);
//         if (find_idx != idx_insert.end())
//         {
//             std::cout << "insert: " << i << std::endl;
//             orders.insert(it, Order{static_cast<double>(i), static_cast<double>(i)});
//         }

//         i++;
//     }

//     for (auto it = orders.begin(); it != orders.end(); it++)
//     {
//         std::cout << "element address: " << &it->price << "element: " << it->price << std::endl;
//     }

// #if defined(__x86_64__) || defined(_M_X64)
//     auto it_avx = orders.begin();
//     double *start_ptr = &it_avx->price;
//     double *last_ptr = start_ptr;

//     while (it_avx != orders.end())
//     {
//         int continuous_num = 1;
//         bool continuous = true;
//         while (continuous && it_avx != orders.end())
//         {
//             it_avx++;
//             double *curr_ptr = &it_avx->price;

//             size_t offset = curr_ptr - last_ptr;

//             if (offset != 4)
//             {
//                 std::cout << "apply avx: " << continuous_num << std::endl;
//                 for (double *j = start_ptr; j <= last_ptr; j = j + 4)
//                 {
//                     std::cout << *j << " " << *(j + 1) << std::endl;
//                 }
//                 double sum = apply_order_avx_sum(start_ptr, continuous_num);
//                 std::cout << "sum: " << sum << std::endl;
//                 continuous = false;
//                 start_ptr = curr_ptr;
//             }
//             else
//             {
//                 continuous_num++;
//             }
//             last_ptr = curr_ptr;
//         }
//     }
// #endif

//     return 0;
// }
#endif