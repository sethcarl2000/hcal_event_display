cmake_minimum_required(VERSION 3.23)

# Event display
# 
project(EventDisplayKernel VERSION 0.1.1 LANGUAGES CXX)

# Find ROOT as a package
# 
find_package(ROOT 6.20 REQUIRED COMPONENTS Core RIO Physics)

set(headers
    include/EventDisplayKernel.hpp
    include/Key.hpp
    include/Frequency_Type.hpp
    include/DrawFunction.hpp
    include/UserApp.hpp
    include/UserWindow.hpp
    include/EventGUI.hpp
    include/UserSlider.hpp
    include/UserParameter.hpp
)

set(sources
    src/EventDisplayKernel.cpp
    src/UserApp.cpp
    src/UserWindow.cpp
    src/EventGUI.cpp
    src/UserSlider.cpp
#   
#   widgets
    src/widgets/EventControlPanel.cpp
    src/widgets/ToggleWindowsPanel.cpp
    src/widgets/TimeControlPanel.cpp
    src/widgets/UserParameterPanel.cpp
)

# be explicit about the directory in which to put the '.so' shared library file
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_library(EDK SHARED ${sources} ${headers})
target_include_directories(EDK PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(EDK PUBLIC ROOT::Core ROOT::RIO ROOT::Physics)

# automatically generate the 'set_edisp_env.sh' script
set(EDK_ENV_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include")
set(EDK_ENV_BUILD_PATH "${PROJECT_BINARY_DIR}")

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/set_edisp_env.sh.in" # input template 
    "${PROJECT_BINARY_DIR}/set_edisp_env.sh" # output script, with hard-coded paths
    @ONLY # only find-and-replace variables with the @VAR@ syntax 
)

# tell the user how the can have the script auto-executed on startup;
message(STATUS 
    " \n"
    " NOTE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
    " For bash, if you want to have the EDK set up each time you login,\n"
    " copy the following line, and paste it at the end of your `~/.bashrc` script:\n\n"
    "   [ -f \"${PROJECT_BINARY_DIR}/set_edisp_env.sh\" ] && source ${PROJECT_BINARY_DIR}/set_edisp_env.sh\n\n"
    " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
)

option(DEBUG "Enable GUI debug printing" OFF)
if (DEBUG)
    target_compile_definitions(EDK PRIVATE DEBUG)
    target_compile_options(EDK PRIVATE -g)
endif()

root_generate_dictionary(G__EDK ${headers} LINKDEF include/LinkDef.h MODULE EventDisplay)
