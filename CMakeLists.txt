cmake_minimum_required (VERSION 3.11)

add_subdirectory(third_party)

project(lightfloweditor
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(lightfloweditor
  src/main.cpp
  src/widget/RenderWidget.hpp
  src/widget/RenderWidget.cpp
)

target_include_directories(lightfloweditor PRIVATE src)
target_link_libraries(lightfloweditor PRIVATE smkflow::smkflow)
target_link_libraries(lightfloweditor PRIVATE fmt::fmt)

set_target_properties(lightfloweditor PROPERTIES CXX_STANDARD 17)

#target_compile_options(lightfloweditor PRIVATE -Wall -Wextra -pedantic -Werror)

# Detect emscripten is used.
if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$") 

  set_property(TARGET lightfloweditor APPEND_STRING PROPERTY LINK_FLAGS  " -s TOTAL_MEMORY=8MB")

  # Copy the index.html file.
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html
    ${CMAKE_CURRENT_BINARY_DIR}/index.html
    COPYONLY
  )

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/style.css
    ${CMAKE_CURRENT_BINARY_DIR}/style.css
    COPYONLY
  )

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/run_webassembly.py
    ${CMAKE_CURRENT_BINARY_DIR}/run_webassembly.py
  )

  # Allow some files to be fetched.
  file(GLOB files "./resources/*" "./resources/*/*")
  foreach(file ${files})
    file(RELATIVE_PATH relative_file ${CMAKE_SOURCE_DIR} ${file})
    set_property(TARGET lightfloweditor APPEND_STRING PROPERTY LINK_FLAGS  " --preload-file ${file}@/${relative_file}")
  endforeach()
endif()

#install(TARGETS lightfloweditor RUNTIME DESTINATION "bin")
