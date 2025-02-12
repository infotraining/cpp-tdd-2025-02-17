#include <algorithm>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

using namespace std;

class Interface
{
public:
    virtual ~Interface() = default;

    virtual std::string get_name() const = 0;
    virtual std::string get_value(int x) = 0;
    virtual std::string get_value(int x) const = 0;
    virtual bool save_value(int k, const string& v) = 0;
    virtual std::vector<int> get_data(const std::string& filename) const = 0;
    virtual int generate() = 0;
};

struct MockInterface : Interface
{
    MOCK_METHOD(string, get_name, (), (const, override));
    MOCK_METHOD(std::string, get_value, (int), (override));
    MOCK_METHOD(std::string, get_value, (int), (const, override));
    MOCK_METHOD(bool, save_value, (int, const std::string&), (override));
    MOCK_METHOD(std::vector<int>, get_data, (const std::string&), (const, override));
    MOCK_METHOD(int, generate, (), (override));
};

namespace Legacy
{
    struct MockInterface : Interface
    {
        MOCK_CONST_METHOD0(get_name, std::string());
        MOCK_METHOD1(get_value, std::string(int));
        MOCK_CONST_METHOD1(get_value, string(int));
        MOCK_METHOD2(save_value, bool(int, const std::string&));
        MOCK_CONST_METHOD1(get_data, vector<int>(const std::string&));
        MOCK_METHOD0(generate, int());
    };
}

TEST(GMockDefaultValuesTests, ReturningDefaultValues)
{
    testing::NiceMock<MockInterface> mock;

    ASSERT_EQ(0, mock.generate());
    ASSERT_EQ(""s, mock.get_name());
    ASSERT_THAT(mock.get_data("text"), ::testing::IsEmpty());
    ASSERT_THAT(mock.get_data("data.txt"), ::testing::Eq(vector<int> {}));
}

struct GMockDemoTests : ::testing::Test
{
    ::testing::NiceMock<MockInterface> mock;

    void SetUp()
    {
        ::testing::DefaultValue<int>::Set(42);
    }

    void TearDown()
    {
        ::testing::DefaultValue<int>::Clear();
    }
};

TEST_F(GMockDemoTests, DefaultValuesCanBeSetForFixture)
{
    ASSERT_EQ(mock.generate(), 42);
}

TEST_F(GMockDemoTests, DefaultValueCanBeSetForMethodUsingOnCall)
{
    using ::testing::ElementsAre;
    using ::testing::Return;

    vector<int> vec = {1, 2, 3};

    ON_CALL(mock, generate()).WillByDefault(Return(665));
    ON_CALL(mock, get_data("text"s)).WillByDefault(Return(vec));

    ASSERT_EQ(mock.generate(), 665);
    ASSERT_EQ(mock.generate(), 665);
    ASSERT_THAT(mock.get_data("text"s), ElementsAre(1, 2, 3));
}

TEST_F(GMockDemoTests, DefaultValueCanBeSetForMethodUsingExpectCall)
{
    using ::testing::Return;

    EXPECT_CALL(mock, get_name()).WillRepeatedly(Return("jan"));

    ASSERT_EQ(mock.get_name(), "jan");
    ASSERT_EQ(mock.get_name(), "jan");
    ASSERT_EQ(mock.get_name(), "jan");

    ASSERT_EQ(mock.get_value(1), ""s);
}

TEST_F(GMockDemoTests, VerificationHowManyTimesMethodIsCalled1)
{
    using namespace ::testing;

    EXPECT_CALL(mock, get_name()).WillOnce(Return("adam"));

    ASSERT_EQ(mock.get_name(), "adam");
    //ASSERT_EQ(mock.get_name(), ""); // error - called twice
}

