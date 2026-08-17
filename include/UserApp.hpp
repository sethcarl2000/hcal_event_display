#ifndef UserApp_hpp
#define UserApp_hpp

#include <DrawFunction.hpp>
#include <Key.hpp>
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

    /// this is the class' 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<UserApp> fMyKey{}; 

    //parent class 
    UserWindow* fParent; 

    TRootEmbeddedCanvas *fECanvas; 

public: 

    // Default constructor. We add a 'kernel key' as an argument,
    // because it's an object that only the kernel can create. 
    UserApp(
        Key<UserWindow>,
        UserWindow* parent, 
        const TGWindow* ptr, 
        UInt_t w, UInt_t h
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