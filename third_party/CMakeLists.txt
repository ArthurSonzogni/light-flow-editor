include(FetchContent)

#set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(smk
  GIT_REPOSITORY https://github.com/ArthurSonzogni/smk
  GIT_TAG bff29c92a0eb9800a3608eae713200bc5c12dab0
)

FetchContent_GetProperties(smk)
if(NOT smk_POPULATED)
  FetchContent_Populate(smk)
  add_subdirectory(${smk_SOURCE_DIR} ${smk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