TEST_F(GMockDemoTests, VerificationHowManyTimesMethodIsCalled2)
{
    using namespace ::testing;

    EXPECT_CALL(mock, save_value(_, _)).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, save_value(Gt(0), _))
        .Times(AtMost(3))
        .WillRepeatedly(Return(true))
        .RetiresOnSaturation();


    ASSERT_TRUE(mock.save_value(1, "a"));
    ASSERT_FALSE(mock.save_value(-1, "z"));
    ASSERT_TRUE(mock.save_value(2, "b"));
    ASSERT_TRUE(mock.save_value(3, "c"));
    ASSERT_FALSE(mock.save_value(4, "d"));
}

TEST_F(GMockDemoTests, ReturnDifferentValuesBasedOnArgument)
{
    using namespace ::testing;

    EXPECT_CALL(mock, get_value(Gt(0))).WillRepeatedly(Return("positive"));
    EXPECT_CALL(mock, get_value(Lt(0))).WillRepeatedly(Return("negative"));

    ASSERT_EQ(mock.get_value(10), "positive");
    ASSERT_EQ(mock.get_value(-1), "negative");   
}

TEST_F(GMockDemoTests, ExpectingOrderedCalls)
{
    using namespace ::testing;

    InSequence s;

    vector<string> names = {"Jan", "Kowalski"};
    size_t index {};

    EXPECT_CALL(mock, get_name())
        .Times(2)
        .WillRepeatedly(Invoke([&names, &index] { return names[index++]; }));
    EXPECT_CALL(mock, generate()).WillOnce(Return(665));
    EXPECT_CALL(mock, save_value(665, "Jan Kowalski"));

    auto name1 = mock.get_name();
    auto name2 = mock.get_name();
    auto key = mock.generate();
    mock.save_value(key, name1 + " " + name2);
}

TEST_F(GMockDemoTests, SpyingOnParamtersInvokedInMock)
{
    using namespace ::testing;

    vector<int> spy;

    EXPECT_CALL(mock, get_value(_))
        .WillRepeatedly(Invoke([&spy](int arg) { spy.push_back(arg); return to_string(arg); }));

    ASSERT_THAT(mock.get_value(1), StrEq("1"));
    ASSERT_THAT(mock.get_value(2), StrEq("2"));
    ASSERT_THAT(mock.get_value(3), StrEq("3"));

    ASSERT_THAT(spy, ElementsAre(1, 2, 3));
}

TEST_F(GMockDemoTests, ReturningSequencedValues)
{
    using namespace ::testing;

    vector<string> data = {"zero", "one", "two", "three"};

    EXPECT_CALL(mock, get_value(_))
        .WillRepeatedly((Invoke([&data](int i) { return data[i]; })));

    ASSERT_THAT(mock.get_value(0), StrEq("zero"));
    ASSERT_THAT(mock.get_value(2), StrEq("two"));
}

TEST_F(GMockDemoTests, ThrowingAnException)
{
    using namespace ::testing;

    invalid_argument excpt("Invalid arg");
    EXPECT_CALL(mock, get_value(Lt(0))).WillRepeatedly(Throw(excpt));

    ASSERT_THROW(mock.get_value(-2), invalid_argument);
}

////////////////////////////////////////////////////////////////////////////////////////////
// move semantics
////////////////////////////////////////////////////////////////////////////////////////////

struct Gadget
{
    int id;

    explicit Gadget(int id)
        : id {id}
    {
    }

    virtual void play() const
    {
    }

    virtual ~Gadget() = default;
};

class MoveSemantics
{
public:
    virtual unique_ptr<Gadget> create_gadget(int id) = 0;
    virtual void sink(std::unique_ptr<Gadget> g) = 0;
    virtual ~MoveSemantics() = default;
};

class MockMoveSemantics : public MoveSemantics
{
public:
    MOCK_METHOD(unique_ptr<Gadget>, create_gadget, (int id), (override));
    MOCK_METHOD(void, sink, (unique_ptr<Gadget> g), (override));
};

