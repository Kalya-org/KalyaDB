#include "kalyadb/query.hpp"

#include "kalyadb/storage.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

namespace kalyadb {
namespace {

class Parser {
public:
    explicit Parser(const std::string& input) : input_(input) {}

    std::vector<std::string> parse(const std::filesystem::path& path) {
        skip_space();

        if (consume_keyword("CREATE")) {
            expect_keyword("TABLE");
            const auto table = identifier();
            finish();
            create_table(path, table);
            return {};
        }

        if (consume_keyword("INSERT")) {
            expect_keyword("INTO");
            const auto table = identifier();
            expect_keyword("VALUES");
            expect('(');
            const auto value = string_literal();
            expect(')');
            finish();
            insert_into_table(path, table, value);
            return {};
        }

        if (consume_keyword("SELECT")) {
            expect('*');
            expect_keyword("FROM");
            const auto table = identifier();
            finish();
            return read_table(path, table);
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
        if (position_ >= input_.size()) {
            throw std::invalid_argument("Expected a table name.");
        }

        const auto first = static_cast<unsigned char>(input_[position_]);
        if (!(std::isalpha(first) || first == '_')) {
            throw std::invalid_argument("Invalid table name.");
        }

        const auto start = position_++;
        while (position_ < input_.size()) {
            const auto current = static_cast<unsigned char>(input_[position_]);
            if (!(std::isalnum(current) || current == '_')) break;
            ++position_;
        }
        return input_.substr(start, position_ - start);
    }

    void expect(char expected) {
        skip_space();
        if (position_ >= input_.size() || input_[position_] != expected) {
            throw std::invalid_argument(std::string("Expected '") + expected + "'.");
        }
        ++position_;
    }

    std::string string_literal() {
        skip_space();
        expect(''');
        std::string value;

        while (position_ < input_.size()) {
            const char current = input_[position_++];
            if (current != ''') {
                value.push_back(current);
                continue;
            }

            if (position_ < input_.size() && input_[position_] == ''') {
                value.push_back(''');
                ++position_;
                continue;
            }
            return value;
        }

        throw std::invalid_argument("Unterminated SQL string literal.");
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

std::vector<std::string> execute(const std::filesystem::path& path,
                                 const std::string& sql) {
    return Parser(sql).parse(path);
}

} // namespace kalyadb
