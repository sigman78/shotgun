cmake_minimum_required(VERSION 2.8.12)
project(Shotgun)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules/")

set(SDL_SHARED OFF CACHE BOOL "SDL2 Static only")

set(TARGET_GLES2 ON CACHE BOOL "GLES2 target")
set(BUILD_STATIC ON CACHE BOOL "Static only")
set(BUILD_PLUGINS_STATIC ON CACHE BOOL "Static only")

#set(WITH_SDL2APPLICATION ON CACHE BOOL "SDL2 target")
set(WITH_GLFWAPPLICATION ON CACHE BOOL "GLFW target")

set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "import plugin")
set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "import plugin")
set(WITH_OBJIMPORTER ON CACHE BOOL "import plugin")
set(WITH_TGAIMPORTER ON CACHE BOOL "import plugin")

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
