#pragma once

#include <utility>
#include <vector>
#include <utility>

#include "../../table/table.hpp"
#include "../parser.hpp"

class OrderByClause {
public:
    enum class Order { Asc, Dsc };

    using Ordering = std::vector<std::pair<std::string, Order>>;

    OrderByClause() = default;
    explicit OrderByClause(Ordering ordering): seq(std::move(ordering)) {}

    [[nodiscard]] const Ordering& getOrdering() const { return seq; }

    std::vector<std::span<Table::Row>> applyOrdering(const Table& table, std::vector<Table::Row>& rows) const;

    static Parser<OrderByClause> createParser();

private:
    Ordering seq{};
};