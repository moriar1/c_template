# ---- clang-tidy ----

# Option `-DCMAKE_C_CLANG_TIDY=clang-tidy` for run clang-tidy when building
if(CMAKE_C_CLANG_TIDY)
  message(STATUS "Incremental clang-tidy is ENABLED for all C targets.")
endif()

# Full analysis target
find_program(CLANG_TIDY_EXE "clang-tidy")
if(NOT CLANG_TIDY_EXE)
  message(
    WARNING
      "clang-tidy not found. Full analysis target 'run-tidy-full' will not be created."
  )
  return()
endif()

file(GLOB_RECURSE ALL_PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_custom_target(
  run-tidy-full
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PROJECT_NAME}
  COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_PROJECT_SOURCE_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running full clang-tidy analysis on all source files..."
  VERBATIM # for files with spaces
)
set_target_properties(run-tidy-full PROPERTIES FOLDER "Maintenance")
message(STATUS "Full clang-tidy analysis target 'run-tidy-full' created.")

# ---- build-info ----

# Usage: cmake --build build --target build-info
add_custom_target(build-info)
add_custom_command(
  TARGET build-info
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E env CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
          ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/build_info.cmake)
file(
  GENERATE
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_info.cmake
  CONTENT
    "
message(STATUS \"======================================\")
message(STATUS \"Build type: $<CONFIG>\")
message(STATUS \"Compiler: ${CMAKE_C_COMPILER_ID} (${CMAKE_C_COMPILER_VERSION})\")
message(STATUS \"Linker: ${CMAKE_LINKER}\")
message(STATUS \"Compile options:\")
message(STATUS \"$<TARGET_PROPERTY:${PROJECT_NAME},COMPILE_OPTIONS>\")
message(STATUS \"Link options:\")
message(STATUS \"$<TARGET_PROPERTY:${PROJECT_NAME},LINK_OPTIONS>\")
message(STATUS \"======================================\")
")

set_target_properties(build-info PROPERTIES FOLDER "Maintenance")
