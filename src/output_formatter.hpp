#pragma once

#include <memory>
#include <string>
#include <vector>
#include <ranges>

#include <fmt/core.h>
#include <fmt/color.h>

#include "user_settings.hpp"
#include "errors/query_execution_error.hpp"
#include "table/table_field.hpp"
#include "table/table.hpp"
#include "query/query_result.hpp"

class ColoredOutputFormatter {
    class OutputFormatter {
    public:
        [[nodiscard]] static std::vector<size_t> calculateColumnWidths(const std::vector<Table::Row>& rows, const std::vector<Table::ColumnSpecification>& columns);
        [[nodiscard]] static std::string formatColumnName(std::string_view name, size_t width);
        [[nodiscard]] static std::string formatField(const Field& field, size_t width);
        [[nodiscard]] static std::string formatRow(const Table::Row& row, const std::vector<size_t>& widths);
        [[nodiscard]] static std::string formatRows(const std::vector<Table::Row>& rows, const std::vector<size_t>& widths);
    };

    std::shared_ptr<UserSettings> settings;

    [[nodiscard]] bool isColored() const;
    [[nodiscard]] bool isCompact() const;
    [[nodiscard]] static std::string createTableHeaderSeparator(std::vector<size_t> widths);
    [[nodiscard]] std::string formatColumnName(std::string_view name, size_t width) const;
    [[nodiscard]] std::string formatTableHeader(const std::vector<Table::ColumnSpecification>& columns, const std::vector<size_t>& widths) const;
    [[nodiscard]] std::string formatErrorName(std::string_view name) const;

public:
    explicit ColoredOutputFormatter(const std::shared_ptr<UserSettings>& settings): settings(settings) {}

    [[nodiscard]] std::string format(const Table& table) const;
    [[nodiscard]] std::string format(const QueryResult& result) const;
    [[nodiscard]] std::string format(const QueryExecutionError& err) const;

    [[nodiscard]] std::string format(const ParsingException& err) const;
};