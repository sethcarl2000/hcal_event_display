#ifndef AppDrawFunction_hpp
#define AppDrawFunction_hpp

//TGUI headers
#include <TRootEmbeddedCanvas.h> 
//stdlib headers
#include <functional> 

class UserApp; 

struct AppDrawFunction { 
    std::function<void(UserApp*,TRootEmbeddedCanvas*)> fcn;
    bool is_active{false};

    inline void operator()(UserApp* p1,TRootEmbeddedCanvas* p2) const { fcn(p1,p2); } 
}; 



#endif