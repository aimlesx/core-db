#pragma once

#include <charconv>

#include "../parser.hpp"

class LimitClause {
public:
    explicit LimitClause(const size_t count): count(count) {}

    static Parser<LimitClause> createParser() {
        using Parser = Parser<LimitClause>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};

        auto [start, end] = parser.getBoundries();

        const auto limitNode = parser.createNode(Category::Limit);
        const auto countNode = parser.createNode(Category::IntLiteral,
            [](const Tokens toks, Subject& clause) {
                const auto literal = toks.front().lexeme;

                const auto beg = literal.data();
                const auto len = literal.size();

                if (std::from_chars(beg, beg + len, clause.count).ec != std::errc()) clause.count = UINT64_MAX;

                return toks.subspan(1);
            });

        Parser::connect(start, limitNode);
        Parser::connect(limitNode, countNode);
        Parser::connect(countNode, end);

        return parser;
    }

private:
    size_t count{};
};
