#include "kalyadb/query.hpp"
#include "kalyadb/storage.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "kalyadb_query_test.kdb";

    try {
        std::filesystem::remove(path);
        kalyadb::create_database(path);

        kalyadb::execute(path, "CREATE TABLE users (name TEXT, age INTEGER);");
        kalyadb::execute(path, "INSERT INTO users VALUES ('Ada', 25);");
        kalyadb::execute(path, "INSERT INTO users VALUES ('O''Reilly', 42)");
        kalyadb::execute(path, "INSERT INTO users VALUES ('Grace Hopper', 85)");

        const auto result = kalyadb::execute(path, " SELECT * FROM users ; ");
        const bool selected =
            result.columns == std::vector<std::string>{"name", "age"} &&
            result.rows == std::vector<std::vector<std::string>>{
                {"Ada", "25"}, {"O'Reilly", "42"}, {"Grace Hopper", "85"}
            };

        bool wrong_type_rejected = false;
        try {
            kalyadb::execute(path, "INSERT INTO users VALUES ('Linus', 'young')");
        } catch (const std::invalid_argument&) {
            wrong_type_rejected = true;
        }

        bool wrong_count_rejected = false;
        try {
            kalyadb::execute(path, "INSERT INTO users VALUES ('Only one value')");
        } catch (const std::invalid_argument&) {
            wrong_count_rejected = true;
        }

        bool unsupported_type_rejected = false;
        try {
            kalyadb::execute(path, "CREATE TABLE broken (score FLOAT)");
        } catch (const std::invalid_argument&) {
            unsupported_type_rejected = true;
        }

        bool unknown_table_rejected = false;
        try {
            kalyadb::execute(path, "SELECT * FROM missing");
        } catch (const std::runtime_error&) {
            unknown_table_rejected = true;
        }

        // Reopen the file and verify schema and rows are persisted.
        const auto reopened = kalyadb::execute(path, "SELECT * FROM users");
        const bool persisted = reopened.columns == result.columns && reopened.rows == result.rows;

        std::filesystem::remove(path);
        return selected && wrong_type_rejected && wrong_count_rejected &&
               unsupported_type_rejected && unknown_table_rejected && persisted
            ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(path, ignored);
        return EXIT_FAILURE;
    }
}
