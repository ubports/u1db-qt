set(U1DB_QT_LIBNAME u1db-qt5)

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

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Sql_INCLUDE_DIRS}
    ${U1DB_INCLUDE_DIRS}
    )

add_library(${U1DB_QT_LIBNAME} SHARED ${U1DB_QT_SRCS} ${U1DB_QT_RCC})
target_link_libraries(${U1DB_QT_LIBNAME}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    ${U1DB_LDFLAGS}
    )

set_target_properties(${U1DB_QT_LIBNAME} PROPERTIES
  SOVERSION 3
  VERSION 3.0.0
  )

# Install
set(INCLUDE_INSTALL_DIR include/lib${U1DB_QT_LIBNAME})

install(TARGETS ${U1DB_QT_LIBNAME}
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

install(FILES global.h database.h document.h index.h query.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
    )

