#pragma once
#include <fstream>

#include "table/table.hpp"

class Deserializer {
public:
    explicit Deserializer(const std::string_view fileName) {
        file = std::ifstream(fileName.data(), std::ios::in);
    }


    Table deserializeTable() {
        const std::string name = deserializeString<char>();

        const auto columnCount = deserialize<size_t>();
        std::vector<Table::ColumnSpecification> specs{columnCount};

        for (auto& [name, type] : specs) {
            type = deserialize<Field::Type>();
            name = deserializeString<char>();
        }

        const auto rowCount = deserialize<size_t>();
        std::vector<Table::Row> rows{rowCount};
        for (auto& row : rows) {
            row.resize(columnCount);
            for (int i = 0; i < columnCount; i++) {
                const auto type = specs[i].type;
                row[i] = std::make_shared<Field>(deserializeField(type));
            }
        }

        Table table{name, specs};
        table.rows = rows;

        return table;
    }

    template<typename T>
        T deserialize() {
        T value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    template<typename TSize>
    std::string deserializeString() {
        const auto size = deserialize<TSize>();
        std::string s{};
        s.resize(size);
        file.read(s.data(), size);
        return s;
    }

    Field::VariantType deserializeField(const Field::Type type) {
        switch (type) {
            case Field::Type::Boolean:
                return {deserialize<bool>()};
                break;
            case Field::Type::Double:
                return {deserialize<double>()};
                break;
            case Field::Type::Int:
                return {deserialize<int>()};
                break;
            case Field::Type::String:
                return {deserializeString<char>()};
                break;
        }
        return {"NULL"};
    }
private:
    std::ifstream file{};
};
