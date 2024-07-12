#pragma once
#include <expected>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <ranges>

#include <fmt/core.h>

#include "deserializer.hpp"
#include "serializer.hpp"
#include "errors/query_execution_error.hpp"
#include "table/table.hpp"
#include "utils.hpp"

class Database {
    std::string name;
    std::unordered_map<std::string, Table, sv_hash, std::equal_to<>> tables;

    inline static std::string dbFolderPrefix = "data\\";
    inline static std::string dbTableFileExtension = ".table";

public:
    explicit Database(std::string name) : name(std::move(name)) {
        namespace fs = std::filesystem;

        const fs::path folderPath(dbFolderPrefix + this->name);

        // if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        //     throw std::runtime_error(fmt::format("\"{}\" database doesn't exist!", this->name));
        // }

        // auto tableExtensionCheck = [](const auto& e) {
        //     return e.path().extension() == dbTableFileExtension;
        // };

        // tables = fs::directory_iterator(folderPath)
        //          | std::views::filter(tableExtensionCheck)
        //          | std::views::transform(&fs::directory_entry::path)
        //          | std::views::transform(Table::load)
        //          | std::views::as_rvalue
        //          | std::views::transform([] (auto&& t) -> std::pair<std::string, Table> { return {t.getName(), t}; })
        //          | std::views::as_rvalue
        //          | std::ranges::to<std::unordered_map>();
    }

    // Tables
    std::expected<std::reference_wrapper<Table>, QueryExecutionError> getTable(std::string_view tableName);
    std::expected<void, QueryExecutionError> createTable(std::string_view tableName, const std::vector<Table::ColumnSpecification>& columns);
    std::expected<void, QueryExecutionError> deleteTable(std::string_view tableName);
    std::expected<size_t, QueryExecutionError> truncateTable(std::string_view tableName);
    [[nodiscard]] bool hasTable(std::string_view tableName) const;

    std::expected<void, QueryExecutionError> storeTable(std::string_view tableName) {
        if (!hasTable(tableName)) return std::unexpected(QueryExecutionError("Table doesn't exist"));

        Serializer serializer{fmt::format("{}{}\\{}{}", dbFolderPrefix, name, tableName, dbTableFileExtension)};
        serializer.serialize(getTable(tableName)->get());

        return {};
    }

    std::expected<void, QueryExecutionError> restoreTable(std::string_view tableName) {
        const std::string filePath = fmt::format("{}{}\\{}{}", dbFolderPrefix, name, tableName, dbTableFileExtension);
        if (!std::filesystem::exists(filePath)) return std::unexpected(QueryExecutionError("Table is not saved on the disk"));

        Deserializer deserializer{filePath};

        tables.insert_or_assign(std::string(tableName), deserializer.deserializeTable());
        return {};
    }


    // Database
    void truncate() { tables.clear(); }
};