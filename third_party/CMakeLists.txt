include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(smk
  GIT_REPOSITORY https://github.com/ArthurSonzogni/smk
  GIT_TAG 83c333edafa1c345728e764a91d33395f7258e6d
)

FetchContent_GetProperties(smk)
if(NOT smk_POPULATED)
  FetchContent_Populate(smk)
  add_subdirectory(${smk_SOURCE_DIR} ${smk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
