#pragma once
#include <string>

#include "query_base.hpp"

#include "../database.hpp"
#include "sql_lexer.hpp"
#include "clause/values_clause.hpp"

class InsertIntoQuery final : public Query {
    using Category = SQLLexer::Token::Category;

    std::string tableName{};
    ValuesClause values{};

public:
    InsertIntoQuery() = default;
    explicit InsertIntoQuery(const std::string_view tableName)
        : tableName(tableName) {}

    std::expected<Result, ExecutionError> execute(Database& db) override {
        auto possiblyTable = db.getTable(tableName);
        if (!possiblyTable) return std::unexpected(possiblyTable.error());

        possiblyTable.value().get().insert(values.getValues());
        return QueryResult{fmt::format("Inserted {} records", values.getValues().size())};
    }

    static Parser<InsertIntoQuery> createParser() {
        using Parser = Parser<InsertIntoQuery>;
        using Tokens = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        Parser parser{};
        auto [start, end] = parser.getBoundries();
        auto [insertNode, intoNode, semicolon] = parser.createNodes<3>({Category::Insert, Category::Into, Category::Semicolon});

        const auto tableNameNode = parser.createNode(Category::Identifier,
                                                     [](const Tokens toks, Subject& query) {
                                                         query.tableName = toks.front().lexeme;
                                                         return toks.subspan(1);
                                                     });

        const auto valuesNode = parser.createNode(Category::Values,
                                                  [parser = ValuesClause::createParser()](const Tokens toks, Subject& query) -> std::expected<Tokens, ParsingException> {
                                                      auto result = parser.parse(toks);

                                                      if (!result) return std::unexpected(result.error());

                                                      query.values = std::move(result.value().first);
                                                      return result.value().second;
                                                  });

        Parser::connect(start, insertNode);
        Parser::connect(insertNode, intoNode);
        Parser::connect(intoNode, tableNameNode);
        Parser::connect(tableNameNode, valuesNode);
        Parser::connect(valuesNode, semicolon);
        Parser::connect(semicolon, end);


        return parser;
    }
};
