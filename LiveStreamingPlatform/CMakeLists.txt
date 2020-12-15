cmake_minimum_required(VERSION 3.5)
set( CMAKE_TOOLCHAIN_FILE "D:/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake" )

project(LiveStreamingPlatform LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Quick Widgets Network REQUIRED)

set( OpenCV_DIR "D:/ThirdParty/vcpkg/installed/x64-windows/share/opencv/" )
find_package( OpenCV REQUIRED )
if( OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${OpenCV_LIB_DIR} )
endif()

set( FFmpeg_DIR "D:/ThirdParty/vcpkg/installed/x64-windows/share/ffmpeg/" )
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE REQUIRED)

if( FFmpeg_FOUND )
  # Additional Include Directories
  include_directories( ${FFmpeg_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${FFmpeg_LIB_DIRS} )

endif()

if(ANDROID)
    add_library(LiveStreamingPlatform SHARED
      main.cpp
      qml.qrc
    )
else()
    add_executable(LiveStreamingPlatform
      main.cpp
      qml.qrc
    )
endif()


target_compile_definitions(LiveStreamingPlatform
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(LiveStreamingPlatform
  PRIVATE Qt5::Core Qt5::Quick Qt5::Widgets Qt5::Network ${OpenCV_LIBS} ${FFMPEG_LIBRARIES})
