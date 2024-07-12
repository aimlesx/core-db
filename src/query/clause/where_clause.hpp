#pragma once

#include <expected>
#include <functional>

// #include "../errors/query_execution_error.hpp"
// #include "../table/table.hpp"

//void whereParsing() {
//    const std::string expr = "age >= 18 AND location = 'Warsaw' OR age < 18";
//
//    SQLLexer lexer{};
//    auto tokens = lexer.process(expr);
//
//    for (const auto& t : tokens) {
//        fmt::println("{} {}", magic_enum::enum_name(t.category), t.lexeme);
//    }
//    fmt::println("{}", WhereClause::findSplitIndex(tokens));
//}

class WhereClause {
    using Row = Table::Row;
    using RowPredicate = std::function<bool(const Row&)>;
    using Token = SQLLexer::Token;
    using Category = Token::Category;

    static constexpr std::string_view errorTemplate = R"(No column named "{}" in "{}" table)";

public:
    explicit WhereClause(/*SQLScanner& in*/) {
        using Category = SQLLexer::Token::Category;

        // in.assertEquals(Category::Where).skip();

        // auto compileExpression = [] (const Token& lhs, const Token& op, const Token& rhs) -> RowPredicate {
        //     auto leftAccessor = createAccessor(table, lhs);
        //     auto rightAccessor = createAccessor(table, rhs);
        //
        //     if (!leftAccessor || !rightAccessor)
        //         return std::unexpected<QueryExecutionError>(
        //                 fmt::format(errorTemplate, !leftAccessor ? lhs.lexeme : rhs.lexeme, table.getName())
        //         );
        //
        //     std::strong_ordering expectedOrdering{};
        //
        //     if (op.category == Category::Equal || op.category == Category::NotEqual) {
        //         expectedOrdering = std::strong_ordering::equal;
        //     } else if (op.category == Category::Greater || op.category == Category::LessEqual) {
        //         expectedOrdering = std::strong_ordering::greater;
        //     } else if (op.category == Category::Less || op.category == Category::GreaterEqual) {
        //         expectedOrdering = std::strong_ordering::less;
        //     }
        //
        //     bool negation = op.category == Category::NotEqual
        //                     || op.category == Category::LessEqual
        //                     || op.category == Category::GreaterEqual;
        //
        //     return [la = *leftAccessor,
        //             ra = *rightAccessor,
        //             expectedOrdering, negation] (const Row& row) -> bool {
        //
        //         const auto actualOrdering = la(row) <=> ra(row);
        //
        //         return negation ?
        //                 actualOrdering != expectedOrdering
        //                 : actualOrdering == expectedOrdering;
        //     };
        // };
        //
        // predCompiler = [] (const Table& table) {
        //     return [] (const Row& row) { return true; };
        // };

        // while(in.isOneOf({
        //     Category::Identifier,
        //     Category::StringLiteral, Category::IntLiteral, Category::DoubleLiteral, Category::BooleanLiteral,
        //     Category::AND, Category::OR, Category::NOT,
        //     Category::Less, Category::LessEqual,
        //     Category::Greater, Category::GreaterEqual,
        //     Category::Equal, Category::NotEqual,
        //     Category::LeftParenthesis, Category::RightParenthesis
        // })) in.skip();
    }

    // static std::optional<std::function<const Field&(Row&)>> createAccessor(const Table& table, const Token& t) {
    //     if (t.category != Category::Identifier) {
    //         return [f = Field::createFromToken(t)](Row&) -> const Field& { return f; };
    //     }
    //
    //     return table.getColumnAccessor(t.lexeme);
    // }

    [[nodiscard]] std::expected<RowPredicate, QueryExecutionError> compile(const Table& table) const {
        return [](const Row& row) { return true; };
        // return predCompiler(table);
    }

private:
    // std::function<RowPredicate(const Table&)> predCompiler;

};