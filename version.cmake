set(tasmomanager_version "v1.0.0")

set(tasmomanager_github_sha "$ENV{GITHUB_SHA}")
if (tasmomanager_github_sha)
    get_filename_component(tasmomanager_github_ref "$ENV{GITHUB_REF}" NAME)
    string(SUBSTRING "${tasmomanager_github_sha}" 0 8 tasmomanager_github_sha)
    set(tasmomanager_version "${tasmomanager_version}-g${tasmomanager_github_sha}-${tasmomanager_github_ref}")
else()
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
endif()