#include "select_query.hpp"

std::expected<Query::Result, Query::ExecutionError> SelectQuery::execute(Database& db)  {
    const ScopedTimer timer{};

    auto tableInQuestion = from.get(db);
    if (!tableInQuestion) return std::unexpected(tableInQuestion.error());

    const Table& source = tableInQuestion.value().get();

    std::vector<Table::Row> rows{};

    if (where) {
        const auto compileResult = where->compile(source);

        if (!compileResult) return std::unexpected(compileResult.error());

        rows.resize(source.getRows().size());

        std::ranges::copy_if(source.getRows(), rows.begin(), compileResult.value());
    } else {
        rows = source.getRows();
    }

    if (orderBy) {
        orderBy->applyOrdering(source, rows);
    }

    auto selectedIndexes = selectedColumns
        | std::views::transform([&table = source](const ColumnDescriptor& descriptor) {
                return table.getColumnIndex(descriptor.getColumn());
            })
        | std::views::filter(&std::optional<size_t>::has_value) // Take only valid column names
        | std::views::transform([] (const auto& opt) { return opt.value(); })
        | std::ranges::to<std::vector>();

    auto sourceColumns = source.getColumns();
    auto columnDefinitions = selectedIndexes
        | std::views::transform([sourceColumns](const size_t index) {
            return sourceColumns[index];
        })
        | std::ranges::to<std::vector>();

    rows = rows
        | std::views::transform([&selectedIndexes](const Table::Row& row) {
            return selectedIndexes
                | std::views::transform([row](const size_t index) { return row[index]; })
                | std::ranges::to<std::vector>();
        })
        | std::ranges::to<std::vector>();

    Table table{"QUERY", std::move(columnDefinitions)};
    table.insert(rows);

    return QueryResult{std::move(table), fmt::format("Queried {} rows in {} ms", rows.size(), timer.get())};
}

Parser<SelectQuery> SelectQuery::createParser() {
    using Parser = Parser<SelectQuery>;
    using TokenSpan = Parser::TokenSpan;
    using Category = Parser::Category;
    using Subject = Parser::Subject;

    Parser parser;
    auto [start, end] = parser.getBoundries();

    const auto [selectNode, asteriskNode, semicolon] =
        parser.createNodes<3>({Category::Select, Category::Asterisk, Category::Semicolon});

    const auto columnNode = parser.createNode(
        Category::Identifier,
        [columnParser = createColumnParser()](const TokenSpan tokens, Subject& query) -> std::expected<TokenSpan, Parser::Exception> {
            auto result = columnParser.parse(tokens);

            if (!result) throw std::runtime_error("nah not again");

            auto& [descriptors, remainingTokens] = result.value();
            query.selectedColumns = descriptors;

            return remainingTokens;
        });

    const auto fromNode = parser.createNode(
        Category::From,
        [parser = FromClause::createParser()](const TokenSpan tokens, Subject& query) -> std::expected<TokenSpan, Parser::Exception> {
            const auto res = parser.parse(tokens);

            if (!res) return std::unexpected(res.error());

            auto [clause, remainingTokens] = res.value();

            query.from = clause;

            return remainingTokens;
        });

    const auto orderByNode = parser.createNode(Category::OrderBy,
        [parser = OrderByClause::createParser()](const TokenSpan toks, Subject& query)
            -> std::expected<TokenSpan, Parser::Exception> {

            auto res = parser.parse(toks);
            if (!res) return std::unexpected(res.error());

            auto [clause, remainingTokens] = res.value();

            query.orderBy = clause;

            return remainingTokens;
        });

    Parser::connect(start, selectNode);
    Parser::connect(selectNode, {asteriskNode, columnNode});
    Parser::connect(asteriskNode, fromNode);
    Parser::connect(columnNode, fromNode);
    Parser::connect(fromNode, {orderByNode, semicolon});

    Parser::connect(orderByNode, semicolon);

    Parser::connect(semicolon, end);

    return parser;
}

Parser<std::vector<SelectQuery::ColumnDescriptor>> SelectQuery::createColumnParser() {
    using SingleColumnParser = Parser<ColumnDescriptor>;
    using ColumnParser = Parser<std::vector<ColumnDescriptor>>;

    SingleColumnParser scp;
    {
        using Parser = SingleColumnParser;
        using TokenSpan = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        auto [start, end] = scp.getBoundries();

        const auto [asNode, dotNode] = scp.createNodes<2>({Category::As, Category::Dot});

        const auto firstSelectorNode = scp.createNode(Category::Identifier,
                                                         [](const TokenSpan& t, Subject& descriptor) {
                                                             const auto& selector = t.front().lexeme;

                                                             // It is table selector
                                                             if (t.size() >= 2 && t[1].category == Category::Dot) {
                                                                descriptor.setTable(selector);
                                                             } else {
                                                                 descriptor.setColumn(selector);
                                                             }

                                                             return t.subspan(1);
                                                         });

        const auto secondSelectorNode = scp.createNode(Category::Identifier,
                                                          [](const TokenSpan& t, Subject& descriptor) {
                                                              const auto& selector = t.front().lexeme;
                                                              descriptor.setColumn(selector);

                                                              return t.subspan(1);
                                                          });

        const auto aliasNode = scp.createNode(Category::Identifier,
                                                 [](const TokenSpan& t, Subject& descriptor) {
                                                     const auto& alias = t.front().lexeme;
                                                     descriptor.setAlias(alias);

                                                     return t.subspan(1);
                                                 });

        Parser::connect(start, firstSelectorNode);
        Parser::connect(firstSelectorNode, {asNode, dotNode, end});
        Parser::connect(dotNode, secondSelectorNode);
        Parser::connect(secondSelectorNode, {asNode, end});
        Parser::connect(asNode, aliasNode);
        Parser::connect(aliasNode, end);
    }

    ColumnParser parser;
    {
        using Parser = ColumnParser;
        using TokenSpan = Parser::TokenSpan;
        using Category = Parser::Category;
        using Subject = Parser::Subject;

        auto [start, end] = parser.getBoundries();

        const auto columnNode = parser.createLoopedNode(
            Category::Identifier,
            [scp](const TokenSpan tokens, Subject& subject) {
                auto result = scp.parse(tokens);

                if (!result) throw std::runtime_error("nah not again");

                auto& [descriptor, remainingTokens] = result.value();

                subject.emplace_back(descriptor);

                return remainingTokens;
            }, Category::Comma);

        Parser::connect(start, columnNode);
        Parser::connect(columnNode, end);
    }

    return parser;
}