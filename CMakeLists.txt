cmake_minimum_required(VERSION 3.16)

# Event display
# 
project(HCalEventDisplay VERSION 0.1 LANGUAGES CXX)

# Find ROOT as a package
# 
find_package(ROOT 6.20 REQUIRED COMPONENTS Core RIO)


set(headers
    include/EventDisplayKernel.hpp
)

set(sources
    src/EventDisplayKernel.cpp
)

root_generate_dictionary(G__EventDisplay ${headers} LINKDEF LinkDef.h)

add_library(EventDisplay SHARED ${headers} ${sources})
target_include_directories(EventDisplay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_directories(EventDisplay PUBLIC ROOT::Core ROOT::RIO)
