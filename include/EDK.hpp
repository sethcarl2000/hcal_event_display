#ifndef EDK_hpp
#define EDK_hpp

#include <EDK/Singleton.hpp>

namespace EDK 
{
//this namespace wraps the entire EDK library, to avoid polluting any global namespace in the projects you import it to. 

inline Singleton& Instance() { 
    return Singleton::Instance(); 
}

}

#endif