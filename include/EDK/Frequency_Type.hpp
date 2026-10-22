#ifndef EDK_Frequency_enum_hpp
#define EDK_Frequency_enum_hpp

namespace EDK 
{

    /// @brief The frequency with which events should be drawn. 
namespace Frequency {
    enum class Type {
        kEachEvent,
        kEachTimeStep
    }; 
};

}

#endif