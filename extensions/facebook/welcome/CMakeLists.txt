#ADD_SUBDIRECTORY(welcome)

INCLUDE_DIRECTORIES(
    ${QIMAGE_BLITZ_INCLUDE_DIR}/qimageblitz
    )

QT4_WRAP_UI(QT_LOGIN_UI login.ui)

set(sourceFiles
    ${QT_LOGIN_UI}
    login.cpp
    welcomewidget.cpp
    frameitem.cpp
    plugin.cpp
    welcomeinterface.cpp
    welcomeitem.cpp
    )

set(headerFiles
    login.h
    welcomeck_ck_widget.h
    frameitem.h
    plugin.h
    welcomeinterface.h
    welcomeitem.h
    )

set(QTMOC_SRCS login.h
    welcomeck_ck_widget.h
    frameitem.h
    plugin.h
    welcomeinterface.h
    welcomeitem.h
    )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QIMAGE_BLITZ_LIBRARY}
    )

ADD_LIBRARY(plexywelcome SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(plexywelcome
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

INSTALL(TARGETS plexywelcome DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES welcome.desktop DESTINATION share/plexy/ext/groups)
