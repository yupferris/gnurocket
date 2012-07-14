project(qtrocket)

find_package(Qt4 REQUIRED)

set(sources clientsocket.cpp main.cpp mainwindow.cpp ServerWrapper.cpp SyncTrack.cpp TrackModel.cpp trackview.cpp)
set(qtrocket_HEADERS mainwindow.h clientsocket.h ServerWrapper.h SyncTrack.h TrackModel.h trackview.h)

set(QT_USE_QTNETWORK true)
qt4_wrap_cpp(qtrocket_HEADERS_MOC ${qtrocket_HEADERS})
qt4_wrap_ui(qtrocket_FORMS_HEADERS mainwindow.ui)

include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(qtrocket ${sources} ${qtrocket_HEADERS_MOC} ${qtrocket_HEADERS} ${qtrocket_FORMS_HEADERS})

target_link_libraries(qtrocket ${QT_LIBRARIES})
