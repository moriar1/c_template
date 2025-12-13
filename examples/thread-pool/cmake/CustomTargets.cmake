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

# ---- clang-tidy ----

# Full analysis target

find_package(LLVM CONFIG)

find_program(CLANG_TIDY_EXE
  NAMES clang-tidy
  PATHS ${LLVM_TOOLS_BINARY_DIR}
  NO_DEFAULT_PATH
)

## or simple:
# find_program( CLANG_TIDY_EXE
#   NAMES clang-tidy clang-tidy21 clang-tidy20 clang-tidy19 clang-tidy18
#   DOC "Path to clang-tidy executable")
# if(NOT CLANG_TIDY_EXE)
#   message( WARNING "clang-tidy not found. Full analysis target 'run-tidy-full' will not be created." )

if(NOT CLANG_TIDY_EXE)
  message(WARNING "clang-tidy not found in LLVM tools directory: ${LLVM_TOOLS_BINARY_DIR}")
else()
  message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE} (from LLVM ${LLVM_PACKAGE_VERSION})")

  file(GLOB_RECURSE ALL_PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
       ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
       ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

  add_custom_target(
    run-tidy-full
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target
            ${PROJECT_NAME}
    COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_PROJECT_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running full clang-tidy analysis on all source files..."
    VERBATIM # for files with spaces
  )
  set_target_properties(run-tidy-full PROPERTIES FOLDER "Maintenance")
  message(STATUS "Full clang-tidy analysis target 'run-tidy-full' created.")
endif()

# ---- clang-format ----

find_program(CLANG_FORMAT_EXE "clang-format")
if(NOT CLANG_FORMAT_EXE)
  message(
    WARNING
      "clang-format not found. Format targets 'format' and 'check-format' will not be created."
  )
else()
  file(
    GLOB_RECURSE ALL_FORMAT_SOURCE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
  )

  # Target for formatting the code
  add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_FORMAT_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting all source files with clang-format..."
    VERBATIM)

  # Target to check the formatting of the code
  add_custom_target(
    check-format
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_FORMAT_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking source files formatting with clang-format..."
    VERBATIM)

  set_target_properties(format PROPERTIES FOLDER "Maintenance")
  set_target_properties(check-format PROPERTIES FOLDER "Maintenance")

  message(STATUS "clang-format targets 'format' and 'check-format' created.")
endif()

# ---- cppcheck ----

find_program(CPPCHECK_EXE "cppcheck")
if(NOT CPPCHECK_EXE)
  message(
    WARNING "cppcheck not found. Target 'run-cppcheck' will not be created.")
else()
  add_custom_target(
    run-cppcheck
    COMMAND ${CPPCHECK_EXE} --enable=all --std=c17
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cppcheck static analysis..."
    VERBATIM)
  add_dependencies(run-cppcheck ${PROJECT_NAME})
  set_target_properties(run-cppcheck PROPERTIES FOLDER "Maintenance")
  message(STATUS "Cppcheck target 'run-cppcheck' created.")
endif()
