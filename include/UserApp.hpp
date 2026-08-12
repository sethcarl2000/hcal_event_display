#ifndef UserApp_hpp
#define UserApp_hpp

#include <TGFrame.h>
#include <functional> 

class UserWindow; 

class UserApp : public TGMainFrame {
private: 

    //parent class 
    UserWindow* fParent; 

public: 
    UserApp(
        UserWindow* parent, 
        const TGWindow* ptr, 
        UInt_t w, UInt_t h, 
        const std::function<void(UserApp*)>& app_draw_fcn
    );

    ~UserApp(); 

    // Override of TGMainFrame's 'CloseWindow' function. We must modify it so that closing this window does not close the whole application. 
    void CloseWindow() override; 

}; 


#endif