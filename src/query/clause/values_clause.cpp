#include "values_clause.hpp"

#include <set>
#include <string_view>

Parser<ValuesClause> ValuesClause::createParser() {
    using Parser = Parser<ValuesClause>;
    using Tokens = Parser::TokenSpan;
    using Category = Parser::Category;
    using Subject = Parser::Subject;

    Parser parser{};
    auto [start, end] = parser.getBoundries();

    const auto valuesNode = parser.createNode(Category::Values);

    const auto rowNode = parser.createLoopedNode(Category::LeftParenthesis,
        [rowParser = createRowParser()](const Tokens toks, Subject& clause) -> std::expected<Tokens, Parser::Exception> {
            auto row = rowParser.parse(toks);
            if (!row) return std::unexpected(row.error());

            clause.values.emplace_back(row.value().first);

            return row.value().second;
        }, Category::Comma);

    Parser::connect(start, valuesNode);
    Parser::connect(valuesNode, rowNode);
    Parser::connect(rowNode, end);

    return parser;
}

Parser<ValuesClause::RowValues> ValuesClause::createRowParser() {
    using Parser = Parser<RowValues>;
    using Tokens = Parser::TokenSpan;
    using Category = Parser::Category;
    using Subject = Parser::Subject;

    Parser parser{};
    auto [start, end] = parser.getBoundries();

    auto [leftParenthesis, rightParenthesis] = parser.createNodes<2>({Category::LeftParenthesis, Category::RightParenthesis});

    const auto literalNode = parser.createLoopedNode(
        Category::AnyCategory,
        [](const Tokens toks, Subject& row) -> std::expected<Tokens, Parser::Exception> {

            if (!std::set{Category::BooleanLiteral, Category::DoubleLiteral, Category::IntLiteral,
                Category::StringLiteral }.contains(toks.front().category)) {

            return std::unexpected(Parser::Exception(fmt::format("Provided token was not a value literal.")));
        }

        row.emplace_back(std::make_shared<Field>(Field::createFromToken(toks.front())));

        return toks.subspan(1);
    }, Category::Comma);

    Parser::connect(start, leftParenthesis);
    Parser::connect(leftParenthesis, literalNode);
    Parser::connect(literalNode, rightParenthesis);
    Parser::connect(rightParenthesis, end);

    return parser;
}
