#pragma once
#include <string>
#include <vector>

#include "query_base.hpp"

#include "../database.hpp"

class CreateTableQuery final : public Query {
public:
    CreateTableQuery() = default;
    CreateTableQuery(const std::string_view tableName, std::vector<Table::ColumnSpecification> columnSpecs)
        : tableName(tableName), columns(std::move(columnSpecs)) {}

    std::expected<Result, ExecutionError> execute(Database& db) override;
    static Parser<CreateTableQuery> createParser();

private:
    std::string tableName{};
    std::vector<Table::ColumnSpecification> columns{};
};