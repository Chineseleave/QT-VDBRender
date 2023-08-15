cmake_minimum_required(VERSION 3.5)

project(volumeRenderCmake VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(CUDATOOLKIT REQUIRED)
find_package(OpenVDB CONFIG REQUIRED)

# 开启延迟加载和压缩解压缩功能
add_definitions(-DOPENVDB_USE_DELAYED_LOADING -DOPENVDB_USE_BLOSC)
set(PROJECT_SOURCES
        vdb_module.h
        vdb_module.cpp
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        #ConvertVdbTo3DTexture.cu
        Camera.cpp
        Camera.h
        geometrysample.h

)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(volumeRenderCmake
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET volumeRenderCmake APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(volumeRenderCmake SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(volumeRenderCmake
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(volumeRenderCmake PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(volumeRenderCmake PRIVATE Qt6::Gui)
target_link_libraries(volumeRenderCmake PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(volumeRenderCmake PRIVATE OpenVDB::openvdb)
target_link_libraries(volumeRenderCmake PRIVATE Qt6::Concurrent)
target_link_libraries(volumeRenderCmake PRIVATE CUDA::cudart)
target_link_libraries(volumeRenderCmake ${CUDA_LIBRARIES})
target_link_libraries(volumeRenderCmake PRIVATE CUDA::cufft)
set_target_properties(volumeRenderCmake PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /openmp")
endif()

install(TARGETS volumeRenderCmake
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(volumeRenderCmake)
endif()
