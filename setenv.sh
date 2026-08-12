#!/bin/bash

echo "activating mamba env" 

conda activate analyzer-env

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

export CMAKE_PREFIX_PATH="${CONDA_PREFIX}"

# check if the optics libs are in the right paths
if [[ -x "/home/$(whoami)/.local/bin/add-path-to-list" ]]
then 
    # add our build file to the list of libs (LD_LIBRARY_PATH)
    source add-path-to-list "$(pwd)/build" LD_LIBRARY_PATH

    # add our src directory to the list of 'include' paths for ROOT to look for
    source add-path-to-list "$(pwd)/include" ROOT_INCLUDE_PATH

else    
    #echo "<${0}>: Error: script 'add-path-to-list' not found in '/home/$(whoami)/.local/bin'." >&2 
    #echo "<${0}>: It can be added wtih the command: " >&2
    #echo "<${0}>:   ln -s \"$(pwd)/scripts/add-path-to-list\" \"/home/$(whoami)/.local/bin/.\"" >&2 

    export LD_LIBRARY_PATH=$(add_path_to_list "$(pwd)/build" $LD_LIBRARY_PATH)
    export ROOT_INCLUDE_PATH=$(add_path_to_list "$(pwd)/include" $ROOT_INCLUDE_PATH)
fi 
