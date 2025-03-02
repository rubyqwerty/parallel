cmake_minimum_required(VERSION 3.5.0)
project(race VERSION 0.1.0 LANGUAGES C CXX)

find_package(FTXUI)

add_compile_definitions(MQ_NAME="/mq")

add_executable(race src/main.cpp)

target_link_libraries(race PRIVATE ftxui::screen ftxui::dom ftxui::component)

