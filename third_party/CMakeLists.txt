include(FetchContent)

#set(FETCHCONTENT_UPDATES_DISCONNECTED false)
FetchContent_Declare(smkflow
  GIT_REPOSITORY https://github.com/ArthurSonzogni/smkflow
  GIT_TAG 5ca27df9a83dd22de22d5a0fb46b29951cb52df6
)

FetchContent_GetProperties(smkflow)
if(NOT smkflow_POPULATED)
  FetchContent_Populate(smkflow)
  add_subdirectory(
    ${smkflow_SOURCE_DIR}
    ${smkflow_BINARY_DIR}
    EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(
    ${fmt_SOURCE_DIR}
    ${fmt_BINARY_DIR}
    EXCLUDE_FROM_ALL)
endif()
