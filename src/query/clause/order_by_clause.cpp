#include "order_by_clause.hpp"

std::vector<std::span<Table::Row>> OrderByClause::applyOrdering(const Table& table, std::vector<Table::Row>& rows) const {
    const auto& ordering = seq;

    std::vector spans{std::span(rows)};

    for (const auto& [columnName, columnOrder] : ordering) {

        auto optIndex = table.getColumnIndex(columnName);
        // if (!optIndex) return std::unexpected(ExecutionError("Column doesn't exist")); // TODO

        size_t columnIndex = optIndex.value();
        const auto projector = [columnIndex] (const Table::Row& row) -> auto& { return row[columnIndex]; };

        const bool isAsc = columnOrder == OrderByClause::Order::Asc;
        auto targetOrdering = isAsc ? std::partial_ordering::less : std::partial_ordering::greater;

        const auto predicate = [targetOrdering](const Table::FieldPtr& left, const Table::FieldPtr& right) -> bool {
            return *left <=> *right == targetOrdering;
        };

        std::ranges::for_each(spans, [&predicate, &projector] (const auto& span) {
            std::ranges::sort(span, predicate, projector);
        });

        spans = spans | std::views::transform([&projector] (const std::span<Table::Row>& span){
            std::vector<std::span<Table::Row>> subspans{};

            for (auto it = span.begin(); it != span.end();) {
                const auto sameAsFirst =
                    [&firstElement = projector(*it)] (const Table::FieldPtr& fieldPtr) {
                        return *firstElement == *fieldPtr;
                    };

                auto endIt = std::ranges::find_if_not(it, span.end(), sameAsFirst, projector);
                subspans.emplace_back(it, endIt);
                it = endIt;
            }

            return subspans;
        }) | std::views::join | std::ranges::to<std::vector>();
    }

    return spans;
}

Parser<OrderByClause> OrderByClause::createParser() {
    using Parser = Parser<OrderByClause>;
    using Tokens = Parser::TokenSpan;
    using Category = Parser::Category;
    using Subject = Parser::Subject;

    Parser parser{};

    auto [start, end] = parser.getBoundries();

    const auto orderByNode = parser.createNode(Category::OrderBy);
    const auto orderNode = parser.createLoopedNode(Category::Identifier,
        [](const Tokens toks, Subject& clause) {

            const auto id = toks.front().lexeme;
            auto ord = Order::Asc;

            bool wasExtended = false;
            const bool enoughTokens = toks.size() >= 2;

            if (enoughTokens && (toks[1].category == Category::Dsc || toks[1].category == Category::Asc)) {
                wasExtended = true;
                ord = toks[1].category == Category::Dsc ? Order::Dsc : Order::Asc;
            }

            clause.seq.emplace_back(id, ord);
        return toks.subspan(wasExtended ? 2 : 1);
    }, Category::Comma);

    Parser::connect(start, orderByNode);
    Parser::connect(orderByNode, orderNode);
    Parser::connect(orderNode, end);


    return parser;
}