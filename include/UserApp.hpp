#ifndef UserApp_hpp
#define UserApp_hpp

#include <AppDrawFunction.hpp>
#include <DrawFunction.hpp>
//TGUI headers
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h> 
//ROOT headers
#include <TCanvas.h> 
#include <TObject.h> 
//stdlib headers
#include <functional> 

class UserWindow; 

class UserApp : public TGMainFrame {
private: 

    //parent class 
    UserWindow* fParent; 

    TRootEmbeddedCanvas *fECanvas; 

public: 
    UserApp(
        UserWindow* parent, 
        const TGWindow* ptr, 
        UInt_t w, UInt_t h, 
        const AppDrawFunction& app_draw_fcn
    );

    ~UserApp();     

    // get the canvas on which objects are drawn in this sub-window. 
    TCanvas* GetCanvas(); 

    /// @return ptr to parent 'UserWindow' 
    inline UserWindow* GetParent() { return fParent; } 

    std::string GetWindowName();

    // Override of TGMainFrame's 'CloseWindow' function. We must modify it so that closing this window does not close the whole application. 
    void CloseWindow() override; 

}; 


#endif