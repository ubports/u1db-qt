project(u1db-qt)
cmake_minimum_required(VERSION 2.8.6)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Dependencies
include(FindPkgConfig)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
add_definitions(-DWITHQT5=1)

set(U1DB_QT_LIBNAME u1db-qt5)
set(OUR_QT_INCLUDES ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS})
set(OUR_QT_CORE_LIB ${Qt5Core_LIBRARIES})
set(OUR_QT_SQL_LIB ${Qt5Sql_LIBRARIES})
set(QT_PKGCONFIG_DEPENDENCIES "QtCore QtQuick QtSql")

set(QT_U1DB_PKGCONFIG_FILE lib${U1DB_QT_LIBNAME}.pc)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -Wundef -std=c++0x")

# Sources
set(U1DB_QT_SRCS
    database.cpp
    document.cpp
    index.cpp
    query.cpp
    )

# Generated files
set(U1DB_QT_GENERATED
    moc_database.cpp
    moc_document.cpp
    moc_index.cpp
    moc_query.cpp
    )

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${U1DB_QT_GENERATED}")

# Resources
set(U1DB_QT_RESOURCES
    sql.qrc
    )

QT5_ADD_RESOURCES(U1DB_QT_RCC ${U1DB_QT_RESOURCES})

## QtU1DBQml
set(QtU1DBQml_SRCS
    plugin.cpp
    )

# Build
add_definitions(-DQT_NO_KEYWORDS)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OUR_QT_INCLUDES}
    ${U1DB_INCLUDE_DIRS}
    )

add_library(${U1DB_QT_LIBNAME} SHARED ${U1DB_QT_SRCS} ${U1DB_QT_RCC})
target_link_libraries(${U1DB_QT_LIBNAME}
    ${OUR_QT_CORE_LIB}
    ${OUR_QT_SQL_LIB}
    ${U1DB_LDFLAGS}
    )

set_target_properties(${U1DB_QT_LIBNAME} PROPERTIES
  SOVERSION 3
  VERSION 3.0.0
  )

# Unit-Test
enable_testing()

add_custom_target(check COMMAND "env" "CTEST_OUTPUT_ON_FAILURE=1" "${CMAKE_CTEST_COMMAND}")
add_custom_target(doc "qdoc" "u1db.qdocconf"
    COMMAND "sed" "-r" "-i" "'s@(</head>)@<link rel=\"stylesheet\" type=\"text/css\" href=\"style/base.css\" />\\1@'" "./output/html/*.html"
    COMMAND "qhelpgenerator" "./output/html/u1dbqt.qhp"
    COMMAND "assistant" "-register" "./output/html/u1dbqt.qch"
    WORKING_DIRECTORY "documentation")
add_subdirectory(modules)
add_subdirectory(tests)

# Install
set(INCLUDE_INSTALL_DIR include/lib${U1DB_QT_LIBNAME})

install(TARGETS ${U1DB_QT_LIBNAME}
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

install(FILES database.h document.h index.h query.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
    )

# PkgConfig file
set (PREFIX "${CMAKE_INSTALL_PREFIX}")
set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")

configure_file (libu1db-qt.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${QT_U1DB_PKGCONFIG_FILE} @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${QT_U1DB_PKGCONFIG_FILE}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
    )

