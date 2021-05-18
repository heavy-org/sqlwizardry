#include <sqlwizardry/v1/all.hpp>
#include <gtest/gtest.h>

using namespace sqlwizardry::column;
SQLWIZARDRY_TABLE(User,
                  "user",
                  (Column<std::string>) username,
                  (Column<std::string>) password
                 )


TEST(Declaration, BasicTableDeclarationSelect) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user\n");
}

TEST(Declaration, BasicTableDeclarationSelectWithPred) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\"\n");
}

TEST(Declaration, BasicTableDeclarationSelectWithPredAndOrder) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .order_by(User::username)
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC\n");
}



TEST(Declaration, BasicTableDeclarationSelectWithPredAndOrderAndLimit) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .order_by(User::username)
                   .limit(1)
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC LIMIT 1\n");
}

TEST(Declaration, BasicTableDeclarationSelectWithPredAndOrderAndLimit2) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .order_by(User::username)
                   .limit<1>()
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC LIMIT 1\n");
}

TEST(Declaration, BasicTableDeclarationSelectWithPredAndOrderAndLimitAndOffSet) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .order_by(User::username)
                   .limit(1)
                   .offset(1)
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC LIMIT 1 OFFSET 1\n");
}

TEST(Declaration, BasicTableDeclarationSelectWithPredAndOrderAndLimitAndOffset2) {
    std::stringstream ss;
    sqlwizardry::engine::Debug debugEngine{ss};
    auto results = User::query(debugEngine)
                   .select()
                   .where(User::username == "Heavyhat")
                   .order_by(User::username)
                   .limit<1>()
                   .offset<1>()
                   .all();
    results.begin();
    EXPECT_EQ(ss.str(), "SELECT user.username, user.password FROM user WHERE user.username = \"Heavyhat\" ORDER BY user.username ASC LIMIT 1 OFFSET 1\n");
}