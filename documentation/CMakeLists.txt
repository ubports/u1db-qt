option(BUILD_DOCS "Build documentation" OFF)
if (BUILD_DOCS)
    set(ALL "ALL")
endif ()

set(U1DB_DOCS "${CMAKE_CURRENT_BINARY_DIR}/output")

add_custom_command(OUTPUT overview.html
    COMMAND "mkdir" "-p" "${U1DB_DOCS}"
    COMMAND "qdoc" "-outputdir" "${U1DB_DOCS}" "${CMAKE_CURRENT_SOURCE_DIR}/u1db.qdocconf" "2>" "${CMAKE_CURRENT_BINARY_DIR}/qdoc.err"
    COMMAND "cat" "${CMAKE_CURRENT_BINARY_DIR}/qdoc.err"
    COMMAND "test" "!" "-s" "${CMAKE_CURRENT_BINARY_DIR}/qdoc.err"
    COMMAND "sed" "-r" "-i" "'s@(</head>)@<link rel=\"stylesheet\" type=\"text/css\" href=\"style/base.css\" />\\1@'" "${U1DB_DOCS}/*.html"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS U1DBPlugin
    )
add_custom_command(OUTPUT u1dbqt.qhp
    COMMAND "qhelpgenerator" "${U1DB_DOCS}/u1dbqt.qhp"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS overview.html
    )
add_custom_target(doc ${ALL} DEPENDS u1dbqt.qhp overview.html)

if (BUILD_DOCS)
    install(FILES "${U1DB_DOCS}/u1dbqt.qch"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/qt5/phrasebooks/"
        )
    install(DIRECTORY "${U1DB_DOCS}/"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/u1db-qt/doc/html"
        FILES_MATCHING PATTERN "*.html"
        )
    install(DIRECTORY "${U1DB_DOCS}/"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/u1db-qt/doc/html"
        FILES_MATCHING PATTERN "*.index"
        )
    install(DIRECTORY "${U1DB_DOCS}/style/"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/u1db-qt/doc/html/style"
        FILES_MATCHING PATTERN "*.css"
        )
    install(DIRECTORY
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/u1db-qt/examples
        FILES_MATCHING PATTERN "*example-*.html"
        )
endif ()

