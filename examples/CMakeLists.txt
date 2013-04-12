file(GLOB ALL_EXAMPLES *-*/*.qml)
install(FILES ${ALL_EXAMPLES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/u1db-qt/examples
    )

