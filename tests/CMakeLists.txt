set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

# common CMake macro for linuxdeploy, adding tests only if they are supposed to be built
# otherwise if you try to run CTest the tests aren't build and CTest can't find them
if(NOT COMMAND ld_add_test)
    function(ld_add_test TARGET_NAME)
        get_target_property(${TARGET_NAME}_EXCLUDE_FROM_ALL ${TARGET_NAME} EXCLUDE_FROM_ALL)
        if(NOT ${${TARGET_NAME}_EXCLUDE_FROM_ALL})
            message(STATUS "[${PROJECT_NAME}] Adding test ${TARGET_NAME}")

            # https://stackoverflow.com/a/35697020
            add_test(NAME ${TARGET_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}>)
        else()
            message(STATUS "[${PROJECT_NAME}] Test ${TARGET_NAME} is excluded from ALL, not adding as test")
        endif()
    endfunction()
endif()

# build a single test binary
add_executable(test_desktopfile
    test_desktopfile.cpp
    test_desktopfileentry.cpp
    test_desktopfilereader.cpp
    test_desktopfilewriter.cpp
    test_desktopfile_conformance.cpp
    main.cpp
)

target_link_libraries(test_desktopfile PRIVATE linuxdeploy_desktopfile gtest)

set_property(TARGET test_desktopfile
    PROPERTY COMPILE_FLAGS "-DDESKTOP_FILE_PATH=\\\"${TEST_DATA_DIR}/simple_app.desktop\\\""
)

ld_add_test(test_desktopfile test_desktopfile)
