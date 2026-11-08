#include "kalyadb/query.hpp"
#include "kalyadb/storage.hpp"

#include <emscripten/bind.h>

#include <cctype>
#include <filesystem>
#include <string>

namespace {
const std::filesystem::path database_path = "/kalya-playground.kdb";

void ensure_database() {
    if (!std::filesystem::exists(database_path)) {
        kalyadb::create_database(database_path);
    }
}

std::string first_keyword(const std::string& sql) {
    std::string word;
    for (const unsigned char ch : sql) {
        if (std::isspace(ch)) {
            if (!word.empty()) break;
            continue;
        }
        if (!std::isalpha(ch)) break;
        word.push_back(static_cast<char>(std::toupper(ch)));
    }
    return word;
}

emscripten::val execute_sql(const std::string& sql) {
    emscripten::val result = emscripten::val::object();
    try {
        ensure_database();
        const auto query = kalyadb::execute(database_path, sql);
        const auto keyword = first_keyword(sql);
        emscripten::val output_columns = emscripten::val::array();
        emscripten::val output_rows = emscripten::val::array();

        for (std::size_t i = 0; i < query.columns.size(); ++i) {
            output_columns.set(i, query.columns[i]);
        }
        for (std::size_t row = 0; row < query.rows.size(); ++row) {
            emscripten::val output_row = emscripten::val::array();
            for (std::size_t column = 0; column < query.rows[row].size(); ++column) {
                output_row.set(column, query.rows[row][column]);
            }
            output_rows.set(row, output_row);
        }

        result.set("ok", true);
        result.set("columns", output_columns);
        result.set("rows", output_rows);
        if (keyword == "SELECT") {
            result.set("message", std::to_string(query.rows.size()) + " row(s) returned.");
        } else if (keyword == "INSERT") {
            result.set("message", "1 row inserted.");
        } else if (keyword == "CREATE") {
            result.set("message", "Table created.");
        } else {
            result.set("message", "SQL statement executed.");
        }
    } catch (const std::exception& error) {
        result.set("ok", false);
        result.set("columns", emscripten::val::array());
        result.set("rows", emscripten::val::array());
        result.set("message", error.what());
    }
    return result;
}

void reset_database() {
    std::error_code ignored;
    std::filesystem::remove(database_path, ignored);
    kalyadb::create_database(database_path);
}
} // namespace

EMSCRIPTEN_BINDINGS(kalyadb_playground) {
    emscripten::function("executeSql", &execute_sql);
    emscripten::function("resetDatabase", &reset_database);
}
