cmake_minimum_required(VERSION 3.10)

project(ExpenseAPI)
set(CMAKE_CXX_STANDARD 17)

# Include Crow and Asio headers
include_directories(${CMAKE_SOURCE_DIR}/crow/include)
include_directories(${CMAKE_SOURCE_DIR}/crow/include/crow/middlewares)
include_directories("/opt/homebrew/include")

# Let the linker know where to find SQLiteCpp and dependencies
link_directories("/opt/homebrew/lib")

add_executable(expense_api api.cpp service.cpp)

# Only link SQLiteCpp (Crow and Asio are header-only)
target_link_libraries(expense_api SQLiteCpp)