namespace Legacy
{
    struct MockMoveSemantics : MoveSemantics
    {
        MOCK_METHOD1(create_gadget_, Gadget*(int));

        // wrapping method
        unique_ptr<Gadget> create_gadget(int id) override
        {
            return unique_ptr<Gadget>(create_gadget_(id));
        }

        MOCK_METHOD1(sink_, void(Gadget*));

        void sink(std::unique_ptr<Gadget> g) override
        {
            sink_(g.get());
        }
    };
}

void using_gadgets(MoveSemantics& mvs)
{
    auto gadget = mvs.create_gadget(1);

    mvs.sink(std::move(gadget));
}

TEST(AdvancedMocks, MocksWithMove)
{
    using namespace ::testing;

    MockMoveSemantics mock;

    EXPECT_CALL(mock, create_gadget(_)).WillOnce(Invoke([](int arg) { return make_unique<Gadget>(arg); }));
    EXPECT_CALL(mock, sink(_)).WillOnce([](std::unique_ptr<Gadget> g) { ASSERT_EQ(g->id, 1); });

    using_gadgets(mock);
}

namespace Legacy
{
    TEST(AdvancedMocks, MocksWithMove_Legacy)
    {
        using namespace ::testing;

        Legacy::MockMoveSemantics mock;

        EXPECT_CALL(mock, create_gadget_(_)).WillOnce(Invoke([](int id) { return new Gadget(id); }));
        EXPECT_CALL(mock, sink_(Field(&Gadget::id, Eq(1))));

        using_gadgets(mock);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
// Matchers
//

TEST(AssertsWithMatchers, SimpleMatchers)
{
    int x = 10;

    ASSERT_THAT(x, ::testing::Eq(10));

    using namespace ::testing;

    ASSERT_THAT(x, Gt(5));
    ASSERT_THAT(x, AllOf(Gt(5), Lt(15)));
}

TEST(AssertsWithMatchers, PointerMatchers)
{
    using namespace ::testing;

    int* ptr = nullptr;
    ASSERT_THAT(ptr, IsNull());

    int x = 10;
    ptr = &x;
    ASSERT_THAT(ptr, NotNull());

    const char* txt = "test";
    ASSERT_THAT(txt, AllOf(NotNull(), Not(StrEq(""))));
}

TEST(AssertsWithMatchers, StringMatchers)
{
    using namespace ::testing;

    string txt = "test of matcher";

    ASSERT_THAT(txt, StartsWith("test"));
    ASSERT_THAT(txt, EndsWith("matcher"));
    ASSERT_THAT(txt, StrEq("test of matcher"));
    ASSERT_THAT(txt, StrNe("test of Matcher"));
    ASSERT_THAT(txt, StrCaseEq("test of Matcher")); // case insensitive eq
}

class AssertsWithContainerMatchers : public ::testing::Test
{
protected:
    vector<int> vec = {1, 2, 3};
};

TEST_F(AssertsWithContainerMatchers, ContainerMatchers)
{
    using namespace ::testing;

    vector<int> expected = {1, 2, 3};

    ASSERT_THAT(vec, Not(IsEmpty()));
    ASSERT_THAT(vec, ContainerEq(expected));
    ASSERT_THAT(vec, ElementsAre(1, 2, 3));
    ASSERT_THAT(vec, ElementsAre(Gt(0), 2, Le(3)));

    vector<int> shuffled = {3, 1, 2};
    ASSERT_THAT(shuffled, UnorderedElementsAre(1, 2, 3));
}

// custom matchers

MATCHER(IsEven, std::string(negation ? "isn't" : "is") + " even") { return arg % 2 == 0; }

MATCHER_P(IsDivisible, value, std::string(negation ? "isn't" : "is")
    + " divisible by " + std::to_string(value)) { return arg % value == 0; }


TEST(CustomMatcherTests, MatcherMacro)
{
    ASSERT_THAT(6, IsEven());
    ASSERT_THAT(9, IsDivisible(3));
}