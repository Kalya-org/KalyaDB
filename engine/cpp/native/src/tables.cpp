#include "kalyadb/storage.hpp"

#include <stdexcept>

namespace kalyadb {
namespace {
const std::string kTablePrefix = "KALYADB_INTERNAL_TABLE_V1|";
const std::string kRowPrefix = "KALYADB_INTERNAL_ROW_V1|";

void validate_name(const std::string& name) {
    if (name.empty() || name.size() > 255 ||
        name.find('\0') != std::string::npos || name.find('|') != std::string::npos) {
        throw std::invalid_argument("Table name must be 1-255 bytes and cannot contain NUL or '|'.");
    }
}

std::vector<std::string> all_records(const std::filesystem::path& path) {
    return read_records(path);
}
}

void create_table(const std::filesystem::path& path, const std::string& name) {
    validate_name(name);
    const auto tables = list_tables(path);
    for (const auto& existing : tables) {
        if (existing == name) throw std::runtime_error("Table already exists.");
    }
    append_record(path, kTablePrefix + name);
}

std::vector<std::string> list_tables(const std::filesystem::path& path) {
    std::vector<std::string> result;
    for (const auto& record : all_records(path)) {
        if (record.compare(0, kTablePrefix.size(), kTablePrefix) == 0) {
            const auto name = record.substr(kTablePrefix.size());
            validate_name(name);
            bool seen = false;
            for (const auto& item : result) if (item == name) seen = true;
            if (!seen) result.push_back(name);
        }
    }
    return result;
}

void insert_into_table(const std::filesystem::path& path, const std::string& name,
                       const std::string& record) {
    validate_name(name);
    const auto tables = list_tables(path);
    bool found = false;
    for (const auto& item : tables) if (item == name) found = true;
    if (!found) throw std::runtime_error("Table does not exist.");
    append_record(path, kRowPrefix + name + "|" + record);
}

std::vector<std::string> read_table(const std::filesystem::path& path,
                                    const std::string& name) {
    validate_name(name);
    const auto tables = list_tables(path);
    bool found = false;
    for (const auto& item : tables) if (item == name) found = true;
    if (!found) throw std::runtime_error("Table does not exist.");

    std::vector<std::string> result;
    const auto prefix = kRowPrefix + name + "|";
    for (const auto& record : all_records(path)) {
        if (record.compare(0, prefix.size(), prefix) == 0)
            result.push_back(record.substr(prefix.size()));
    }
    return result;
}

} // namespace kalyadb
