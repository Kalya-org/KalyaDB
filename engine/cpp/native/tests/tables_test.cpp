#include "kalyadb/storage.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "kalyadb_tables_test.kdb";
    try {
        std::filesystem::remove(path);
        kalyadb::create_database(path);
        kalyadb::create_table(path, "users");
        kalyadb::create_table(path, "projects");
        kalyadb::insert_into_table(path, "users", "Ada");
        kalyadb::insert_into_table(path, "users", "Grace");
        kalyadb::insert_into_table(path, "projects", "KalyaDB");

        const bool ok =
            kalyadb::list_tables(path) == std::vector<std::string>{"users", "projects"} &&
            kalyadb::read_table(path, "users") == std::vector<std::string>{"Ada", "Grace"} &&
            kalyadb::read_table(path, "projects") == std::vector<std::string>{"KalyaDB"};
        std::filesystem::remove(path);
        return ok ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(path, ignored);
        return EXIT_FAILURE;
    }
}
