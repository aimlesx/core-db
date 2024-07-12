#pragma once
#include <expected>

#include "query_result.hpp"

class QueryExecutionError;
class Database;

class Query {
public:
    using ExecutionError = QueryExecutionError;
    using Result = QueryResult;

    virtual std::expected<Result, ExecutionError> execute(Database& db) = 0;
    virtual ~Query() = default;
};