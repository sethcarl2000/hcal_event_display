cmake_minimum_required(VERSION 3.16)

# Event display
# 
project(HCalEventDisplay VERSION 0.1 LANGUAGES CXX)

# Find ROOT as a package
# 
find_package(ROOT 6.20 REQUIRED COMPONENTS Core RIO Physics)


set(headers
    include/EventDisplayKernel.hpp
)

set(sources
    src/EventDisplayKernel.cpp
)

add_library(EventDisplay SHARED ${sources} ${headers})
target_include_directories(EventDisplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(EventDisplay PUBLIC ROOT::Core ROOT::RIO ROOT::Physics)

root_generate_dictionary(
    G__EventDisplay ${headers} 
    MODULE EventDisplay
    LINKDEF include/LinkDef.h 
)
