# Project Needs a name ofcourse
project(athkar)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_QTSQL_INCLUDE_DIR}
   )
 
# We add our source code here
set(athkar_SRCS src/athkar.cpp src/database.cpp src/wathakker.cpp src/voice.cpp src/hisnmuslim.cpp)

kde4_add_ui_files(athkar_SRCS form/configform.ui form/dbform.ui form/wathakkerform.ui form/voiceform.ui)
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_athkar ${athkar_SRCS})
target_link_libraries(plasma_applet_athkar 
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTNETWORK_LIBRARY})
 
install(TARGETS plasma_applet_athkar
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma_applet_athkar.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES data/data.db data/hisnmuslim.db
        DESTINATION ${DATA_INSTALL_DIR}/athkar)

add_subdirectory(po)
add_subdirectory(icons)