cmake_minimum_required(VERSION 3.19)
project(QtLearn LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(QtLearn
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/widget.cpp
    src/widget.h
)

target_link_libraries(QtLearn
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS QtLearn
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtLearn
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
