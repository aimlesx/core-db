#include "table.hpp"

bool Table::checkTypes(const Row& row) const {
    return std::ranges::all_of(row, [this, &row](const FieldPtr& fieldPtr) {
        return fieldPtr->getTypeIndex() == static_cast<size_t>(columns[&fieldPtr - &row[0]].type);
    });
}

void Table::insert(const std::vector<Row>& values) {
    if (!std::ranges::all_of(values, [this](const auto& row) { return checkTypes(row); })) {
        // TODO
        fmt::println("TYPE ERROR");
        return;
    }

    for (auto& row : values) {
        rows.emplace_back(row);
    }
}

std::optional<size_t> Table::getColumnIndex(const std::string_view columnName) const {
    const auto it = std::ranges::find(columns, columnName, &ColumnSpecification::name);

    if (it == columns.end()) return std::nullopt;

    return std::distance(columns.begin(), it);
}