#include <random>
#include <vector>
#include <algorithm>

double gen_random_double() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

std::vector<int> gen_random_int(int lower, int upper, int nums)
{
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator

    // Create a uniform distribution in the specified range
    std::uniform_int_distribution<> distr(lower, upper);

    std::vector<int> results(nums);
    for (int i = 0; i < nums; ++i)
    {
        // Generate a random number
        bool unique = false;
        while (!unique)
        {
            int random_integer = distr(eng);
            auto it = std::find(results.begin(), results.end(), random_integer);
            if (it == results.end())
            {
                unique = true;
                results[i] = random_integer;
            }
        }
    }
    return results;
}