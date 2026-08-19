
#include <UserApp.hpp> 
#include <UserWindow.hpp>
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
UserApp::UserApp(Key<UserWindow>, UserWindow* parent, const TGWindow* ptr, UInt_t w, UInt_t h)
    : TGMainFrame(ptr, w, h), fParent{parent}
{    

    //let's do some dummy checks
    if (fParent == nullptr) {
        Error(__func__, "User app constructor called with 'null' parent ptr."); 
        std::exit(1); 
    }

    //set cleanup so all widgets the user allocates will be cleaned up. 
    SetCleanup(kDeepCleanup); 

    const auto window_name = parent->GetName().c_str(); 

    //auto hframe = new TGHorizontalFrame(this, w, h);
    //

    fECanvas = new TRootEmbeddedCanvas(parent->GetName().c_str(), this, w, h);

    //draw an empty frame to make sure the canvas shows up 
    fECanvas->GetCanvas()->cd()->DrawFrame(0,0,1,1);

    AddFrame(fECanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,2,2,2)); 
    
    //hframe->AddFrame(fECanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5)); 
    
    //check to make sure the user defined an embedded canvas! 
    if (fECanvas == nullptr) {
        Error(__func__, "<UserWindow: %s>: In user-supplied app draw function, a TRootEmbeddedCanvas was not defined, or its ptr was not assigned correctly", window_name);
        std::exit(1);
        return; 
    }

    //AddFrame()

    //set the name of this app 
    SetWindowName(window_name);

    //map the windows / subwindows 
    MapWindow(); 
    Resize(GetDefaultSize()); 
    MapSubwindows(); 

#ifdef DEBUG
    Info(__func__, "<UserWindow: %s>: Exiting app constructor", window_name); 
#endif
}   
//__________________________________________________________________________________________________________________________
void UserApp::CloseWindow()
{
#ifdef DEBUG
    Info(__func__, "<window: %s>: called. deleting window...", fParent ? fParent->GetName().c_str() : "null");
#endif
    DeleteWindow();
}
//__________________________________________________________________________________________________________________________
UserApp::~UserApp() 
{ 
#ifdef DEBUG
    Info(__func__, "<window: %s>: destructor called.", fParent ? fParent->GetName().c_str() : "null");
#endif
    if (fParent) {
        fParent->DoDeactivate(); 
    } else {
        Error(__func__, "UserApp destructor called with null parent ptr."); 
        std::exit(1);
        return; 
    }
    Cleanup(); 
}
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