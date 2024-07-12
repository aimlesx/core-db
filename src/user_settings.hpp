#pragma once

#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <optional>

class UserSettings {
public:
    using VariantType = std::variant<bool>; // Kiedys cos sie doda

    explicit UserSettings();

    void set(std::string_view settingName, VariantType value);

    template<typename T>
    std::optional<T> get(const std::string_view settingName) const noexcept {
        const auto name = std::string(settingName);

        if (!settings.contains(name)) return std::nullopt;

        const auto& variant = settings.find(name)->second;

        if (!std::holds_alternative<T>(variant)) return std::nullopt;

        return std::get<T>(variant);
    }

private:
    std::map<std::string, VariantType> settings{};
};