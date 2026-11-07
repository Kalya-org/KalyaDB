#include "kalyadb/storage.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace kalyadb {
namespace {

constexpr std::array<char, 8> kMagic{{'K', 'A', 'L', 'Y', 'A', 'D', 'B', '\0'}};
constexpr std::uint32_t kFormatVersion = 1;
constexpr std::uint32_t kMaxRecordBytes = 1024 * 1024;

void write_u32(std::ostream& output, std::uint32_t value) {
    const char bytes[4] = {
        static_cast<char>(value & 0xff),
        static_cast<char>((value >> 8) & 0xff),
        static_cast<char>((value >> 16) & 0xff),
        static_cast<char>((value >> 24) & 0xff)
    };
    output.write(bytes, sizeof(bytes));
    if (!output) {
        throw std::runtime_error("Failed to write database file.");
    }
}

bool read_u32(std::istream& input, std::uint32_t& value) {
    char bytes[4];
    input.read(bytes, sizeof(bytes));
    if (input.gcount() != static_cast<std::streamsize>(sizeof(bytes))) {
        return false;
    }

    value =
        static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[0])) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[1])) << 8) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[2])) << 16) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[3])) << 24);
    return true;
}

void write_header(std::ostream& output) {
    output.write(kMagic.data(), static_cast<std::streamsize>(kMagic.size()));
    if (!output) {
        throw std::runtime_error("Failed to write database header.");
    }
    write_u32(output, kFormatVersion);
}

void validate_header(std::istream& input) {
    std::array<char, kMagic.size()> magic{};
    input.read(magic.data(), static_cast<std::streamsize>(magic.size()));
    if (!input || magic != kMagic) {
        throw std::runtime_error("File is not a valid KalyaDB database.");
    }

    std::uint32_t version = 0;
    if (!read_u32(input, version)) {
        throw std::runtime_error("Database header is incomplete.");
    }
    if (version != kFormatVersion) {
        throw std::runtime_error("Unsupported KalyaDB file format version.");
    }
}

} // namespace

void create_database(const std::filesystem::path& path) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Could not create database file.");
    }
    write_header(output);
}

void append_record(const std::filesystem::path& path, const std::string& record) {
    if (record.size() > kMaxRecordBytes ||
        record.size() > std::numeric_limits<std::uint32_t>::max()) {
        throw std::length_error("Record exceeds the 1 MiB prototype limit.");
    }

    {
        std::ifstream input(path, std::ios::binary);
        if (!input) {
            throw std::runtime_error("Could not open database file.");
        }
        validate_header(input);
    }

    std::ofstream output(path, std::ios::binary | std::ios::app);
    if (!output) {
        throw std::runtime_error("Could not open database file for writing.");
    }

    write_u32(output, static_cast<std::uint32_t>(record.size()));
    output.write(record.data(), static_cast<std::streamsize>(record.size()));
    if (!output) {
        throw std::runtime_error("Failed to append record.");
    }
}

std::vector<std::string> read_records(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Could not open database file.");
    }

    validate_header(input);
    std::vector<std::string> records;

    while (input.peek() != std::char_traits<char>::eof()) {
        std::uint32_t size = 0;
        if (!read_u32(input, size)) {
            throw std::runtime_error("Database contains a truncated record length.");
        }
        if (size > kMaxRecordBytes) {
            throw std::runtime_error("Database record exceeds the 1 MiB prototype limit.");
        }

        std::string record(size, '\0');
        input.read(record.data(), static_cast<std::streamsize>(size));
        if (input.gcount() != static_cast<std::streamsize>(size)) {
            throw std::runtime_error("Database contains a truncated record.");
        }
        records.push_back(std::move(record));
    }

    if (!input.eof()) {
        throw std::runtime_error("Failed while reading database file.");
    }
    return records;
}

} // namespace kalyadb
