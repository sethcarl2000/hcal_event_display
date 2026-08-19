
#include <UserWindow.hpp>
#include <EventDisplayKernel.hpp>
//ROOT headers
#include <TError.h> 
//stdlib headers
#include <algorithm> 

//__________________________________________________________________________________________________________________________
void UserWindow::DrawObjects(Frequency::Type type)
{
#ifdef DEBUG
    Info(__func__, "in body. fetching kernel...");
#endif
    //check what state the app is in. if it's in the 'init' state, we need to run all 'draw' functions (to collect all necessary data)
    auto& kernel = EventDisplayKernel::Instance(); 
    auto app_state = kernel.GetAppState(); 

    fCurrentDrawFunctionFrequency = type;

    //set ourselves as the exclusive 'current' user window 
    this->cd(); 

    const auto& list_of_draw_functions = (type == Frequency::Type::kEachEvent) 
        ? fDrawFunctions_event 
        : fDrawFunctions_timestep;

    auto& primitive_list = (type == Frequency::Type::kEachEvent)
        ? fPrimitiveList_event 
        : fPrimitiveList_timestep; 

    primitive_list.Delete(); 

    switch (app_state) {

        //if we're in the 'init' state, we need to loop over all 'draw' events, to see which branches we need. 
        case EventDisplayKernel::AppState::kInit : {
#ifdef DEBUG
            Info(__func__, "App state is 'init'"); 
#endif  
            for (const auto& draw_fcn : list_of_draw_functions) {
#ifdef DEBUG
                Info(__func__, "<window: %s>: Attempting to draw event-update: %s", GetName().c_str(), draw_fcn.name.c_str()); 
#endif          
                fCurrentDrawFunctionName = draw_fcn.name; 
                draw_fcn(); 
#ifdef DEBUG
                Info(__func__, "<window: %s>: done.", GetName().c_str()); 
#endif 
            }

#ifdef DEBUG
            Info(__func__, "leaving body");
#endif
            fCurrentDrawFunctionName = "none";
            return; 
        }
        
        case EventDisplayKernel::AppState::kActive : {
#ifdef DEBUG
            Info(__func__, "App state is 'active'"); 
#endif
            //check if this window is active. 
            if (!IsActive()) return; 

            auto canvas = fApp->GetCanvas(); 
            if (!canvas) {
                Error(__func__, "Canvas for user app is null.");
                std::exit(1);
            }
            kernel.SetCanvas(fMyKey, canvas);

            for (const auto& draw_fcn : list_of_draw_functions) {
#ifdef DEBUG
                Info(__func__, "<window: %s>: Attempting to draw update: %s", GetName().c_str(), draw_fcn.name.c_str()); 
#endif
                fCurrentDrawFunctionName = draw_fcn.name; 
                draw_fcn(); 
#ifdef DEBUG
                Info(__func__, "<window: %s>: done.", GetName().c_str()); 
#endif 
            }

            //tell the canvas to update, to relfect our changes
            canvas->Modified(); 
            canvas->Update(); 
#ifdef DEBUG
            Info(__func__, "leaving body");
#endif
            fCurrentDrawFunctionName = "none";
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
    DoDeactivate(); 

    //wipe the list of primitives on the pad 
    fPrimitiveList_event.Delete();
    fPrimitiveList_timestep.Delete();  
}
//__________________________________________________________________________________________________________________________
void UserWindow::DoActivate(Key<EventDisplayKernel>)
{
    if (!IsActive()) {

        //initialize the app
        fApp = new UserApp(fMyKey, this, gClient->GetRoot(), fWidth, fHeight);
    }
}

//__________________________________________________________________________________________________________________________
void UserWindow::DoDeactivate()
{
    if (fApp) { 
        fApp->DeleteWindow(); 
        fApp = nullptr; 
    }
    auto& kernel = EventDisplayKernel::Instance();
    kernel.SetWindowStatus();
}
//__________________________________________________________________________________________________________________________
void UserWindow::cd()
{
    //get access to the kernel
    auto& kernel = EventDisplayKernel::Instance();

    //set this as the active user window
    kernel.SetUserWindow(fMyKey, this);

    //set our canvas as the active canvas; 
    if (EventDisplayKernel::Instance().GetAppState() == EventDisplayKernel::AppState::kActive && IsActive()) {
        auto canv = fApp->GetCanvas(); 
        canv->cd(); 
        kernel.SetCanvas(fMyKey, canv);
    } else { 
        kernel.SetCanvas(fMyKey, nullptr); 
    }
}
//__________________________________________________________________________________________________________________________
void UserWindow::AddDrawnItem(Key<EventDisplayKernel>, DrawFunction draw_function, Frequency::Type type)
{
    auto& draw_function_list = (type == Frequency::Type::kEachEvent) ? fDrawFunctions_event : fDrawFunctions_timestep; 

    //check to make sure another item with this same name is not already present. 
    auto find_it = std::find_if(
        draw_function_list.begin(), 
        draw_function_list.end(), 
        [&draw_function](const DrawFunction& rhs){ return rhs.name == draw_function.name; }
    );
    if (find_it != draw_function_list.end()) {
        Warning(__func__, "User attepmted to add DrawFunction '%s', but this already exists in list (duplicate will not be added).", draw_function.name.c_str());
        return;  
    }

    //add this function to list of drawin functions
    draw_function_list.emplace_back( draw_function ); 

    std::printf("<window: %s>: Added drawn item '%s'.\n", GetName().c_str(), draw_function.name.c_str()); 
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