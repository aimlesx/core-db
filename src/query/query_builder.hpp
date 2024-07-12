#pragma once
#include <memory>

#include "query_base.hpp"
#include "create_table_query.hpp"
#include "drop_table_query.hpp"
#include "insert_into_query.hpp"
#include "select_query.hpp"
#include "sql_lexer.hpp"
#include "truncate_table_query.hpp"

#include <magic_enum.hpp>

class QueryBuilder {
public:
    using QueryPtr = std::unique_ptr<Query>;
    using Queries = std::vector<QueryPtr>;

    QueryBuilder() = delete;

    static std::expected<Queries, ParsingException> process(const std::string& input);

private:
    static Parser<QueryPtr> createParser();
};
