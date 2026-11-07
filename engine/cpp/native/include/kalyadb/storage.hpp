#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kalyadb {

/// Creates a new experimental KalyaDB file, replacing any existing file.
void create_database(const std::filesystem::path& path);

/// Appends one UTF-8 record to an existing database file.
void append_record(const std::filesystem::path& path, const std::string& record);

/// Reads all records from a database file in insertion order.
std::vector<std::string> read_records(const std::filesystem::path& path);

} // namespace kalyadb
