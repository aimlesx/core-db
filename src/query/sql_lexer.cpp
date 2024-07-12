#include "sql_lexer.hpp"

SQLLexer::SQLLexer() {
    using TC = Token::Category;

    std::vector<std::pair<std::string, TC>> mapping_initializer = {
        {"[+-]?\\d+", TC::IntLiteral},
        {R"('.*')", TC::StringLiteral},
        {R"([+-]?(\d+(\.\d+)?|\.\d+))", TC::DoubleLiteral},
        {"", TC::BooleanLiteral},

        {"create", TC::Create},
        {"select", TC::Select},
        {"insert", TC::Insert},
        {"into", TC::Into},
        {"delete", TC::Delete},
        {"drop", TC::Drop},
        {"from", TC::From},
        {"truncate", TC::Truncate},
        {"table", TC::Table},
        {"string", TC::String},
        {"int", TC::Int},
        {"bool", TC::Bool},
        {"double", TC::Double},
        {"\\(", TC::LeftParenthesis},
        {"\\)", TC::RightParenthesis},
        {"as", TC::As},
        {",", TC::Comma},
        {"\\.", TC::Dot},
        {";", TC::Semicolon},

        {"store", TC::Store},
        {"restore", TC::Restore},

        {"where", TC::Where},
        {"order by", TC::OrderBy}, {"asc", TC::Asc}, {"dsc", TC::Dsc},

        {"group by", TC::GroupBy},
        {"limit", TC::Limit},

        {"values", TC::Values},

        {"<=", TC::LessEqual},
        {"<", TC::Less},
        {">", TC::Greater},
        {">=", TC::GreaterEqual},
        {"=", TC::Equal},
        {"!=", TC::NotEqual},
        {"<>", TC::NotEqual},

        {"and", TC::AND},
        {"or", TC::OR},
        {"not", TC::NOT}
    };

    this->mapping = mapping_initializer
            | std::views::transform([](const auto &s) -> auto {
                    return std::pair(std::regex(s.first, std::regex::icase | std::regex::optimize), s.second);
                })
            | std::ranges::to<std::vector>();

}

SQLLexer::Token::Category SQLLexer::categorizeLexeme(const std::string_view &lexeme) {
    for (const auto &[regex, category]: mapping) {
        if (std::regex_match(lexeme.begin(), lexeme.end(), regex)) return category;
    }
    return Token::Category::Identifier;
}

std::vector<SQLLexer::Token> SQLLexer::process(const std::string& input) {

    std::regex token_regex(R"(group by|order by|'.+?'|\w+|[<>=]{1,2}|\S)", std::regex::icase | std::regex::optimize);

    auto inputBegin = std::sregex_iterator(input.begin(), input.end(), token_regex);
    auto inputEnd = std::sregex_iterator();

    std::vector<Token> result(std::distance(inputBegin, inputEnd));

    std::ranges::transform(
            inputBegin,
            inputEnd,
            result.begin(),
            [this, &input](const auto &match) -> Token {

                const auto beg = input.begin() + match.position();
                const auto len = match.length();
                const std::string_view lexeme{&(*beg), static_cast<size_t>(len)};
                const auto category = categorizeLexeme(lexeme);

                return {lexeme, category};
            });

    return result;
}