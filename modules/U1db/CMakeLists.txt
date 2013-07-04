find_package(Qt5Quick REQUIRED)
find_package(Qt5Network REQUIRED)

get_target_property(QMAKE_EXECUTABLE Qt5::qmake LOCATION)
# See http://doc-snapshot.qt-project.org/5.0/qtcore/qlibraryinfo.html#LibraryLocation-enum
# exec_program(${QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE QT_IMPORTS_DIR)
exec_program(${QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE QT_IMPORTS_DIR)
file(TO_CMAKE_PATH "${QT_IMPORTS_DIR}" QT_IMPORTS_DIR)

set(U1DBPlugin_SRCS
    plugin.cpp
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt5Sql_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    )

add_library(U1DBPlugin SHARED ${U1DBPlugin_SRCS})
add_dependencies(check U1DBPlugin)

target_link_libraries(U1DBPlugin
  ${U1DB_QT_LIBNAME}
  ${Qt5Quick_LIBRARIES}
  ${Qt5Network_LIBRARIES}
  )

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${U1DB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    )

# copy qmldir file into build directory for shadow builds
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

install(TARGETS U1DBPlugin
    LIBRARY DESTINATION ${QT_IMPORTS_DIR}/U1db
    )

install(FILES qmldir
    DESTINATION ${QT_IMPORTS_DIR}/U1db
    )
