#pragma once

#include <optional>
#include <expected>

#include "../utils.hpp"
#include "query_base.hpp"
#include "../table/table.hpp"
#include "sql_lexer.hpp"
#include "clause/from_clause.hpp"
#include "clause/limit_clause.hpp"
#include "clause/order_by_clause.hpp"
#include "clause/where_clause.hpp"

#include "parser.hpp"


class SelectQuery final : public Query {
public:
    class ColumnDescriptor {
        std::string table{};
        std::string column{};
        std::string alias{};

    public:
        [[nodiscard]] bool hasTable() const { return !table.empty(); }
        [[nodiscard]] bool hasColumn() const { return !column.empty(); }
        [[nodiscard]] bool hasAlias() const { return !alias.empty(); }

        [[nodiscard]] const std::string& getTable() const { return table; }
        [[nodiscard]] const std::string& getColumn() const { return column; }
        [[nodiscard]] const std::string& getAlias() const { return alias; }

        void setTable(const std::string_view table) { this->table = table; }
        void setColumn(const std::string_view column) { this->column = column; }
        void setAlias(const std::string_view alias) { this->alias = alias; }
    };

    std::expected<Result, ExecutionError> execute(Database& db) override;
    static Parser<SelectQuery> createParser();

private:
    static Parser<std::vector<ColumnDescriptor>> createColumnParser();

    std::vector<ColumnDescriptor> selectedColumns{};
    FromClause from{};
    std::optional<WhereClause> where{};
    std::optional<OrderByClause> orderBy{};
    std::optional<LimitClause> limit{};
};