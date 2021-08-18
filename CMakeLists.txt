project(achelper)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Widgets Gui)
find_package(Qt5 REQUIRED COMPONENTS Quick Core Gui Sql X11Extras OpenGL Network PrintSupport WebEngineWidgets SerialPort SerialBus Charts)
find_package(Qt5QuickCompiler)

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})


include_directories(src resource)

set(HEADERS
    src/mainwindow.h 
)

set(SOURCE_achelper
    src/mainwindow.cpp
    src/main.cpp 
)

qt5_wrap_cpp(HEADER_achelper ${HEADERS})
qt5_wrap_ui(UI_achelper src/mainwindow.ui)
qtquick_compiler_add_resources(RESOURCES resource/resources.qrc)

add_executable(achelper 
    ${SOURCE_achelper} ${HEADER_achelper} ${UI_achelper} ${RESOURCES}
)


target_link_libraries(achelper PUBLIC 
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::WebEngineWidgets Qt5::OpenGL Qt5::Charts
) 
