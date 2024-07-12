#pragma once

#include <exception>
#include <string>

class QueryExecutionError final : std::exception {
    friend class ColoredOutputFormatter;
public:
    explicit QueryExecutionError(const std::string_view reason) {
        this->reason = reason;
    }

    [[nodiscard]] const std::string& getReason() const noexcept {
        return reason;
    }

private:
    std::string reason;
};