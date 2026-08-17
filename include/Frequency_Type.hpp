#ifndef Frequency_enum_hpp
#define Frequency_enum_hpp

/// @brief The frequency with which events should be drawn. 
namespace Frequency {
    enum class Type {
        kEachEvent,
        kEachTimeStep
    }; 
};

#endif