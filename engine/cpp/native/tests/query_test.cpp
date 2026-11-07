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

        kalyadb::execute(path, "CREATE TABLE users;");
        kalyadb::execute(path, "insert into users values ('Ada');");
        kalyadb::execute(path, "INSERT INTO users VALUES ('O''Reilly')");
        kalyadb::execute(path, "INSERT INTO users VALUES ('Grace Hopper')");

        const auto rows = kalyadb::execute(path, " SELECT * FROM users ; ");
        const bool correct =
            rows == std::vector<std::string>{"Ada", "O'Reilly", "Grace Hopper"};

        bool malformed_rejected = false;
        try {
            kalyadb::execute(path, "INSERT INTO users VALUES (42)");
        } catch (const std::invalid_argument&) {
            malformed_rejected = true;
        }

        bool unknown_table_rejected = false;
        try {
            kalyadb::execute(path, "SELECT * FROM missing");
        } catch (const std::runtime_error&) {
            unknown_table_rejected = true;
        }

        std::filesystem::remove(path);
        return correct && malformed_rejected && unknown_table_rejected
            ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(path, ignored);
        return EXIT_FAILURE;
    }
}
