cmake_minimum_required(VERSION 3.5.0)
project(Gen VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(Gen main.cpp)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(CURL REQUIRED)

file(COPY ${CMAKE_SOURCE_DIR}/Accounts.txt DESTINATION ${CMAKE_BINARY_DIR}/CMAKE_BUILD_TYPE)

target_link_libraries(Gen PRIVATE CURL::libcurl nlohmann_json::nlohmann_json cpr::cpr)