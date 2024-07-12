#pragma once
#include <variant>
#include <string>

#include "../query/sql_lexer.hpp"

#pragma region OverloadPattern
template<typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};
// Deduction Guide (for C++17)
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;
#pragma endregion

class Field {
    friend class Serializer;
public:
    using VariantType = std::variant<std::string, int, double, bool>;
    enum class Type { String = 0, Int = 1, Double = 2, Boolean = 3 };

    explicit Field(VariantType value): data(std::move(value)) {}

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] size_t getTypeIndex() const { return data.index(); }
    [[nodiscard]] static Field createFromToken(const SQLLexer::Token& token);

    auto operator<=>(const Field&) const = default;

private:
    VariantType data;
};
