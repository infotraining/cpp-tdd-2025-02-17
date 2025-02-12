#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

#include "warehouse.hpp"
#include "order.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

struct MockWarehouse : Warehouse
{
    MOCK_METHOD(bool, has_inventory, (const std::string&, size_t), (const, override));
    MOCK_METHOD(void, add, (const std::string&, size_t ), (override));
    MOCK_METHOD(void, remove, (const std::string&, size_t), (override));
    MOCK_METHOD(size_t, get_inventory, (const std::string&), (const, override));
};

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

class OrderInteractionsTests : public ::testing::Test
{
protected:
    const string talisker = "Talisker";
    const string highland_park = "Highland Park";

    NiceMock<MockWarehouse> warehouse_;

public:
    OrderInteractionsTests() = default;
};

TEST_F(OrderInteractionsTests, FillingOrderRemovesInventoryIfEnoughInStock)
{
    Order order{talisker, 50};

    ON_CALL(warehouse_, has_inventory(talisker, 50)).WillByDefault(Return(true));
    EXPECT_CALL(warehouse_, remove(talisker, 50)).Times(1);

    order.fill(warehouse_);
}

TEST_F(OrderInteractionsTests, OrderIsFilledIfEnoughInStock)
{
    Order order{talisker, 50};

    ON_CALL(warehouse_, has_inventory(talisker, 50)).WillByDefault(Return(true));
    EXPECT_CALL(warehouse_, remove(_, _));

    order.fill(warehouse_);

    ASSERT_TRUE(order.is_filled());
}

TEST_F(OrderInteractionsTests, FillingOrderDoesNotRemoveInventoryIfNotEnoughInStock)
{
    Order order{talisker, 50};

    ON_CALL(warehouse_, has_inventory(talisker, 50)).WillByDefault(Return(false));
    EXPECT_CALL(warehouse_, remove(_, _)).Times(0);

    order.fill(warehouse_);
}

TEST_F(OrderInteractionsTests, OrderIsNotFilledIfNotEnoughInStock)
{
    Order order{talisker, 50};

    ON_CALL(warehouse_, has_inventory(talisker, 50)).WillByDefault(Return(false));

    order.fill(warehouse_);

    ASSERT_FALSE(order.is_filled());
}
