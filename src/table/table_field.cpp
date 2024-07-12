#include "table_field.hpp"

#include <charconv>

std::string Field::toString() const {
    return std::visit(Overload(
        [](const std::string& v){ return v; },
        [](const int v){ return std::to_string(v); },
        [](const bool v) { return v ? std::string("true") : std::string("false"); },
        [](const double v) { return std::to_string(v); }
    ), data);
}

Field Field::createFromToken(const SQLLexer::Token& token) {
    using Token = SQLLexer::Token;
    using Category = Token::Category;

    const auto beg = token.lexeme.data();
    const auto end = token.lexeme.data()+token.lexeme.size();

    // Valid Integer
    if (token.category == Category::IntLiteral) {
        int value{};

        if (auto [ptr, ec] = std::from_chars(beg, end, value); ec != std::errc())
            return Field("ERRORED");

        return Field(value);
    }

    // Valid Double
    if (token.category == Category::DoubleLiteral) {
        float value{};

        if (auto [ptr, ec] = std::from_chars(beg, end, value); ec != std::errc())
            return Field("ERRORED");

        return Field(value);
    }

    // Valid Boolean
    if (token.category == Category::BooleanLiteral) {
        return Field(std::ranges::equal(token.lexeme, "true", {}, tolower, tolower));
    }

    // Valid String
    if (token.lexeme.front() == '\'' && token.lexeme.back() == '\'') {
        return Field(std::string(token.lexeme.substr(1, token.lexeme.size()-2)));
    }

    return Field(std::string(token.lexeme));
}