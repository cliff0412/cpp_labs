#ifndef _LISTS_H_
#define _LISTS_H_
struct alignas(16) Order
{
    double price;
    double fee;

    bool operator==(const Order &other) const
    {
        return (price == other.price) && (fee == other.fee);
    }
};
double apply_order_avx_sum(const double *p, size_t size);
#endif