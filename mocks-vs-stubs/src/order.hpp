#ifndef ORDER_HPP
#define ORDER_HPP

#include "warehouse.hpp"

class Order
{
    std::string product_;
private:
    size_t quantity_ = 0;
    bool is_filled_ = false;

public:
    Order() = default;

    Order(const std::string& product, size_t quantity_)
        : product_(product), quantity_(quantity_)
    {
    }

    void fill(Warehouse& warehouse)
    {
        if (warehouse.has_inventory(product_, quantity_))
        {
            warehouse.remove(product_, quantity_);

            is_filled_ = true;
        }
    }

    const std::string& product() const
    {
        return product_;
    }

    size_t quantity() const
    {
        return quantity_;
    }

    bool is_filled() const
    {
        return is_filled_;
    }
};

#endif