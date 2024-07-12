#pragma once
#include <string>

#include "query_base.hpp"

#include "../database.hpp"
#include "sql_lexer.hpp"

class TruncateTableQuery final : public Query {
    using Category = SQLLexer::Token::Category;

    std::string tableName{};

public:
    TruncateTableQuery() = default;
    explicit TruncateTableQuery(const std::string_view tableName)
        : tableName(tableName) {}

    std::expected<Result, ExecutionError> execute(Database& db) override {
        auto result = db.truncateTable(tableName);

        if (!result) return std::unexpected(result.error());

        return QueryResult{fmt::format("Truncated {} records", result.value())};
    }

    static Parser<TruncateTableQuery> createParser() {
        using Parser = Parser<TruncateTableQuery>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};
        auto [start, end] = parser.getBoundries();
        //DROP TABLE Person;
        const auto [truncateNode, tableNode, semicolon] = parser.createNodes<3>({Category::Truncate, Category::Table, Category::Semicolon});

        const auto tableNameNode = parser.createNode(Category::Identifier,
                                                     [](const Tokens toks, Subject& query) {
                                                         query.tableName = toks.front().lexeme;
                                                         return toks.subspan(1);
                                                     });

        Parser::connect(start, truncateNode);
        Parser::connect(truncateNode, tableNode);
        Parser::connect(tableNode, tableNameNode);
        Parser::connect(tableNameNode, semicolon);
        Parser::connect(semicolon, end);

        return parser;
    }
};