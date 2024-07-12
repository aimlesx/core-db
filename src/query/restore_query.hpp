#pragma once

#pragma once
#include <string>

#include "query_base.hpp"

#include "../database.hpp"
#include "sql_lexer.hpp"

class RestoreQuery final : public Query {
public:
    RestoreQuery() = default;
    explicit RestoreQuery(const std::string_view tableName)
        : tableName(tableName) {}

    std::expected<Result, ExecutionError> execute(Database& db) override {
        auto result = db.restoreTable(tableName);

        if (!result) return std::unexpected(result.error());

        return QueryResult{fmt::format("Restored '{}' table", tableName)};
    }

    static Parser<RestoreQuery> createParser() {
        using Parser = Parser<RestoreQuery>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};
        auto [start, end] = parser.getBoundries();
        //DROP TABLE Person;
        const auto [restoreNode, semicolon] = parser.createNodes<2>({Category::Restore, Category::Semicolon});

        const auto tableNameNode = parser.createNode(Category::Identifier,
                                                     [](const Tokens toks, Subject& query) {
                                                         query.tableName = toks.front().lexeme;
                                                         return toks.subspan(1);
                                                     });

        Parser::connect(start, restoreNode);
        Parser::connect(restoreNode, tableNameNode);
        Parser::connect(tableNameNode, semicolon);
        Parser::connect(semicolon, end);

        return parser;
    }

private:
    std::string tableName{};
};