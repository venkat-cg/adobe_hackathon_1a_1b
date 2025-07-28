cmake_minimum_required(VERSION 3.10)
project(pdf_project)

set(CMAKE_CXX_STANDARD 17)

# Find Poppler package
find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler)

include_directories(${POPPLER_INCLUDE_DIRS})
link_directories(${POPPLER_LIBRARY_DIRS})

add_executable(main
    src/main.cpp
    src/outline_extractor.cpp
    src/utils/file_utils.cpp
    src/utils/logger.cpp
    src/utils/json_utils.cpp
    src/task1a.cpp
    src/task1b.cpp
)

target_link_libraries(main ${POPPLER_LIBRARIES})
target_link_libraries(main nlohmann_json)
