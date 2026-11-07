#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kalyadb {

/// Executes one supported SQL statement.
/// CREATE TABLE and INSERT return an empty result; SELECT returns row values.
/// The current prototype supports one text value per row and no column definitions.
std::vector<std::string> execute(const std::filesystem::path& path,
                                 const std::string& sql);

} // namespace kalyadb
