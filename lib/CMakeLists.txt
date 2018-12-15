include(CTest)

if(BUILD_TESTING AND NOT TARGET gtest)
    add_subdirectory(googletest)
endif()
