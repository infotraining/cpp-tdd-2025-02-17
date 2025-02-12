#include <algorithm>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "document.hpp"

using namespace ::testing;

struct DocumentTests : ::testing::Test
{
    Document doc;
};

struct Document_DefaultConstructed : DocumentTests
{
};

TEST_F(Document_DefaultConstructed, TextIsEmpty)
{
    ASSERT_THAT(doc.text(), StrEq(""));
}

TEST_F(Document_DefaultConstructed, ZeroLength)
{
    ASSERT_THAT(doc.length(), Eq(0));
}

struct Document_ValueConstructed : Test
{
    Document doc{"abc"};
};

TEST_F(Document_ValueConstructed, TextIsSet)
{
    ASSERT_THAT(doc.text(), StrEq("abc"));
}

TEST_F(Document_ValueConstructed, Length)
{
    ASSERT_THAT(doc.length(), Eq(3));
}

struct Document_Clear : Document_ValueConstructed
{
};

TEST_F(Document_Clear, TextIsCleared)
{
    doc.clear();

    ASSERT_THAT(doc.text(), IsEmpty());
}

struct Document_AddText : Document_ValueConstructed
{
};

TEST_F(Document_AddText, TextIsAddedAtTheEnd)
{
    doc.add_text("def");

    ASSERT_THAT(doc.text(), StrEq("abcdef"));
}

struct Document_CaseConversion : Test
{
    Document doc{"abcDEF"};
};

TEST_F(Document_CaseConversion, ToUpperCase)
{
    doc.to_upper();

    ASSERT_THAT(doc.text(), StrEq("ABCDEF"));
}

TEST_F(Document_CaseConversion, ToLowerCase)
{
    doc.to_lower();

    ASSERT_THAT(doc.text(), StrEq("abcdef"));
}

struct Document_ReplacingText : Document_ValueConstructed
{
};

TEST_F(Document_ReplacingText, TextIsReplaced)
{
    doc.replace(0, 2, "xyz");

    ASSERT_THAT(doc.text(), StrEq("xyzc"));
}

struct Document_Memento : Document_ValueConstructed
{
};

TEST_F(Document_Memento, RestoresThePreviousState)
{
    auto snaphot = doc.create_memento();
    doc.clear();
    doc.set_memento(snaphot);

    ASSERT_THAT(doc.text(), StrEq("abc"));
}