cmake_minimum_required (VERSION 3.11)

add_subdirectory(third_party)

project(smkstarter
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(smkstarter
  src/main.cpp
  src/util.hpp
)

target_include_directories(smkstarter PRIVATE src)
target_link_libraries(smkstarter PRIVATE smk)

set_target_properties(smkstarter PROPERTIES CXX_STANDARD 11)

target_compile_options(smkstarter PRIVATE -Wall -Wextra -pedantic -Werror)

# Detect emscripten is used.
if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$") 

  set_property(TARGET smkstarter APPEND_STRING PROPERTY LINK_FLAGS  " -s WASM=1")

  # Copy the index.html file.
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html
    ${CMAKE_CURRENT_BINARY_DIR}/index.html
    COPYONLY
  )
endif()
