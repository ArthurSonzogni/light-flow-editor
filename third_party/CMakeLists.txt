include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED false)
#FetchContent_Declare(smkflow
  #GIT_REPOSITORY git@github.com:ArthurSonzogni/smkflow
  #GIT_TAG 8ac7b321fabf9d17ee2411b3437d13c1df11dfc2
#)
FetchContent_Declare(smkflow
  GIT_REPOSITORY /home/sonzogna/programmation/real/smkflow/
  GIT_TAG HEAD
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
