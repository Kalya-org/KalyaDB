#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kalyadb {

/// Column definition for the experimental relational catalog.
struct Column {
    std::string name;
    std::string type; // "TEXT" or "INTEGER" in this milestone.
};

/// Creates a new experimental KalyaDB file, replacing any existing file.
void create_database(const std::filesystem::path& path);

/// Appends one UTF-8 record to an existing database file.
void append_record(const std::filesystem::path& path, const std::string& record);

/// Reads all records from a database file in insertion order.
std::vector<std::string> read_records(const std::filesystem::path& path);

/// Legacy prototype API retained for tests and old experimental records.
void create_table(const std::filesystem::path& path, const std::string& table_name);
std::vector<std::string> list_tables(const std::filesystem::path& path);
void insert_into_table(const std::filesystem::path& path,
                       const std::string& table_name,
                       const std::string& record);
std::vector<std::string> read_table(const std::filesystem::path& path,
                                    const std::string& table_name);

/// Creates a schema-aware table using the initial TEXT and INTEGER types.
void create_table_schema(const std::filesystem::path& path,
                         const std::string& table_name,
                         const std::vector<Column>& columns);

/// Returns a table's column definitions in declaration order.
std::vector<Column> get_table_schema(const std::filesystem::path& path,
                                     const std::string& table_name);

/// Inserts a row after checking its value count and declared data types.
void insert_row(const std::filesystem::path& path,
                const std::string& table_name,
                const std::vector<std::string>& values);

/// Reads schema-aware rows in insertion order.
std::vector<std::vector<std::string>> read_rows(
    const std::filesystem::path& path,
    const std::string& table_name);

} // namespace kalyadb
