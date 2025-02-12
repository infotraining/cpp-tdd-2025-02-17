#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

#include "order.hpp"
#include "warehouse.hpp"
#include "gtest/gtest.h"

using namespace std;

class WarehouseImpl : public Warehouse
{
    unordered_map<string, size_t> inventory_;

public:
    bool has_inventory(const std::string& product, size_t quantity) const override
    {
        return inventory_.at(product) >= quantity;
    }

    void add(const std::string& name, size_t count) override
    {
        inventory_.insert(make_pair(name, count));
    }

    size_t get_inventory(const std::string& name) const override
    {
        return inventory_.at(name);
    }

    void remove(const std::string& product, size_t quantity) override
    {
        inventory_.at(product) -= quantity;
    }
};

class OrderStateTests : public ::testing::Test
{
protected:
    const string talisker = "Talisker";
    const string highland_park = "Highland Park";

    WarehouseImpl warehouse_;
    Order order_;

public:
    OrderStateTests()
    {
        warehouse_.add(talisker, 50);
        warehouse_.add(highland_park, 25);
    }
};

TEST_F(OrderStateTests, OrderIsFilledIfEnoughInWarehouse)
{
    Order order{talisker, 50};

    order.fill(warehouse_);

    ASSERT_TRUE(order.is_filled());
}

TEST_F(OrderStateTests, OrderIsNotFilledIfNotEnoughInWarehouse)
{
    Order order{talisker, 51u};

    order.fill(warehouse_);

    ASSERT_FALSE(order.is_filled());
}

TEST_F(OrderStateTests, IfEnoughInStockItemsAreTransferedFromWarehouse)
{
    Order order{talisker, 50u};

    order.fill(warehouse_);

    ASSERT_EQ(warehouse_.get_inventory(talisker), 0u);
}

TEST_F(OrderStateTests, IfNotEnoughInStockItemsAreNotTransferedFromWarehouse)
{
    Order order{talisker, 51};

    order.fill(warehouse_);

    ASSERT_EQ(warehouse_.get_inventory(talisker), 50u);
}