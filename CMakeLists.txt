cmake_minimum_required(VERSION 3.16)

project(BankingSystem VERSION 4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

# Detect C++ source files
file(GLOB CPP_SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")

message(STATUS "Detected C++ sources:")
foreach(SRC ${CPP_SOURCES})
    message(STATUS "  ${SRC}")
endforeach()

# Header-only dependencies used by the current project:
#   cpp-httplib -> include/httplib.h
#   nlohmann/json -> include/json.hpp
#   picosha2 -> include/picosha2.h
if(NOT EXISTS "${INC_DIR}/httplib.h")
    message(FATAL_ERROR "cpp-httplib not found: ${INC_DIR}/httplib.h")
endif()

if(NOT EXISTS "${INC_DIR}/json.hpp")
    message(FATAL_ERROR "nlohmann/json not found: ${INC_DIR}/json.hpp")
endif()

if(NOT EXISTS "${INC_DIR}/picosha2.h")
    message(FATAL_ERROR "picosha2 not found: ${INC_DIR}/picosha2.h")
endif()

add_library(project_dependencies INTERFACE)

target_include_directories(project_dependencies
    INTERFACE
        "${INC_DIR}"
)

# Warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Backend executables.
# The HTTP servers currently execute:
#   ./src/user
#   ./src/admin
# so the output directory is kept as src/.
if(EXISTS "${SRC_DIR}/user.cpp")
    add_executable(user "${SRC_DIR}/user.cpp")
    target_link_libraries(user PRIVATE project_dependencies)
    set_target_properties(user PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SRC_DIR}"
    )
else()
    message(FATAL_ERROR "src/user.cpp was not found.")
endif()

if(EXISTS "${SRC_DIR}/admin.cpp")
    add_executable(admin "${SRC_DIR}/admin.cpp")
    target_link_libraries(admin PRIVATE project_dependencies)
    set_target_properties(admin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SRC_DIR}"
    )
else()
    message(FATAL_ERROR "src/admin.cpp was not found.")
endif()

# User HTTP server
if(EXISTS "${SRC_DIR}/user-server.cpp")
    add_executable(user-server "${SRC_DIR}/user-server.cpp")
    target_link_libraries(user-server PRIVATE project_dependencies)
    set_target_properties(user-server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SRC_DIR}"
    )
else()
    message(FATAL_ERROR "src/user-server.cpp was not found.")
endif()

# Admin HTTP server
if(EXISTS "${SRC_DIR}/admin-server.cpp")
    add_executable(admin-server "${SRC_DIR}/admin-server.cpp")
    target_link_libraries(admin-server PRIVATE project_dependencies)
    set_target_properties(admin-server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SRC_DIR}"
    )
else()
    message(FATAL_ERROR "src/admin-server.cpp was not found.")
endif()

# Build all Phase 4 executables at once
add_custom_target(phase4 ALL
    DEPENDS user admin user-server admin-server
)

message(STATUS "")
message(STATUS "========== Banking System - Phase 4 ==========")
message(STATUS "C++ standard      : C++17")
message(STATUS "Source directory  : ${SRC_DIR}")
message(STATUS "Include directory : ${INC_DIR}")
message(STATUS "cpp-httplib       : ${INC_DIR}/httplib.h")
message(STATUS "nlohmann/json     : ${INC_DIR}/json.hpp")
message(STATUS "picosha2          : ${INC_DIR}/picosha2.h")
message(STATUS "Targets           : user, admin, user-server, admin-server")
message(STATUS "==============================================")
message(STATUS "")
