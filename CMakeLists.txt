cmake_minimum_required(VERSION 2.8.12)
project(shotgun)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules/")

set(SDL_SHARED OFF CACHE BOOL "SDL2 Static only")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")

set(TARGET_GLES2 ON CACHE BOOL "GLES2 target")
set(BUILD_STATIC ON CACHE BOOL "Static only")
set(BUILD_PLUGINS_STATIC ON CACHE BOOL "Static only")

#set(WITH_SDL2APPLICATION ON CACHE BOOL "SDL2 target")
set(WITH_GLFWAPPLICATION ON CACHE BOOL "GLFW target")

set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "import plugin")
set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "import plugin")
set(WITH_OBJIMPORTER ON CACHE BOOL "import plugin")
set(WITH_TGAIMPORTER ON CACHE BOOL "import plugin")

# Warning police
set(MSVC_BUILD_FLAGS "/wd4996 /wd4312")
set(LINUX_BUILD_FLAGS "-Wall -pedantic")

if(MSVC)
#set(BUILD_FLAGS ${MSVC_BUILD_FLAGS})
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MSVC_BUILD_FLAGS}")
else()
#set(BUILD_FLAGS ${LINUX_BUILD_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LINUX_BUILD_FLAGS}")
endif()


# Set some Win32 Specific Settings
if(WIN32)
set(GUI_TYPE WIN32)
endif(WIN32)

# Set some Apple MacOS Specific settings
if(APPLE)
set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_subdirectory(3rd)
add_subdirectory(src)
