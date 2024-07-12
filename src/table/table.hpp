#pragma once
#include <optional>
#include <utility>

#include "table_field.hpp"
#include "../query/clause/values_clause.hpp"

class Table {
    friend class ColoredOutputFormatter;
    friend class Serializer; friend class Deserializer;
public:
    using FieldPtr = std::shared_ptr<Field>;
    using Row = std::vector<FieldPtr>;

    struct ColumnSpecification {
        std::string name;
        Field::Type type;
    };

    Table(const std::string_view name, std::vector<ColumnSpecification> columns)
        : name(name), columns(std::move(columns)) {}

    Table(Table& source, std::vector<std::string_view> columns, std::vector<std::pair<std::string_view, bool>> sortOrder, auto recordPredicate) {
        const auto& sourceRows = source.rows;

        this->name = "SELECT RESULT";
        this->columns = source.columns;
        this->rows = sourceRows | std::views::filter(recordPredicate) | std::ranges::to<std::vector>();
    }

    [[nodiscard]] size_t getColumnCount() const { return rows.empty() ? 0 : rows.front().size(); }
    [[nodiscard]] size_t getRowCount() const { return rows.size(); }
    [[nodiscard]] const std::string& getName() const noexcept { return this->name; }
    void clear() { rows.clear(); }

    [[nodiscard]] bool checkTypes(const Row& row) const;
    void insert(const std::vector<Row>& values);

    [[nodiscard]] std::optional<size_t> getColumnIndex(std::string_view columnName) const;

    [[nodiscard]] const std::vector<Row>& getRows() const { return rows; }
    [[nodiscard]] const std::vector<ColumnSpecification>& getColumns() const { return columns; }

private:
    std::string name{};
    std::vector<Row> rows{};
    std::vector<ColumnSpecification> columns{};
};