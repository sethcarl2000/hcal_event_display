#!/bin/bash

add_path_to_list () {

    my_path="${1}"
    path_list="${2}" 
    
    # loop over all paths in the list. if we find a match for the given path, then quit (we don't need to add it). 
    local found="false"
    IFS=':' read -ra path_array <<< "${path_list}" 
    for path_item in "${path_array[@]}" 
    do
        if [[ "${path_item}" == "${my_path}" ]]; then 
            found="true"
            break; 
        fi
    done

    if [[ $found == "false" ]]; then 
        if [[ -z "${path_list}" ]]; then path_list="${my_path}"; else path_list="${my_path}:${path_list}"; fi
    fi 
    echo "${path_list}"
}

export LD_LIBRARY_PATH=$(add_path_to_list "$(pwd)/build" $LD_LIBRARY_PATH)
export ROOT_INCLUDE_PATH=$(add_path_to_list "$(pwd)/include" $ROOT_INCLUDE_PATH)

