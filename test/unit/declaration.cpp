#include <sqlwizardry/v1/all.hpp>
#include <gtest/gtest.h>

using namespace sqlwizardry;
SQLWIZARDRY_TABLE(User,
    "user",
    (Column<std::string>) username,
    (Column<std::string>) password
)

TEST(Declaration, BasicTableDeclaration) {
}