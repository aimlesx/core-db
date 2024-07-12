cmake_minimum_required(VERSION 3.28)
project(ProjektPJC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.4
        )
FetchContent_MakeAvailable(magic_enum)

add_executable(ProjektPJC src/main.cpp
        src/database.cpp
        src/output_formatter.cpp
        src/user_settings.cpp
        src/table/table_field.cpp
        src/table/table.cpp
        src/query/sql_lexer.cpp
        src/query/clause/values_clause.cpp
        src/query/query_builder.cpp
        src/query/create_table_query.cpp
        src/query/select_query.cpp
        src/query/clause/order_by_clause.cpp
        src/serializer.hpp)
target_link_libraries(ProjektPJC fmt magic_enum)