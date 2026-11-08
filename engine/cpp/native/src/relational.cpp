#include "kalyadb/storage.hpp"

#include <charconv>
#include <cctype>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace kalyadb {
namespace {

const std::string kSchemaPrefix = "KALYADB_SCHEMA_V2|";
const std::string kDataPrefix = "KALYADB_DATA_V2|";

void validate_identifier(const std::string& value, const std::string& kind) {
    if (value.empty() || value.size() > 63 ||
        !(std::isalpha(static_cast<unsigned char>(value[0])) || value[0] == '_')) {
        throw std::invalid_argument("Invalid " + kind + " identifier.");
    }
    for (const unsigned char ch : value) {
        if (!(std::isalnum(ch) || ch == '_')) {
            throw std::invalid_argument("Invalid " + kind + " identifier.");
        }
    }
}

bool is_integer(const std::string& value) {
    if (value.empty()) return false;
    long long parsed = 0;
    const char* begin = value.data();
    const char* end = begin + value.size();
    const auto result = std::from_chars(begin, end, parsed);
    return result.ec == std::errc{} && result.ptr == end;
}

void append_u32(std::string& output, std::uint32_t value) {
    output.push_back(static_cast<char>(value & 0xff));
    output.push_back(static_cast<char>((value >> 8) & 0xff));
    output.push_back(static_cast<char>((value >> 16) & 0xff));
    output.push_back(static_cast<char>((value >> 24) & 0xff));
}

bool take_u32(const std::string& input, std::size_t& offset, std::uint32_t& value) {
    if (offset + 4 > input.size()) return false;
    value =
        static_cast<std::uint32_t>(static_cast<unsigned char>(input[offset])) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(input[offset + 1])) << 8) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(input[offset + 2])) << 16) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(input[offset + 3])) << 24);
    offset += 4;
    return true;
}

std::string schema_record(const std::string& table, const std::vector<Column>& columns) {
    std::string record = kSchemaPrefix + table + "|";
    for (std::size_t i = 0; i < columns.size(); ++i) {
        if (i) record.push_back(',');
        record += columns[i].name + ":" + columns[i].type;
    }
    return record;
}

std::string data_prefix(const std::string& table) {
    return kDataPrefix + table + "|";
}

} // namespace

void create_table_schema(const std::filesystem::path& path,
                         const std::string& table_name,
                         const std::vector<Column>& columns) {
    validate_identifier(table_name, "table");
    if (columns.empty() || columns.size() > 128) {
        throw std::invalid_argument("A table must have between 1 and 128 columns.");
    }

    for (const auto& existing : list_tables(path)) {
        if (existing == table_name) {
            throw std::runtime_error("A legacy table with this name already exists. Reset the prototype database before recreating it with a schema.");
        }
    }

    const auto records = read_records(path);
    const auto prefix = kSchemaPrefix + table_name + "|";
    for (const auto& record : records) {
        if (record.compare(0, prefix.size(), prefix) == 0) {
            throw std::runtime_error("Table already exists.");
        }
    }

    std::vector<std::string> names;
    for (const auto& column : columns) {
        validate_identifier(column.name, "column");
        if (column.type != "TEXT" && column.type != "INTEGER") {
            throw std::invalid_argument("Unsupported type for column '" + column.name + "'. Use TEXT or INTEGER.");
        }
        for (const auto& name : names) {
            if (name == column.name) throw std::invalid_argument("Duplicate column name.");
        }
        names.push_back(column.name);
    }

    append_record(path, schema_record(table_name, columns));
}

std::vector<Column> get_table_schema(const std::filesystem::path& path,
                                     const std::string& table_name) {
    validate_identifier(table_name, "table");
    const auto prefix = kSchemaPrefix + table_name + "|";
    const auto records = read_records(path);

    for (const auto& record : records) {
        if (record.compare(0, prefix.size(), prefix) != 0) continue;
        const std::string encoded = record.substr(prefix.size());
        std::vector<Column> columns;
        std::size_t start = 0;
        while (start <= encoded.size()) {
            const auto comma = encoded.find(',', start);
            const auto end = comma == std::string::npos ? encoded.size() : comma;
            const auto token = encoded.substr(start, end - start);
            const auto colon = token.find(':');
            if (colon == std::string::npos) throw std::runtime_error("Corrupt table schema.");
            columns.push_back({token.substr(0, colon), token.substr(colon + 1)});
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
        if (columns.empty()) throw std::runtime_error("Corrupt table schema.");
        return columns;
    }

    throw std::runtime_error("Table schema does not exist. Create the table with column definitions first.");
}

void insert_row(const std::filesystem::path& path,
                const std::string& table_name,
                const std::vector<std::string>& values) {
    const auto columns = get_table_schema(path, table_name);
    if (values.size() != columns.size()) {
        throw std::invalid_argument("Column count does not match the table schema.");
    }

    std::string record = data_prefix(table_name);
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (columns[i].type == "INTEGER" && !is_integer(values[i])) {
            throw std::invalid_argument("Column '" + columns[i].name + "' requires an INTEGER value.");
        }
        if (values[i].size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::length_error("Column value is too large.");
        }
        append_u32(record, static_cast<std::uint32_t>(values[i].size()));
        record += values[i];
    }
    append_record(path, record);
}

std::vector<std::vector<std::string>> read_rows(
    const std::filesystem::path& path,
    const std::string& table_name) {
    const auto columns = get_table_schema(path, table_name);
    const auto prefix = data_prefix(table_name);
    std::vector<std::vector<std::string>> rows;

    for (const auto& record : read_records(path)) {
        if (record.compare(0, prefix.size(), prefix) != 0) continue;
        std::size_t offset = prefix.size();
        std::vector<std::string> row;
        for (std::size_t i = 0; i < columns.size(); ++i) {
            std::uint32_t size = 0;
            if (!take_u32(record, offset, size) || size > record.size() - offset) {
                throw std::runtime_error("Corrupt row data.");
            }
            row.push_back(record.substr(offset, size));
            offset += size;
        }
        if (offset != record.size()) throw std::runtime_error("Row contains unexpected trailing data.");
        rows.push_back(std::move(row));
    }
    return rows;
}

} // namespace kalyadb
