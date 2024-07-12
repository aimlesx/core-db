#pragma once

#include "../../database.hpp"
#include "../parser.hpp"

class FromClause {
public:
    FromClause() = default;
    explicit FromClause(const std::string_view source): source(source) {}

    std::expected<std::reference_wrapper<Table>, QueryExecutionError> get(Database& db) const {
        auto result = db.getTable(source);
        if (!result) return std::unexpected(result.error());

        return result.value();
    }

    static Parser<FromClause> createParser() {
        using Parser = Parser<FromClause>;
        using Category = Parser::Category;
        using Tokens = Parser::TokenSpan;
        using Subject = Parser::Subject;

        Parser parser{};

        auto [start, end] = parser.getBoundries();

        const auto fromNode = parser.createNode(Category::From);
        const auto idNode = parser.createNode(Category::Identifier, [](const Tokens toks, Subject& clause) {
            clause.source = toks.front().lexeme;
            return toks.subspan(1);
        });

        Parser::connect(start, fromNode);
        Parser::connect(fromNode, idNode);
        Parser::connect(idNode, end);

        return parser;
    }

private:
    std::string source{};
};
