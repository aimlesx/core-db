#pragma once
#include <magic_enum.hpp>
#include <regex>
#include <ranges>

class SQLLexer {
public:
    struct Token {
        std::string_view lexeme;

        enum class Category {
            None, AnyCategory, // Default
            // General Keywords
            Comma, Dot, As, Into, Identifier, Semicolon, LeftParenthesis, RightParenthesis,
            // DQL
            Select, Asterisk, From, GroupBy, Having,
            // DDL
            Create, Table, Int, String, Bool, Double,

            Insert, Delete, Drop, Truncate,

            Where,
            OrderBy, Asc, Dsc,
            Limit,

            Values,
            // Value Literals
            StringLiteral, IntLiteral, DoubleLiteral, BooleanLiteral,

            // Logical Operators
            AND, OR, NOT,

            // Comparison Operators
            Equal, NotEqual, Greater, GreaterEqual, Less, LessEqual,

            Store, Restore
        } category{};
    };

    explicit SQLLexer();
    std::vector<Token> process(const std::string &input);

private:
    Token::Category categorizeLexeme(const std::string_view& lexeme);

    std::vector<std::pair<std::regex, Token::Category>> mapping;
};