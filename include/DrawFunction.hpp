#ifndef DrawFunction_hpp
#define DrawFunction_hpp

#include <functional> 
#include <string> 

struct DrawFunction {
    std::string name{}; 
    std::function<void(void)> fcn{[](){}}; 
    bool is_active{false}; 
    
    inline void operator()(void) const { fcn(); }
};

//this lets us check if two structs are the same 
inline bool operator==(const DrawFunction& lhs, const DrawFunction& rhs) { return lhs.name == rhs.name; }

#endif