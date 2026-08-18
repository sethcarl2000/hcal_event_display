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
#    include/PrivateMessenger.hpp
)

set(sources
    src/EventDisplayKernel.cpp
    src/UserApp.cpp
    src/UserWindow.cpp
    src/EventGUI.cpp
#   
#   widgets
    src/widgets/EventControlPanel.cpp
    src/widgets/ToggleWindowsPanel.cpp
#    src/PrivateMessenger.cpp
)

add_library(EventDisplay SHARED ${sources} ${headers})
target_include_directories(EventDisplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(EventDisplay PUBLIC ROOT::Core ROOT::RIO ROOT::Physics)

# root_generate_dictionary(
#    G__EventDisplay ${headers} 
#    MODULE EventDisplay
#    LINKDEF include/LinkDef.h 
#)

option(DEBUG "Enable GUI debug printing" ON)
if (DEBUG)
    target_compile_definitions(EventDisplay PRIVATE DEBUG)
endif()

root_generate_dictionary(G__EventDisplay ${headers} LINKDEF include/LinkDef.h MODULE EventDisplay)
