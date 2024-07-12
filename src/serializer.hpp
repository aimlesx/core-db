#pragma once
#include <fstream>

#include "table/table.hpp"

class Serializer {
public:
    explicit Serializer(const std::string_view fileName) {
        file = std::ofstream(fileName.data(), std::ios::out);
    }

    void serialize(const Table& table) {
        serialize<char>(table.getName());

        const size_t columnCount = table.columns.size();
        serialize(columnCount);

        for (const auto& [name, type] : table.columns) {
            serialize(type);
            serialize<char>(name);
        }

        serialize(table.rows.size());
        for (const auto& row : table.rows) {
            for (const auto& field : row) {
                serialize(field->data);
            }
        }
    }

    void serialize(auto value) {
        file.write(reinterpret_cast<char*>(&value), sizeof(value));
    }

    template<typename Size>
    void serialize(const std::string& str) {
        const Size size = str.size();
        serialize(size);
        file.write(str.c_str(), static_cast<Size>(str.size()));
    }

    void serialize(const Field::VariantType& var) {
        std::visit([this](const auto& v){ serialize(v); }, var);
    }

private:
    std::ofstream file{};
};
