cmake_minimum_required(VERSION 3.16)

# Event display
# 
project(HCalEventDisplay VERSION 0.1 LANGUAGES CXX)

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

add_library(EventDisplay SHARED ${sources} ${headers})
target_include_directories(EventDisplay PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(EventDisplay PUBLIC ROOT::Core ROOT::RIO ROOT::Physics)

# automatically generate the 'set_edisp_env.sh' script
set(EDISP_ENV_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include")
set(EDISP_ENV_BUILD_PATH "${PROJECT_BINARY_DIR}")

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/set_edisp_env.sh.in" # input template 
    "${PROJECT_BINARY_DIR}/set_edisp_env.sh" # output script, with hard-coded paths
    @ONLY # only find-and-replace variables with the @VAR@ syntax 
)

option(DEBUG "Enable GUI debug printing" OFF)
if (DEBUG)
    target_compile_definitions(EventDisplay PRIVATE DEBUG)
endif()

root_generate_dictionary(G__EventDisplay ${headers} LINKDEF include/LinkDef.h MODULE EventDisplay)
