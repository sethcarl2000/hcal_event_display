
#include <UserApp.hpp> 
#include <UserWindow.hpp>
#include <PrivateMessenger.hpp> 
//ROOT headers
#include <TError.h> 
//stdlib headers
#include <cstdlib>
#include <iostream> 
#include <stdexcept> 

#ifdef DEBUG
namespace {
    constexpr char classname[] = "UserApp"; 
}
#endif

//__________________________________________________________________________________________________________________________
UserApp::UserApp(UserWindow* parent, const TGWindow* ptr, UInt_t w, UInt_t h, const AppDrawFunction& app_draw_fcn)
    : TGMainFrame(ptr, w, h), fParent{parent}
{
    //let's do some dummy checks
    if (fParent == nullptr) {
        Error(__func__, "User app constructor called with 'null' parent ptr."); 
        std::exit(1); 
    }

    const auto window_name = parent->GetName().c_str(); 

    //now, attempt to construct the window 
    try {

#ifdef DEBUG
        Info(__func__, "<UserWindow: %s>: In UserApp constructor. Attempting execution of construction function...", window_name); 
#endif  
        PrivateMessenger::SetAppDrawFunctionName(fParent->GetName()); 
        app_draw_fcn(this, fECanvas); 
        PrivateMessenger::SetAppDrawFunctionName("none"); 

    } catch (const std::exception& e) {

        Error(__func__, "<UserWindow: %s>: Exception caught attempting execution of App-Draw function. what() %s", window_name, e.what()); 
        std::exit(1);
        return;  
    }

    //check to make sure the user defined an embedded canvas! 
    if (fECanvas == nullptr) {
        Error(__func__, "<UserWindow: %s>: In user-supplied app draw function, a TRootEmbeddedCanvas was not defined, or its ptr was not assigned correctly", window_name);
        std::exit(1);
        return; 
    }

    //set the name of this app 
    SetWindowName(window_name);

    //map the windows / subwindows 
    MapWindow(); 
    Resize(GetDefaultSize()); 
    MapSubwindows(); 

    //set cleanup so all widgets the user allocates will be cleaned up. 
    SetCleanup(kDeepCleanup); 

#ifdef DEBUG
    Info(__func__, "<UserWindow: %s>: Exiting app constructor", window_name); 
#endif
}   
//__________________________________________________________________________________________________________________________
void UserApp::CloseWindow()
{
    //notify the parent that this app has been closed
    fParent->WindowClosed(); 

    //delete this app 
    DeleteWindow(); 
}
//__________________________________________________________________________________________________________________________
UserApp::~UserApp() { Cleanup(); }
//__________________________________________________________________________________________________________________________
TCanvas* UserApp::GetCanvas()
{
    if (!fECanvas || !fParent) {
        Error(__func__, "Either canvas or parent is null."); 
        std::exit(1);
        return nullptr; 
    }
    return fECanvas->GetCanvas(); 
}
//__________________________________________________________________________________________________________________________
std::string UserApp::GetWindowName() { return (fParent == nullptr) ? "null" : fParent->GetName(); }
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________________