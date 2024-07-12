#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <memory>

#include "database.hpp"
#include "deserializer.hpp"

#include "utils.hpp"
#include "user_settings.hpp"
#include "output_formatter.hpp"
#include "query/parser.hpp"
#include "query/query_base.hpp"
#include "query/query_builder.hpp"

namespace fs = std::filesystem;

bool inputProcessor(std::string& in, UserSettings& settings) {
    if (in.starts_with("#file ")) {
        if (!fs::exists(in.substr(6))) {
            fmt::println("File '{}' doesn't exist!", in.substr(6));
            return false;
        }

        std::ifstream file(in.substr(6), std::ios::in);

        in.clear();
        for (std::string line{}; std::getline(file, line); ) {
            in.append(line);
            in.append(" ");
        }

        return true;
    }
    if (in.starts_with("!")) {
        const auto flagName = in.substr(1);
        const auto flag = settings.get<bool>(flagName);
        if (flag) settings.set(flagName, !flag.value());
        else {
            fmt::println("Flag doesn't exist!");
        }
        return false;
    }
    return true;
}











int main() {
    const std::string username = "filip";
    const std::string dbName = "main";

    const auto settings = std::make_shared<UserSettings>();
    const ColoredOutputFormatter formatter(settings);

    Database db{"main"};

    cli::printWelcomeScreen();

    while (true) {
        bool isOutputColored = settings->get<bool>("colors").value_or(false);
        cli::printCommandPrompt(isOutputColored);
        std::string input{};
        std::getline(std::cin, input);

        // Preprocess input
        if (!inputProcessor(input, *settings)) continue;

        auto parseResult = QueryBuilder::process(input);

        if (!parseResult) {
            fmt::println("{}", formatter.format(parseResult.error()));
            continue;
        }

        const auto& queries = parseResult.value();

        for (const auto& q : queries) {
            const auto result = q->execute(db);

            fmt::println("{}",
                result ? formatter.format(result.value()) : formatter.format(result.error()));
        }
    }

    return 0;
}
