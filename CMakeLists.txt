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

if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets QuickWidgets Qml)
endif()

# ── Shared library: shaderqt ──────────────────────────────────────────────────
set(SHADERQT_LIB_SOURCES
    include/shaderqt/shaderqt_global.h
    include/shaderqt/shaderqt.h
    include/shaderqt/ShadyGradientWidget.h
    include/shaderqt/ShadyGradientEffect.h
    shaderqt.cpp
    src/ShadyGradientWidget.cpp
    src/ShadyGradientEffect.cpp
    shaders.qrc
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt6_add_qml_module(shaderqt
        URI ShaderQt
        VERSION 1.0
        RESOURCE_PREFIX /qt/qml
        QML_FILES
            qml/ShadyGradientItem.qml
        SOURCES
            ${SHADERQT_LIB_SOURCES}
    )
else()
    add_library(shaderqt SHARED ${SHADERQT_LIB_SOURCES})
endif()

target_include_directories(shaderqt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/shaderqt>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
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
            Qt6::Qml
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

set_target_properties(shaderqt PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

# ── Preview executable ────────────────────────────────────────────────────────
option(SHADERQT_BUILD_PREVIEW "Build the preview application" ON)

if(SHADERQT_BUILD_PREVIEW)
    add_executable(shaderqt-preview examples/preview/main.cpp)
    target_include_directories(shaderqt-preview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/examples/preview)
    target_link_libraries(shaderqt-preview PRIVATE shaderqt)
endif()

# ── Install rules ─────────────────────────────────────────────────────────────
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "shaderqtConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS shaderqt
    EXPORT shaderqtTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

if(SHADERQT_BUILD_PREVIEW)
    install(TARGETS shaderqt-preview
        RUNTIME DESTINATION bin
    )
endif()

install(EXPORT shaderqtTargets
    FILE shaderqtTargets.cmake
    NAMESPACE ShaderQt::
    DESTINATION lib/cmake/shaderqt
)

install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaderqtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/shaderqtConfigVersion.cmake"
    DESTINATION lib/cmake/shaderqt
)

install(DIRECTORY include/shaderqt
    DESTINATION include
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
