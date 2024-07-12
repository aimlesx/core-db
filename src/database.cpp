#include "database.hpp"

std::expected<std::reference_wrapper<Table>, QueryExecutionError> Database::getTable(std::string_view tableName) {

    if (!hasTable(tableName))
        return std::unexpected(
            QueryExecutionError(fmt::format("'{}' table doesn't exist", tableName)));

    return tables.at(std::string(tableName));
}

std::expected<void, QueryExecutionError> Database::createTable(
    const std::string_view tableName,
    const std::vector<Table::ColumnSpecification>& columns)
{
    if (hasTable(tableName))
        return std::unexpected(QueryExecutionError(fmt::format("'{}' table already exists", tableName)));

    tables.emplace(tableName, Table{tableName, columns});
    return {};
}

std::expected<void, QueryExecutionError> Database::deleteTable(const std::string_view tableName) {
    if (!hasTable(tableName))
        return std::unexpected(
            QueryExecutionError(fmt::format("'{}' table doesn't exist!", tableName)));

    tables.erase(tableName);
    return {};
}

std::expected<size_t, QueryExecutionError> Database::truncateTable(const std::string_view tableName) {
    if (!hasTable(tableName))
        return std::unexpected(
            QueryExecutionError(fmt::format("'{}' table doesn't exist", tableName)));

    auto result = getTable(tableName);
    if (!result) return std::unexpected(result.error());

    Table& table = result.value();

    size_t recordCount = table.getRowCount();
    table.clear();

    return recordCount;
}

bool Database::hasTable(const std::string_view tableName) const {
    return tables.contains(tableName);
}
