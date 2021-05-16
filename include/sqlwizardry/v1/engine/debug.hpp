
#ifndef INCLUDED_SQLWIZARDRY_V1_ENGINE_DEBUG_HPP
#define INCLUDED_SQLWIZARDRY_V1_ENGINE_DEBUG_HPP
#include <string>
#include <iostream>

namespace sqlwizardry {
namespace engine{
struct Debug {
    std::ostream& os;
    auto run(const std::string& query) {
        os << query << std::endl;
    }
};
}
}

#endif