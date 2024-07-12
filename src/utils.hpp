#pragma once
#include <chrono>

#include <fmt/core.h>
#include <fmt/color.h>

struct sv_hash {
    using hash_type = std::hash<std::string_view>;
    using is_transparent = void;

    std::size_t operator()(const char* str) const { return hash_type{}(str); }
    std::size_t operator()(const std::string_view str) const { return hash_type{}(str); }
    std::size_t operator()(const std::string& str) const { return hash_type{}(str); }
};

namespace cli {
    inline void printWelcomeScreen() noexcept {
        std::string logo = "   ____               ____  ____  \n"
                           "  / ___|___  _ __ ___|  _ \\| __ ) \n"
                           " | |   / _ \\| '__/ _ \\ | | |  _ \\ \n"
                           " | |__| (_) | | |  __/ |_| | |_) |\n"
                           "  \\____\\___/|_|  \\___|____/|____/ \n"
                           "                                  ";

        fmt::println("{}", logo);
    }

    inline void printCommandPrompt(const bool colored = false) noexcept {
        using namespace fmt;

        fmt::print(":{} ", colored ? format(fg(color::dark_cyan), ">") : ">");
    }
}

class ScopedTimer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    TimePoint start;
public:
    explicit ScopedTimer(): start(Clock::now()) {}

    [[nodiscard]] float get() const { return std::chrono::duration<float, std::milli>(Clock::now() - start).count(); }
};