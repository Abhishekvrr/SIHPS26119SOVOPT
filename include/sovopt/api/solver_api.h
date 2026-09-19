#pragma once

#include <string>

namespace sovopt::api
{

class SolverAPI
{
public:
    static int run(
        const std::string& input_json,
        std::string& output_json);
};

} // namespace sovopt::api