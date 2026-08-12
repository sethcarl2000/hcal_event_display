#ifndef UserWindow_hpp
#define UserWindow_hpp

 
#include <DrawFunction.hpp>
#include <UserApp.hpp> 
//TGUI headers
#include <TGFrame.h>
// stdlib headers
#include <functional> 

class UserWindow {
private: 

    //'name', which uniquely identifies this window. 
    std::string fName; 

    //the 'app' this window actually owns
    UserApp* fApp; 

    // this is the user-supplied function which is responsible for actually drawing the user-defined window. 
    std::function<void(UserApp*)> fDrawAppFunction; 

    UInt_t fWidth{800}, fHeight{500}; 

public: 

    UserWindow(const std::string& name, const std::function<void(UserApp*)>& draw_app_fcn); 

    ~UserWindow(); 

    /// @brief This signal is how the app notifies the UserWindow parent that its 'CloseWindow' signal has been emitted.  
    void WindowClosed(); 

    /// @brief Draw the window, if it isn't already drawn. 
    void DrawWindow(); 


    /// @return 'true' if app is active, 'false' otherwise. 
    bool IsActive() const; 

    inline std::string GetName() const { return fName; } 
};

// equivalence operator for two 'UserWindow' instances 
inline bool operator==(const UserWindow& lhs, const UserWindow& rhs) { return lhs.GetName() == rhs.GetName(); } 

#endif