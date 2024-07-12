#pragma once

#pragma once
#include <string>

#include "query_base.hpp"

#include "../database.hpp"
#include "sql_lexer.hpp"

class StoreQuery final : public Query {
public:
    StoreQuery() = default;
    explicit StoreQuery(const std::string_view tableName)
        : tableName(tableName) {}

    std::expected<Result, ExecutionError> execute(Database& db) override {
        auto result = db.storeTable(tableName);

        if (!result) return std::unexpected(result.error());

        return QueryResult{fmt::format("Stored '{}' table", tableName)};
    }

    static Parser<StoreQuery> createParser() {
        using Parser = Parser<StoreQuery>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};
        auto [start, end] = parser.getBoundries();
        //DROP TABLE Person;
        const auto [storeNode, semicolon] = parser.createNodes<2>({Category::Store, Category::Semicolon});

        const auto tableNameNode = parser.createNode(Category::Identifier,
                                                     [](const Tokens toks, Subject& query) {
                                                         query.tableName = toks.front().lexeme;
                                                         return toks.subspan(1);
                                                     });

        Parser::connect(start, storeNode);
        Parser::connect(storeNode, tableNameNode);
        Parser::connect(tableNameNode, semicolon);
        Parser::connect(semicolon, end);

        return parser;
    }

private:
    std::string tableName{};
};