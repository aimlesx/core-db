#include "create_table_query.hpp"
#include "sql_lexer.hpp"

std::expected<Query::Result, Query::ExecutionError> CreateTableQuery::execute(Database& db) {
    const auto result = db.createTable(std::string(tableName), columns);

    if (!result) return std::unexpected(result.error());

    return QueryResult{fmt::format("Created '{}' table", tableName)};
}

using Category = SQLLexer::Token::Category;
Field::Type cat2type(const Category cat) {
    switch (cat) {
        case Category::Int: return Field::Type::Int;
        case Category::Bool: return Field::Type::Boolean;
        case Category::Double: return Field::Type::Double;
        default: return Field::Type::String;
    }
}

Parser<CreateTableQuery> CreateTableQuery::createParser() {
    using Parser = Parser<CreateTableQuery>;
    using Tokens = Parser::TokenSpan;
    using Category = Parser::Category;
    using Subject = Parser::Subject;

    Parser parser{};

    auto [start, end] = parser.getBoundries();

    const auto [createNode, tableNode, leftParenthesis, rightParenthesis, semicolon] =
        parser.createNodes<5>({Category::Create, Category::Table, Category::LeftParenthesis, Category::RightParenthesis, Category::Semicolon});

    const auto tableNameNode = parser.createNode(Category::Identifier,
        [](const Tokens toks, Subject& query) {
            query.tableName = toks.front().lexeme;
            return toks.subspan(1);
    });

    const auto specNode = parser.createLoopedNode(Category::Identifier,
        [](const Tokens toks, Subject& query) {
            const auto id = toks.front().lexeme;
            const auto type = cat2type(toks[1].category);

            query.columns.emplace_back(std::string(id), type);

            return toks.subspan(2);
        },
        Category::Comma);

    // Parser::connect(start, createNode);
    // Parser::connect(createNode, tableNode); TODO: bugged
    Parser::connect(start, tableNode);
    Parser::connect(tableNode, tableNameNode);
    Parser::connect(tableNameNode, leftParenthesis);
    Parser::connect(leftParenthesis, specNode);
    Parser::connect(specNode, rightParenthesis);
    Parser::connect(rightParenthesis, semicolon);
    Parser::connect(semicolon, end);

    return parser;
}
