#include "query_builder.hpp"

#include "restore_query.hpp"
#include "store_query.hpp"

std::expected<QueryBuilder::Queries, ParsingException> QueryBuilder::process(const std::string& input) {
    SQLLexer lexer{};
    auto allTokens = lexer.process(input);
    auto tokens = std::span{allTokens};

    Queries queries{};

    static Parser<QueryPtr> parser = createParser();

    while (!tokens.empty()) {
        auto result = parser.parse(tokens);

        if (!result) return std::unexpected(result.error());

        auto& [q, remainingTokens] = result.value();

        queries.emplace_back(std::move(result.value().first));

        tokens = remainingTokens;
    }

    return queries;
}

template<typename TQueryType>
    static auto parse() -> Parser<QueryBuilder::QueryPtr>::Node::Callback {
    using Parser = Parser<QueryBuilder::QueryPtr>;
    using Tokens = Parser::TokenSpan;
    using Subject = Parser::Subject;

    return [parser = TQueryType::createParser()](const Tokens toks, Subject& q) -> std::expected<Tokens, ParsingException> {
        auto result = parser.parse(toks);

        if (!result) return std::unexpected(result.error());

        auto [query, remainingTokens] = result.value();

        q = std::make_unique<TQueryType>(std::move(query));

        return remainingTokens;
    };
}

Parser<QueryBuilder::QueryPtr> QueryBuilder::createParser() {
    using Parser = Parser<QueryPtr>;
    using Category = Parser::Category;

    Parser parser{};

    auto [start, end] = parser.getBoundries();

    auto [createNode] = parser.createNodes<1>({Category::Create});

    const auto selectNode = parser.createNode(Category::Select, parse<SelectQuery>());
    const auto createTableNode = parser.createNode(Category::Table, parse<CreateTableQuery>());
    const auto insertIntoNode = parser.createNode(Category::Insert, parse<InsertIntoQuery>());
    const auto dropTableNode = parser.createNode(Category::Drop, parse<DropTableQuery>());
    const auto truncateTableNode = parser.createNode(Category::Truncate, parse<TruncateTableQuery>());
    const auto storeNode = parser.createNode(Category::Store, parse<StoreQuery>());
    const auto restoreNode = parser.createNode(Category::Restore, parse<RestoreQuery>());

    Parser::connect(start, selectNode);
    Parser::connect(selectNode, end);

    Parser::connect(start, createNode);
    Parser::connect(createNode, createTableNode);
    Parser::connect(createTableNode, end);

    Parser::connect(start, insertIntoNode);
    Parser::connect(insertIntoNode, end);

    Parser::connect(start, dropTableNode);
    Parser::connect(dropTableNode, end);

    Parser::connect(start, truncateTableNode);
    Parser::connect(truncateTableNode, end);

    Parser::connect(start, storeNode);
    Parser::connect(storeNode, end);

    Parser::connect(start, restoreNode);
    Parser::connect(restoreNode, end);

    return parser;
}