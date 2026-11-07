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

/// Creates a named table. Names must be 1-255 bytes and cannot contain NUL or '|'.
void create_table(const std::filesystem::path& path, const std::string& table_name);

/// Lists tables in their creation order.
std::vector<std::string> list_tables(const std::filesystem::path& path);

/// Inserts a record into an existing table.
void insert_into_table(const std::filesystem::path& path,
                       const std::string& table_name,
                       const std::string& record);

/// Reads records from a table in insertion order.
std::vector<std::string> read_table(const std::filesystem::path& path,
                                    const std::string& table_name);

} // namespace kalyadb
