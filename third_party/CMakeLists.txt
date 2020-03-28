include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED false)
FetchContent_Declare(smkflow
  #GIT_REPOSITORY https://github.com/ArthurSonzogni/smkflow
  GIT_REPOSITORY git@github.com:ArthurSonzogni/smkflow
  GIT_TAG 92c05cd37555e12543387a5b1545e4901a9d3772
)

FetchContent_GetProperties(smkflow)
if(NOT smkflow_POPULATED)
  FetchContent_Populate(smkflow)
  add_subdirectory(
    ${smkflow_SOURCE_DIR}
    ${smkflow_BINARY_DIR}
    EXCLUDE_FROM_ALL)
endif()
