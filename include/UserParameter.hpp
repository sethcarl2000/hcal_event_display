#ifndef UserParameter_hpp
#define UserParameter_hpp

#include <string> 
#include <cmath> 

//basic struct containing information on a user parameter 
template <typename T> struct UserParameter {
    
    std::string name; 
    
    T min, max; 

    int n_steps; 

    std::string format; 

    //return value of 'T' bounded to the given range 
    T clamp(const T& val) {
        return std::min<T>( std::max<T>(val, min), max );
    }
}; 

#endif