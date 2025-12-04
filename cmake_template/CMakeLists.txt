cmake_minimum_required(VERSION 3.22)

project(
  MyProject
  VERSION 0.1.0
  DESCRIPTION "C Project"
  LANGUAGES C)
add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# ---- Configure Project ----

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ProjectOptions)
include(CompilerFlags)
include(ProjectLibraries)
include(CustomTargets)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
# Unistall: run from `build` directory: `sudo xargs rm < install_manifest.txt`

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
