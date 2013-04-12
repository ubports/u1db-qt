find_package(Qt5Test REQUIRED)
set(OUR_QT_TEST_LIB ${Qt5Test_LIBRARIES})
set(OUR_QT_TEST_INCLUDES ${Qt5Test_INCLUDE_DIRS})

add_test(NAME plugintest COMMAND "qmltestrunner" "-maxwarnings" "20" "-platform" "minimal" "-import" "../modules" "-xunitxml" "-o" "plugintest-xunit.xml" "-input" "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OUR_QT_TEST_INCLUDES}
    ${CMAKE_SOURCE_DIR}/src
    ${OUR_QT_QUICK_INCLUDE}
    )

add_executable(test-database test-database.cpp)
target_link_libraries(test-database ${OUR_QT_WIDGETS_LIB} ${OUR_QT_TEST_LIB} ${OUR_QT_QUICK_LIB} ${OUR_QT_SQL_LIB} ${U1DB_QT_LIBNAME})
set_target_properties(test-database PROPERTIES COMPILE_FLAGS -fPIC)
add_test(NAME test-database COMMAND "dbus-test-runner" "--task" "${CMAKE_CURRENT_BINARY_DIR}/test-database" "-p" "-xunitxml" "-p" "-o" "-p" "test-database-xunit.xml")
set_property(TEST test-database PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=.")
set_property(TEST test-database PROPERTY ENVIRONMENT "QT_QPA_PLATFORM=minimal")
add_dependencies(check test-database)

# pkg_search_module(PYTHON REQUIRED python-2.7)
# if (PYTHON_FOUND)
find_package(PythonLibs)
if (PYTHONLIBS_FOUND)
    set(U1DB_QT_PY_SRCS qt-backend-wrapper.cpp)
    include_directories(${PYTHON_INCLUDE_DIRS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -Wall -Wundef -std=c++0x")
    add_library(u1dbqt SHARED ${U1DB_QT_PY_SRCS})
    target_link_libraries(u1dbqt ${OUR_QT_CORE_LIB} ${OUR_QT_SQL_LIB} ${U1DB_QT_LIBNAME} ${PYTHON_LDFLAGS})
    # target_link_libraries(u1dbqt ${OUR_QT_CORE_LIB} ${OUR_QT_SQL_LIB} ${U1DB_QT_LIBNAME} ${PYTHON_LIBRARIES})
    add_custom_command(TARGET u1dbqt POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink libu1dbqt.so u1dbqt.so)
endif ()

