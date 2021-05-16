#include <sqlwizardry/v1/all.hpp>
#include <gtest/gtest.h>

using namespace sqlwizardry;
SQLWIZARDRY_TABLE(User,
    "user",
    (Column<std::string>) username,
    (Column<std::string>) password
)

TEST(Declaration, BasicTableDeclaration) {
    std::stringstream ss;
    engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
        .select()
        .where(User::username == "Heavyhat")
        .order_by(User::username)
        .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC\n");
}