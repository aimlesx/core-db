#pragma once
#include <optional>
#include <string>

#include "../table/table.hpp"

class QueryResult {
    friend class ColoredOutputFormatter;

    std::optional<Table> table{};
    std::optional<std::string> additionalInfo{};

public:
    explicit QueryResult(Table table)
        : table(std::make_optional(table)) {}

    explicit QueryResult(std::string info)
        : additionalInfo(std::make_optional(info)) {}

    explicit QueryResult(Table table, std::string info)
        : table(std::make_optional(table)), additionalInfo(std::make_optional(info)) {}

    auto getInfo() { return this->additionalInfo; }
};