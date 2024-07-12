#pragma once

#include <utility>
#include <vector>

#include "../parser.hpp"
#include "../../table/table_field.hpp"

class ValuesClause {
public:
    using FieldPtr = std::shared_ptr<Field>;
    using RowValues = std::vector<FieldPtr>;
    using Values = std::vector<RowValues>;

    ValuesClause() = default;
    explicit ValuesClause(Values vals): values(std::move(vals)) {}

    [[nodiscard]] const Values& getValues() const { return values; }
    [[nodiscard]] static Parser<ValuesClause> createParser();

private:
    [[nodiscard]] static Parser<RowValues> createRowParser();

    Values values{};
};
