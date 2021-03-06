project(u1db-qt)
cmake_minimum_required(VERSION 2.8.6)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Dependencies
include(FindPkgConfig)
include(GNUInstallDirs)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
add_definitions(-DWITHQT5=1)

set(U1DB_QT_LIBNAME u1db-qt5)
set(QT_PKGCONFIG_DEPENDENCIES "Qt5Core Qt5Network Qt5Quick Qt5Sql")
set(QT_U1DB_PKGCONFIG_FILE lib${U1DB_QT_LIBNAME}.pc)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -Wundef -Werror -std=c++0x")
add_definitions(-DQT_NO_KEYWORDS)
# Disable building during install to avoid file permission chaos
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY 1)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
add_subdirectory(modules)
add_subdirectory(examples)
add_subdirectory(gallery)
add_subdirectory(documentation)
add_subdirectory(qtcreator)

# PkgConfig file
set (PREFIX "${CMAKE_INSTALL_PREFIX}")
set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

configure_file (libu1db-qt.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${QT_U1DB_PKGCONFIG_FILE} @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${QT_U1DB_PKGCONFIG_FILE}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
