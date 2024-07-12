#pragma once
#include <string>

#include "query_base.hpp"

#include "../database.hpp"
#include "sql_lexer.hpp"

class DropTableQuery final : public Query {
public:
    DropTableQuery() = default;
    explicit DropTableQuery(const std::string_view tableName)
        : tableName(tableName) {}

    std::expected<Result, ExecutionError> execute(Database& db) override {
        const auto result = db.deleteTable(tableName);

        if (!result) return std::unexpected(result.error());

        return QueryResult{fmt::format("Dropped '{}' table", tableName)};
    }

    static Parser<DropTableQuery> createParser() {
        using Parser = Parser<DropTableQuery>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};
        auto [start, end] = parser.getBoundries();
        //DROP TABLE Person;
        const auto [dropNode, tableNode, semicolon] = parser.createNodes<3>({Category::Drop, Category::Table, Category::Semicolon});

        const auto tableNameNode = parser.createNode(Category::Identifier,
                                                     [](const Tokens toks, Subject& query) {
                                                         query.tableName = toks.front().lexeme;
                                                         return toks.subspan(1);
                                                     });

        Parser::connect(start, dropNode);
        Parser::connect(dropNode, tableNode);
        Parser::connect(tableNode, tableNameNode);
        Parser::connect(tableNameNode, semicolon);
        Parser::connect(semicolon, end);

        return parser;
    }

private:
    std::string tableName{};
};