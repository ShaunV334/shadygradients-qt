cmake_minimum_required(VERSION 3.16)

project(shaderqt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Qt6 (preferred) with fallback to Qt5 ──────────────────────────────────────
find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS Core Widgets OpenGL OpenGLWidgets Quick QuickWidgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS Core Widgets OpenGL Quick Gui)

# Qt6 has OpenGLWidgets as a separate module; Qt5 bundles it in Widgets
if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets QuickWidgets)
endif()

# ── Shared library: shaderqt ──────────────────────────────────────────────────
set(SHADERQT_LIB_SOURCES
    shaderqt_global.h
    shaderqt.cpp
    shaderqt.h
    src/ShadyGradientWidget.h
    src/ShadyGradientWidget.cpp
    src/ShadyGradientEffect.h
    src/ShadyGradientEffect.cpp
    shaders.qrc          # embeds the GLSL files as Qt resources
)

add_library(shaderqt SHARED ${SHADERQT_LIB_SOURCES})

target_include_directories(shaderqt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_definitions(shaderqt PRIVATE SHADERQT_LIBRARY)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(shaderqt
        PUBLIC
            Qt6::Core
            Qt6::Widgets
            Qt6::Gui
            Qt6::OpenGL
            Qt6::OpenGLWidgets
            Qt6::Quick
            Qt6::QuickWidgets
    )
else()
    target_link_libraries(shaderqt
        PUBLIC
            Qt5::Core
            Qt5::Widgets
            Qt5::Gui
            Qt5::OpenGL
            Qt5::Quick
    )
endif()

# ── Preview executable ────────────────────────────────────────────────────────
add_executable(shaderqt-preview
    src/preview/main.cpp
)

target_include_directories(shaderqt-preview PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(shaderqt-preview PRIVATE shaderqt)

# ── Install rules (optional) ──────────────────────────────────────────────────
install(TARGETS shaderqt shaderqt-preview
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
install(FILES
    src/ShadyGradientWidget.h
    src/ShadyGradientEffect.h
    shaderqt.h
    shaderqt_global.h
    DESTINATION include/shaderqt
)
install(FILES
    shaders/waterplane.vert
    shaders/waterplane.frag
    DESTINATION share/shaderqt/shaders
)
install(FILES
    qml/ShadyGradientItem.qml
    DESTINATION share/shaderqt/qml
)
