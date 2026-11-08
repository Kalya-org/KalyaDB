#include "kalyadb/query.hpp"

#include "kalyadb/storage.hpp"

#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace kalyadb {
namespace {

class Parser {
public:
    explicit Parser(const std::string& input) : input_(input) {}

    QueryResult parse(const std::filesystem::path& path) {
        skip_space();

        if (consume_keyword("CREATE")) {
            expect_keyword("TABLE");
            const auto table = identifier();
            expect('(');
            std::vector<Column> columns;
            do {
                Column column;
                column.name = identifier();
                column.type = type_name();
                columns.push_back(std::move(column));
                skip_space();
                if (!consume(',')) break;
            } while (true);
            expect(')');
            finish();
            create_table_schema(path, table, columns);
            return {};
        }

        if (consume_keyword("INSERT")) {
            expect_keyword("INTO");
            const auto table = identifier();
            expect_keyword("VALUES");
            expect('(');
            std::vector<std::string> values;
            do {
                values.push_back(value());
                skip_space();
                if (!consume(',')) break;
            } while (true);
            expect(')');
            finish();
            insert_row(path, table, values);
            return {};
        }

        if (consume_keyword("SELECT")) {
            expect('*');
            expect_keyword("FROM");
            const auto table = identifier();
            finish();
            QueryResult result;
            for (const auto& column : get_table_schema(path, table)) {
                result.columns.push_back(column.name);
            }
            result.rows = read_rows(path, table);
            return result;
        }

        throw std::invalid_argument("Unsupported SQL statement. Use CREATE TABLE, INSERT INTO, or SELECT * FROM.");
    }

private:
    const std::string& input_;
    std::size_t position_ = 0;

    void skip_space() {
        while (position_ < input_.size() &&
               std::isspace(static_cast<unsigned char>(input_[position_]))) {
            ++position_;
        }
    }

    bool consume_keyword(const std::string& keyword) {
        skip_space();
        if (position_ + keyword.size() > input_.size()) return false;
        for (std::size_t i = 0; i < keyword.size(); ++i) {
            const auto actual = static_cast<unsigned char>(input_[position_ + i]);
            if (std::toupper(actual) != keyword[i]) return false;
        }
        const auto end = position_ + keyword.size();
        if (end < input_.size()) {
            const auto next = static_cast<unsigned char>(input_[end]);
            if (std::isalnum(next) || next == '_') return false;
        }
        position_ = end;
        return true;
    }

    void expect_keyword(const std::string& keyword) {
        if (!consume_keyword(keyword)) {
            throw std::invalid_argument("Expected SQL keyword: " + keyword);
        }
    }

    std::string identifier() {
        skip_space();
        if (position_ >= input_.size()) throw std::invalid_argument("Expected an identifier.");
        const auto first = static_cast<unsigned char>(input_[position_]);
        if (!(std::isalpha(first) || first == '_')) throw std::invalid_argument("Invalid identifier.");
        const auto start = position_++;
        while (position_ < input_.size()) {
            const auto current = static_cast<unsigned char>(input_[position_]);
            if (!(std::isalnum(current) || current == '_')) break;
            ++position_;
        }
        return input_.substr(start, position_ - start);
    }

    std::string type_name() {
        if (consume_keyword("TEXT")) return "TEXT";
        if (consume_keyword("INTEGER")) return "INTEGER";
        throw std::invalid_argument("Unsupported column type. Use TEXT or INTEGER.");
    }

    bool consume(char expected) {
        skip_space();
        if (position_ < input_.size() && input_[position_] == expected) {
            ++position_;
            return true;
        }
        return false;
    }

    void expect(char expected) {
        if (!consume(expected)) {
            throw std::invalid_argument(std::string("Expected '") + expected + "'.");
        }
    }

    std::string value() {
        skip_space();
        if (position_ < input_.size() && input_[position_] == static_cast<char>(39)) {
            ++position_;
            std::string text;
            while (position_ < input_.size()) {
                const char current = input_[position_++];
                if (current != static_cast<char>(39)) {
                    text.push_back(current);
                    continue;
                }
                if (position_ < input_.size() && input_[position_] == static_cast<char>(39)) {
                    text.push_back(static_cast<char>(39));
                    ++position_;
                    continue;
                }
                return text;
            }
            throw std::invalid_argument("Unterminated SQL string literal.");
        }

        const auto start = position_;
        if (position_ < input_.size() && input_[position_] == '-') ++position_;
        const auto digits = position_;
        while (position_ < input_.size() &&
               std::isdigit(static_cast<unsigned char>(input_[position_]))) {
            ++position_;
        }
        if (digits == position_) throw std::invalid_argument("Expected a quoted TEXT value or an INTEGER value.");
        return input_.substr(start, position_ - start);
    }

    void finish() {
        skip_space();
        if (position_ < input_.size() && input_[position_] == ';') {
            ++position_;
            skip_space();
        }
        if (position_ != input_.size()) {
            throw std::invalid_argument("Unexpected text after SQL statement.");
        }
    }
};

} // namespace

QueryResult execute(const std::filesystem::path& path, const std::string& sql) {
    return Parser(sql).parse(path);
}

} // namespace kalyadb
