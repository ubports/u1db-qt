add_custom_target(check COMMAND "env" "CTEST_OUTPUT_ON_FAILURE=1" "${CMAKE_CTEST_COMMAND}")

find_package(Qt5Test REQUIRED)

add_test(NAME plugintest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/strict-qmltestrunner.sh" "-import" "../modules" "-xunitxml" "-o" "plugintest-xunit.xml" "-input" "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Test_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${Qt5Sql_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    )

add_executable(test-database test-database.cpp)
target_link_libraries(test-database
    ${Qt5Test_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    ${U1DB_QT_LIBNAME}
    )
set_target_properties(test-database PROPERTIES COMPILE_FLAGS -fPIC)
add_test(NAME test-database COMMAND "dbus-test-runner" "--task" "${CMAKE_CURRENT_BINARY_DIR}/test-database" "-p" "-xunitxml" "-p" "-o" "-p" "test-database-xunit.xml")
set_property(TEST test-database PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=.")
set_property(TEST test-database PROPERTY ENVIRONMENT "QT_QPA_PLATFORM=minimal")
add_dependencies(check test-database)

option(BUILD_PYTHON "Build Python wrapper" OFF)
if (BUILD_PYTHON)
    find_package(PythonLibs)
    if (PYTHONLIBS_FOUND)
        set(U1DB_QT_PY_SRCS qt-backend-wrapper.cpp)
        include_directories(
            ${PYTHON_INCLUDE_DIRS}
            ${Qt5Sql_INCLUDE_DIRS}
            )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -Wall -Wundef -std=c++0x")
        add_library(u1dbqt SHARED ${U1DB_QT_PY_SRCS})
        target_link_libraries(u1dbqt
            ${Qt5Core_LIBRARIES}
            ${Qt5Sql_LIBRARIES}
            ${U1DB_QT_LIBNAME}
            ${PYTHON_LDFLAGS}
            )
        add_custom_command(TARGET u1dbqt POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink libu1dbqt.so u1dbqt.so)
    endif ()
endif ()

add_test(NAME licensetest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/checklicense.sh" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

