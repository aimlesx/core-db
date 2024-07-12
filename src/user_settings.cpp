#include "user_settings.hpp"

UserSettings::UserSettings() {
    settings.try_emplace("colors", true);
    settings.try_emplace("output.compact", false);
}

void UserSettings::set(const std::string_view settingName, VariantType value) {
    const auto [element, inserted] =
        settings.try_emplace(std::string(settingName), value);

    if(!inserted) element->second = value;
}
