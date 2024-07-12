#include "output_formatter.hpp"

std::vector<size_t> ColoredOutputFormatter::OutputFormatter::calculateColumnWidths(const std::vector<Table::Row>& rows,
    const std::vector<Table::ColumnSpecification>& columns) {

    return std::views::iota(0, static_cast<ptrdiff_t>(columns.size()))
            | std::views::transform([&rows, &columns](auto columnIndex) {
                const auto rowToColumnWidth = [columnIndex] (const Table::Row& row) {
                    return row[columnIndex]->toString().size();
                };

                const size_t columnNameSize = columns[columnIndex].name.size();
                size_t widestFieldSize = 0;

                if (!rows.empty()) widestFieldSize = rowToColumnWidth(std::ranges::max(rows, {}, rowToColumnWidth));

                return std::max(columnNameSize, widestFieldSize);
            })
            | std::ranges::to<std::vector>();

}

std::string ColoredOutputFormatter::OutputFormatter::formatColumnName(const std::string_view name, const size_t width) {
    return fmt::format("{:{}}", name, width);
}

std::string ColoredOutputFormatter::OutputFormatter::formatField(const Field& field, size_t width) {
    return fmt::format("{:{}}", field.toString(), width);
}

std::string ColoredOutputFormatter::OutputFormatter::
    formatRow(const Table::Row& row, const std::vector<size_t>& widths) {
    {
        using namespace std::literals;

        const auto fieldFormatter = [&widths] (const auto& pair) -> std::string {
            auto [index, field] = pair;
            return formatField(field, widths[index]);
        };

        const std::string formattedRow = fmt::format("| {} |",
            row | std::views::transform(&Table::FieldPtr::operator*<>)
                | std::views::enumerate
                | std::views::transform(fieldFormatter)
                | std::views::join_with(" | "sv)
                | std::ranges::to<std::string>()
                );

        return formattedRow;
    }
}

std::string ColoredOutputFormatter::OutputFormatter::formatRows(const std::vector<Table::Row>& rows,
    const std::vector<size_t>& widths) {
    auto rowFormatter = [&widths](const Table::Row& row) -> std::string {
        return formatRow(row, widths);
    };

    return rows | std::views::transform(rowFormatter)
        | std::views::join_with('\n')
        | std::ranges::to<std::string>();
}

bool ColoredOutputFormatter::isColored() const {
    return settings->get<bool>("colors").value_or(false);
}

bool ColoredOutputFormatter::isCompact() const {
    return settings->get<bool>("output.compact").value_or(false);
}

std::string ColoredOutputFormatter::createTableHeaderSeparator(std::vector<size_t> widths) {
    using namespace std::literals;

    return fmt::format("| {} |",
        widths | std::views::transform([](const size_t width) { return std::string(width, '-'); })
            | std::views::join_with(" + "sv)
            | std::ranges::to<std::string>());
}

std::string ColoredOutputFormatter::formatColumnName(const std::string_view name, const size_t width) const {
    auto formattedName = OutputFormatter::formatColumnName(name, width);

    if (!isColored()) return formattedName;

    return fmt::format(fmt::fg(fmt::color::cyan), "{}", formattedName);
}

std::string ColoredOutputFormatter::formatTableHeader(const std::vector<Table::ColumnSpecification>& columns,
const std::vector<size_t>& widths) const  {
    using namespace std::literals;

    auto columnNameFormatter = [this, &widths](const auto& pair) -> std::string {
        auto [columnIndex, columnName] = pair;
        return formatColumnName(columnName, widths[columnIndex]);
    };

    auto columnNames = fmt::format("| {} |", columns | std::views::transform(&Table::ColumnSpecification::name)
        | std::views::enumerate
        | std::views::transform(columnNameFormatter)
        | std::views::join_with(" | "sv)
        | std::ranges::to<std::string>());

    if (isColored()) return columnNames;

    return fmt::format("{}\n{}", columnNames, createTableHeaderSeparator(widths));
}

std::string ColoredOutputFormatter::formatErrorName(const std::string_view name) const {
    if (isColored()) return fmt::format(fmt::fg(fmt::color::red), "{}", name);

    return std::string(name);
}

std::string ColoredOutputFormatter::format(const Table& table) const {
    using namespace std::literals;

    const auto& columns = table.columns;
    const auto& rows = table.rows;

    const auto columnWidths = OutputFormatter::calculateColumnWidths(rows, columns);

    auto formattedHeader = formatTableHeader(columns, columnWidths);
    if (rows.empty()) {
        return fmt::format("{}", formattedHeader);
    }

    auto formattedRows = OutputFormatter::formatRows(rows, columnWidths);

    return fmt::format("{}\n{}", formattedHeader, formattedRows);
}

std::string ColoredOutputFormatter::format(const QueryResult& result) const {
    static const std::string coloredOk = fmt::format(fmt::fg(fmt::color::spring_green), "OK");

    std::string formatted{fmt::format("[{}] ", isColored() ? coloredOk : "OK")};

    if (result.additionalInfo) {
        formatted.append(result.additionalInfo.value());
    }

    if (result.table) {
        formatted.append("\n\n");

        const Table& table = result.table.value();
        formatted.append(format(table));
    }

    if (!isCompact()) formatted.append("\n");

    return formatted;
}

std::string ColoredOutputFormatter::format(const QueryExecutionError& err) const {
    return fmt::format("[{}] {}{}",
        formatErrorName("Execution Error"),
        err.getReason(),
        isCompact() ? ' ' : '\n');
}

std::string ColoredOutputFormatter::format(const ParsingException& err) const {
    return fmt::format("[{}] {}{}",
        formatErrorName("Parsing Error"),
        err.getMessage(),
        isCompact() ? ' ' : '\n');
}