
#include <UserWindow.hpp>
#include <PrivateMessenger.hpp>
#include <EventDisplayKernel.hpp>
//ROOT headers
#include <TError.h> 
//stdlib headers
#include <algorithm> 

//__________________________________________________________________________________________________________________________
UserWindow::UserWindow(const std::string& name, const AppDrawFunction& app_draw_function)
    : fName{name}, fAppDrawFunction{app_draw_function}, fDrawFunctions{}
{   
}
//__________________________________________________________________________________________________________________________
void UserWindow::WindowClosed()
{
    //wipe the list of primitives on the pad 
    fPrimitiveList.Delete(); 
}
//__________________________________________________________________________________________________________________________
void UserWindow::DrawWindow()
{
#ifdef DEBUG
    Info(__func__, "in body. fetching kernel...");
#endif
    //check what state the app is in. if it's in the 'init' state, we need to run all 'draw' functions (to collect all necessary data)
    auto& kernel = EventDisplayKernel::Instance(); 
    auto app_state = kernel.GetAppState(); 

    switch (app_state) {

        //if we're in the 'init' state, we need to loop over all 'draw' events, to see which branches we need. 
        case EventDisplayKernel::AppState::kInit : {
#ifdef DEBUG
            Info(__func__, "App state is 'init'"); 
#endif
            DrawObjects(); 
#ifdef DEBUG
    Info(__func__, "leaving body");
#endif
            return; 
        }

        case EventDisplayKernel::AppState::kActive : {
#ifdef DEBUG
            Info(__func__, "App state is 'active'"); 
#endif

            //if the app is inactive, draw the window. 
            if (!IsActive()) fApp = new UserApp(this, gClient->GetRoot(), fWidth, fHeight, fAppDrawFunction); 

            //draw all associated objects 
            DrawObjects();
#ifdef DEBUG
    Info(__func__, "leaving body");
#endif
            return; 
        }

        default : {
            //it should not be possible for the app state to be 'none' here...
            if (app_state == EventDisplayKernel::AppState::kNone) {
                Error(__func__, "<UserWindow: %s>: Somehow, we got here even though 'EventDisplayKernel' app is in state 'None'.", GetName().c_str());
                std::exit(1); 
                return; 
            }
        }

    }
}
//__________________________________________________________________________________________________________________________
bool UserWindow::IsActive() const { return fApp != nullptr; }
//__________________________________________________________________________________________________________________________
UserWindow::~UserWindow() {

    //close & delete the app, if it's open 
    Deactivate(); 
}
//__________________________________________________________________________________________________________________________
void UserWindow::DrawObjects() 
{   
    //if this window is inactive, skip. 
    if (!IsActive()) { return; }

#ifdef DEBUG
    Info(__func__, "<UserWindow: %s>: in body", GetName().c_str()); 
#endif

    //delete all primitvies we own
    fPrimitiveList.Delete(); 
    
    //try to get access to the canvas 
    this->cd(); 

    auto& kernel = EventDisplayKernel::Instance(); 

    //true if both the app & window are active
    const bool active = (kernel.GetAppState() == EventDisplayKernel::AppState::kActive && IsActive()); 

    TCanvas* canv=nullptr; 
    
    //if the app & window are active
    if (active) {
        canv = fApp->GetCanvas(); 
        canv->Clear(); 
    }

    for (auto& obj_draw_fcn : fDrawFunctions) { 

        //don't draw deactivated objects 
        if (obj_draw_fcn.is_active == false) { 
#ifdef DEBUG
        Info(__func__, "<UserWindow: %s>: obj-draw function '%s' deactivated; not drawing.", GetName().c_str(), obj_draw_fcn.c_str()); 
#endif
            continue;            
        } 

#ifdef DEBUG
        Info(__func__, "<UserWindow: %s>: drawing obj-draw function '%s'", GetName().c_str(), obj_draw_fcn.c_str()); 
#endif
        PrivateMessenger::SetObjDrawFunctionName(obj_draw_fcn.name); 
        obj_draw_fcn(); 
    }    
    PrivateMessenger::SetObjDrawFunctionName("none"); 

    if (active) {
        canv->Modified(); 
        canv->Update(); 
    }

#ifdef DEBUG
    Info(__func__, "<UserWindow: %s>: leaving body", GetName().c_str()); 
#endif
}
//__________________________________________________________________________________________________________________________
void UserWindow::Activate()
{
    if (!IsActive()) DrawWindow(); 
}
//__________________________________________________________________________________________________________________________
void UserWindow::Deactivate()
{
    if (IsActive()) { fApp->CloseWindow(); }
}
//__________________________________________________________________________________________________________________________
void UserWindow::cd()
{
    //set our canvas as the active canvas; 
    if (EventDisplayKernel::Instance().GetAppState() == EventDisplayKernel::AppState::kActive && IsActive()) {
        auto canv = fApp->GetCanvas(); 
        canv->cd(); 
        PrivateMessenger::SetCanvas( canv ); 
    } else { 
        PrivateMessenger::SetCanvas( nullptr ); 
    }

    //set this as the active user window
    PrivateMessenger::SetUserWindow( this ); 
}
//__________________________________________________________________________________________________________________________
void UserWindow::AddDrawnItem(std::string item_name, const std::function<void(void)>& draw_function)
{
    //check to make sure another item with this same name is not already present. 
    auto find_it = std::find_if(fDrawFunctions.begin(), fDrawFunctions.end(), [item_name](const DrawFunction& rhs){ return rhs.name == item_name; });
    if (find_it != fDrawFunctions.end()) {
        Warning(__func__, "User attepmted to add DrawFunction '%s', but this already exists in list (duplicate will not be added).", item_name.c_str());
        return;  
    }

    //add this function to list of drawin functions
    fDrawFunctions.emplace_back( item_name, draw_function, true ); 

    std::printf("Added drawn item '%s'.\n", item_name.c_str()); 
}
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