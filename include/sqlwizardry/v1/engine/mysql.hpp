
#ifndef INCLUDED_SQLWIZARDRY_V1_ENGINE_MYSQL_HPP
#define INCLUDED_SQLWIZARDRY_V1_ENGINE_MYSQL_HPP
#include <string>

namespace sqlwizardry {
namespace engine{
struct MySQL {
    std::string name;
    auto run(const std::string& query) {
    }
};
}
}

#endif