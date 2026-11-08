#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kalyadb {

struct QueryResult {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
};

/// Executes one supported SQL statement. CREATE and INSERT return empty results;
/// SELECT returns column names and rows. Initial types: TEXT and INTEGER.
QueryResult execute(const std::filesystem::path& path, const std::string& sql);

} // namespace kalyadb
