#pragma once
#include <exception>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "../query/sql_lexer.hpp"

class ParsingException final : std::exception {
    using Token = SQLLexer::Token;
    using Category = Token::Category;

public:
    explicit ParsingException(const std::string_view msg): message({msg.begin(), msg.end()}) {}
    explicit ParsingException(const Category was, const Category expected)
        : ParsingException(fmt::format("Expected {}, got {}", magic_enum::enum_name(expected), magic_enum::enum_name(was))) {}

    explicit ParsingException(const Category was, const std::vector<Category>& expected)
        : ParsingException(
            fmt::format("Expected one of {}, got {}",
            expected | std::views::transform(
                [](const Category cat) {
                    return std::string(magic_enum::enum_name(cat));
                })
            | std::ranges::to<std::vector>(),
            magic_enum::enum_name(was))) {}

    [[nodiscard]] const std::string& getMessage() const noexcept { return message; }

private:
    std::string message{};
};