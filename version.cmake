set(tasmomanager_version "v1.0.0")

find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --tags
        RESULT_VARIABLE tasmomanager_git_describe_result
        OUTPUT_VARIABLE tasmomanager_git_describe_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(tasmomanager_git_describe_result EQUAL 0)
        set(tasmomanager_version "${tasmomanager_git_describe_output}")
    endif()
endif()

message(STATUS "Building Tasmomanager version ${tasmomanager_version}...")

configure_file("${INPUT_FILE}" "${OUTPUT_FILE}")