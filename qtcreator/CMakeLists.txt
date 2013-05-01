set(U1DB_WIZARDS "${CMAKE_INSTALL_PREFIX}/share/qtcreator/templates/qml")

install(DIRECTORY "settings"
    DESTINATION "${U1DB_WIZARDS}"
    )

install(DIRECTORY "contacts"
    DESTINATION "${U1DB_WIZARDS}"
    )

