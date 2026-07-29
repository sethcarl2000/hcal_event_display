#!/bin/bash

echo "activating mamba env" 

mamba activate analyzer-env

# check if the optics libs are in the right paths
if [[ ! -x "/home/$(whoami)/.local/bin/add-path-to-list" ]]
then    
    echo "<${0}>: Error: script 'add-path-to-list' not found in '/home/$(whoami)/.local/bin'." >&2 
    echo "<${0}>: It can be added wtih the command: " >&2
    echo "<${0}>:   ln -s \"$(pwd)/scripts/add-path-to-list\" \"/home/$(whoami)/.local/bin/.\"" >&2 
    exit 
fi

# add our build file to the list of libs (LD_LIBRARY_PATH)
source add-path-to-list "$(pwd)/build" LD_LIBRARY_PATH

# add our src directory to the list of 'include' paths for ROOT to look for
source add-path-to-list "$(pwd)/include" ROOT_INCLUDE_PATH
