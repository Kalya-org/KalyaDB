#include "kalyadb/storage.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "kalyadb_storage_test.kdb";

    try {
        std::filesystem::remove(path);
        kalyadb::create_database(path);

        const std::vector<std::string> expected{
            "Hello from KalyaDB",
            "Second record",
            ""
        };

        for (const auto& record : expected) {
            kalyadb::append_record(path, record);
        }

        const auto actual = kalyadb::read_records(path);
        std::filesystem::remove(path);

        return actual == expected ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(path, ignored);
        return EXIT_FAILURE;
    }
}
