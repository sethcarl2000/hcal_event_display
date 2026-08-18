
#include <EventDisplayKernel.hpp>
#include <EventGUI.hpp> 
// ROOT headers
#include <TError.h> 
#include <TObjArray.h>
#include <TObject.h>
#include <TBranch.h>
#include <TString.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TClass.h>
#include <TDataType.h> 
#include <TROOT.h>
#include <TString.h> 
#include <TCanvas.h> 
// TGUI headers
#include <TGClient.h> 
#include <TGLayout.h> 
#include <TApplication.h> 
// stdlib headers
#include <stdexcept> 
#include <cstdlib> 
#include <cstdio> 
#include <map> 
#include <algorithm> 

#ifdef DEBUG
constexpr char classname[] = "EventDisplayKernel"; 
#endif

namespace inst_helpers {
    template<typename T> std::string GetBranchCode(); 
}

//________________________________________________________________________________________________
//Access to single, static instance 
EventDisplayKernel& EventDisplayKernel::Instance() { 
    static EventDisplayKernel instance; 
    return instance; 
}
//________________________________________________________________________________________________
EventDisplayKernel::EventDisplayKernel()
{
#ifdef DEBUG
    std::cout << "in <"<<classname<<"> in constructor.\n"; 
#endif
    //set the app state
    fAppState = AppState::kNone; 
}
//________________________________________________________________________________________________
void EventDisplayKernel::LaunchApp()
{
#ifdef DEBUG
    std::cout << "in <"<<classname<<"::"<<__func__<<"> in body.\n"; 
#endif
    //try and open the TFile
    try {
        
        fDataFrame = std::make_unique<ROOT::RDataFrame>(fTreeName, fFilePath); 

        if (!fDataFrame) {
            throw std::runtime_error("in <LaunchApp>: Dataframe failed to be constructed."); 
        }

        //get list of column names
        fBranchList = fDataFrame->GetColumnNames(); 
        
    } catch (const std::exception& e) {
        Error(__func__, "Something went wrong trying to open the file / tree.\n"
            " file:     %s\n"
            " tree:     %s\n"
            " what():   %s", fFilePath.c_str(), fTreeName.c_str(), e.what()); 
        std::exit(1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  This is the 'init' phase of the app. We have several things on our to-do list. 
    // 
    //    1. We need to loop over all User apps, and do a 'dry-run' of all their draw functions. 
    //       this will let us know what branches we need to make copies of from the tree. 
    //
    //    2. Next, we're going to (try) to open the user-proivded TTree, and make copies of all the data
    //       the user requested. 
    //  
    fAppState = AppState::kInit; 

    DrawCurrentEvent(); 
    std::cout << "done evaluating all draw functions.\n"; 
    std::cout << "Looping over DataFrame..." << std::flush;
    
    size_t n_events; 

    // let's try and get all the branches the user wants. 
    try {
        auto event_number_rptr = GetDataFrame()->Take<UInt_t>("fEvtHdr.fEvtNum"); 

        n_events = *(GetDataFrame()->Count()); 

        fEventNumbers = *event_number_rptr; 

    } catch (const std::exception& e) {

        Error(__func__, "Something went wrong trying to collect data from the RDataFrame.\n what(): %s", e.what()); 
        std::exit(1); 
        return; 
    }
    std::cout << "done.\n";

    fAppState = AppState::kActive; 

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Now, we should be ready to actually launch the GUI app. 
    //
    LaunchGUI(); 

    //activate all windows, by default. 
    for (auto& user_window : fUserWindows) user_window->DoActivate(fMyKey); 

    fEventIndex=0; 
    for (auto& user_window : fUserWindows) {
#ifdef DEBUG
        const char* const window_name = user_window->GetName().c_str(); 
#endif
        try {

#ifdef DEBUG 
            Info(__func__, "<window: %s> activating user window\n", window_name); 
#endif
            user_window->DoActivate(fMyKey);      
#ifdef DEBUG 
            Info(__func__, "<window: %s> drawing event draw functions...\n", window_name); 
#endif
            user_window->DoDrawEvent(fMyKey);
#ifdef DEBUG 
            Info(__func__, "<window: %s> drawing timestamp draw functions...\n", window_name); 
#endif
            user_window->DoDrawTimestamp(fMyKey);

        } catch (const std::exception& e) {
            
            Error(__func__, "Something went wrong trying draw window / object: %s / %s.\n what(): %s", 
                user_window->GetName().c_str(),
                user_window->GetCurrentDrawFunctionName().c_str(), 
                e.what()
            );
            std::exit(1);
        }
#ifdef DEBUG 
        Info(__func__, "<window: %s> done.\n", window_name); 
#endif
    }
    // do stuff that actually launches the GUI app...   
    // 
    // 
    // ...
}
//________________________________________________________________________________________________
ROOT::RDataFrame* EventDisplayKernel::GetDataFrame() 
{
    //first, check if the dataframe is null 
    if (!fDataFrame) {
        Error(__func__, "Tried to access dataframe, but it's null / not open");
        std::exit(1); 
        return nullptr;  
    }
    return fDataFrame.get(); 
}
//________________________________________________________________________________________________
template<typename T> T EventDisplayKernel::GetData(std::string branch_name) 
{
    //this is a cointainer for all our data of type 'T'. 
    // each event is a single entry in a vector, 
    // and this vector itself is an 'RResultPtr'. This lets us 'request' this data from the RDataFrame before we run it over all the data. 
    // then, in one fell swoop, we will fill all requested branches, making sure that we dont make duplicate copies of data (if multiple drawing routines request it). 
    static std::map<std::string, ROOT::RDF::RResultPtr<std::vector<T>>> fBranches; 

    // we need to be able to return a 'default constructed' version of this object (i.e. empty vector, or '0' if its an int / float)
    static_assert(std::is_default_constructible_v<T>, "Template arg 'T' is not default constructable");

    //first, check the status of the app. 
    switch (fAppState) {
        case AppState::kNone : { return T{}; break; }

        case AppState::kActive : { // We're in the 'run' phase ------------------------------------------------------------------------------

            auto find_it = fBranches.find(branch_name);
            if (find_it != fBranches.end()) {

                //we've found our value. return it: 
                ROOT::RDF::RResultPtr<std::vector<T>>& rptr = find_it->second; 

                //do a quick range check 
                size_t n_events = rptr->size();
                if (GetEventIndex() >= n_events) {
                    Error("GetData(run phase)", "Requested event index %zi, but last event index is %zi.", GetEventIndex(), n_events); 
                    std::exit(1); 
                    return T{};
                }

                //if we've passed all these checks, then actually draw the events. 
                return (*rptr)[GetEventIndex()]; 

            } else {

                Error(  "GetData(run phase)", "Requested branch '%s' appears in tree, but not in app's list of branches.\n"
                        "   Current window / object: %s / %s",
                    branch_name.c_str(),
                    fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
                ); 
                std::exit(1); 
                return T{};
            }
            break; 
        }

        //we're in the initialization phase. -----------------------------------------------------------------------------------------------
        case AppState::kInit : {

            //check if the TTree is ok 
            if (!GetDataFrame()) {
                Error("GetData(init phase)", "DataFrame is null.");
                std::exit(1);  
                return T{};
            }

            //check if branch exists
            if (!DoesBranchExist(branch_name)) {
                Error("GetData(init phase)", "Branch '%s' does not exist in tree '%s'\n"
                "   Current window / object: %s / %s\n", 
                branch_name.c_str(), fTreeName.c_str(),
                fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
            ); 
                std::exit(1);  
                return T{};
            }

            //std::cout << "branch: '" << branch_name << "' type: '" << GetDataFrame()->GetColumnType(branch_name) 
            //    << "' template type: '" << TClass::GetClass(typeid(T))->GetName() << "'\n"; 

            //check to make sure the branch type matches the type given. 
            std::string branch_type = GetDataFrame()->GetColumnType(branch_name); 
            if (!BranchTypeMatches(branch_name, typeid(T))) {

                const char* requested_class_name=nullptr; 

                
                if (auto my_type = TClass::GetClass(typeid(T))) {

                    requested_class_name = my_type->ClassName();
                }
                if (auto my_datatype = TDataType::GetDataType( TDataType::GetType(typeid(T)))) {

                    requested_class_name = my_datatype->GetTypeName().Data(); 
                } 


                Error(  
                    "GetData(init phase)", "Branch '%s' is reported as having type '%s', but this function was invoked with type '%s'.\n"
                    "   Current window / object: %s / %s",
                    branch_name.c_str(), 
                    branch_type.c_str(), 
                    (requested_class_name != nullptr ? requested_class_name : "null"),
                    fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
                );
                std::exit(1); 
                return T{};
            }; 

            //check to see if this branch has already been requested 
            auto find_it = fBranches.find(branch_name); 
            if (find_it == fBranches.end()) {

                //add a new element to the map corresponding to this branch. 
                //auto rptr = GetDataFrame()->Take<ROOT::VecOps::RVec<T>>(branch_name);
                fBranches.insert({ branch_name, GetDataFrame()->Take<T>(branch_name) });

                //auto& rptr = fBranches[branch_name]; 
                //rptr = std::move( GetDataFrame()->Take<ROOT::VecOps::RVec<T>>(branch_name) );
                //auto rptr = ;
                //fBranch_vec.push_back( GetDataFrame()->Take<T>(branch_name) );


                //let the user know this branch was added successfully 
                std::printf("in <EventDisplayKernel::GetData(init phase)>: Requested branch '%s' added. \n", branch_name.c_str());
            }

            break; 
        } 

    }
    return T{}; 
}
//________________________________________________________________________________________________
bool EventDisplayKernel::DoesBranchExist(std::string branch_name) const 
{
    for (const auto& branch : fBranchList) {
        if (branch_name == branch) { return true; }
    }
    return false; 
}
//________________________________________________________________________________________________

//________________________________________________________________________________________________
bool EventDisplayKernel::BranchTypeMatches(std::string branch_name, const std::type_info& type)
{
    ROOT::RDataFrame *df; 
    if (!(df = GetDataFrame())) {
        Error(__func__, "Dataframe is null"); 
        std::exit(1); 
    }   

    if (!DoesBranchExist(branch_name)) {
        Error(__func__, "Branch '%s' does not exist in RDataFrame.", branch_name.c_str());
        return false; 
    }

    //Claude Fable showed me how to do this 

    std::string branch_type_str = df->GetColumnType(branch_name); 
    
    // if the type passed is a ROOT type, then we can compare it & the column this way: 
    if (TClass *expected_type = TClass::GetClass(type)) {
    
        // the GetClass() method return nullptr if it's not a ROOT type
        //std::cout << "Input branch: '" << branch_name << "' with type: '" << branch_type_str << "', with class: '" << TClass::GetClass(branch_type_str.c_str()) << "'\n";
        //std::cout << "TClass found: '" << expected_type->GetName() << "'. match? " << (expected_type == TClass::GetClass(branch_type_str.c_str()) ? "yes" : "no") << "\n"; 
        
        return (TClass::GetClass(branch_type_str.c_str()) == expected_type); 
    }

    // if not, we use TDataType for fundamental types: 
    TDataType* data_type = gROOT->GetType(branch_type_str.c_str()); 
    if (!data_type) {
        Error(__func__, "Branch '%s' with type '%s' could not be converted to TDataType enum", branch_name.c_str(), branch_type_str.c_str());
        return false; 
    }

    return (data_type->GetType() == TDataType::GetType(type));  
}
//________________________________________________________________________________________________
void EventDisplayKernel::Draw(TObject* object, const char* option)
{
    if (fCurrentUserWindow == nullptr) {
        Warning(__func__, 
            "Cannot draw object, current user window is null.\n"
            "   Current window / object: %s / %s\n", 
            fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
        ); 
        return; 
    }

    if (!object) {
        Warning(__func__, 
            "Object called to be drawn is nullptr, and thus cannot be drawn. was it properly initailzied?\n"
            "   Current window / object: %s / %s",
            fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
        ); 
        return; 
    }

    //set ownership over this object (so that ROOT doesn't have to manage it itself).
    object->SetBit(kMustCleanup); 
    object->ResetBit(kCanDelete); 

    // check to see if this is a histogram (in ROOT, all histogram types inherit from TH1)
    // if so, make sure that we have ownership of it (we control when it is deleted). 
    if (object->IsA()->InheritsFrom( TClass::GetClass<TH1>() )) {

        auto th1 = dynamic_cast<TH1*>(object); 
        if (th1 == nullptr) {
            //dynamic cast failed, for some reason: 
            Error(__func__, 
                "Dynamic cast of TObject* to TH1* failed, even though this object inherits from TH1 (and thus this should have succeeded).\n"
                "   Current window / object: %s / %s",
                fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str()
            ); 
            std::exit(1);
        }
        th1->SetDirectory(nullptr); 
    }

    if (fAppState == AppState::kActive) {
        
        fCurrentUserWindow->AddPrimitive(fMyKey, object); 

        auto canv = GetCanvas(); 
        canv->cd(); 
        object->Draw(option); 
        return; 
    
    } else {

        delete object; 
    }
}
//________________________________________________________________________________________________
size_t EventDisplayKernel::FindEventIndex(UInt_t event_number)
{
    auto find_it = std::find_if(fEventNumbers.begin(), fEventNumbers.end(), [event_number](UInt_t lhs){ return lhs == event_number; }); 

    if (find_it == fEventNumbers.end()) {
        //we did NOT find the event number
        Warning(__func__, "Event number %u does not exist in list of events.", event_number); 
        return GetEventIndex(); 
    }

    return (*find_it); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::LaunchGUI()
{
    if (fGUI) {
        Error(__func__, "Function invoked even though GUI object is already allocated.");
        std::exit(1);
        return; 
    }
    
    //get list of all windows
    std::vector<std::string> window_names; 
    window_names.reserve(fUserWindows.size());

    for (const auto& window : fUserWindows) window_names.emplace_back(window->GetName());

    fGUI = new EventGUI(fMyKey, gClient->GetRoot(), 500, 400, window_names);
}

//________________________________________________________________________________________________
TCanvas* EventDisplayKernel::GetCanvas()
{
    if (fAppState != AppState::kActive) {
        Error(__func__, "GetCanvas() invoked in inactive state. (No canvas exists!)");
        return nullptr;
    }

    if (fCurrentCanvas) return fCurrentCanvas; 

    Error(__func__, "Current canvas (%p) is null.", fCurrentCanvas); 
    std::exit(1); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DoNextEvent(Key<EventGUI>)
{
    if (fEventIndex < fEventNumbers.size()-1) {
        ++fEventIndex; 
    } else {
        Info(__func__, "Cannont load next event; %u is already the max. event in the list.", fEventNumbers.back());
    }

    DrawCurrentEvent(); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DoPrevEvent(Key<EventGUI>)
{
    if (fEventIndex > 0) {
        --fEventIndex; 
    } else {
        Info(__func__, "%u is the min. event in the list", fEventNumbers.front());
    }

    DrawCurrentEvent(); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DrawEventIndex(size_t index)
{
    if (index >= fEventNumbers.size()) {
        Error(__func__, "illegal event index: %zi (valid range is 0-%zi).", index, fEventNumbers.size()-1);
        std::exit(1);
        return; 
    }

    fEventIndex = index; 
    fEventNumber = fEventNumbers[index];
    
    std::cout << "drawing event "<<fEventNumber<<"..." << std::flush; 
    DrawCurrentEvent(); 
    std::cout << "done.\n" << std::flush; 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DrawCurrentEvent()
{
    std::printf("drawing event %u...", fEventNumber);


    //run each (active) event-drawing function (in order!)
    /*for (const auto& draw_function : fDrawFunctions) {
        if(draw_function.is_active) { draw_function.fcn(); } 
    }*/ 
    //loop over all drawn functions, to register each of the requested branches
    for (auto& user_window : fUserWindows) {

#ifdef DEBUG
        const char* const window_name = user_window->GetName().c_str(); 
#endif
        //if the winow is inactive, skip it. 
        if (fAppState == AppState::kActive) {
            if (!user_window->IsActive()) {
#ifdef DEBUG
                Info(__func__, "UserWindow %s is inactive, skipping it.", window_name); 
#endif        
                continue;      
            } 
        }

        try {
#ifdef DEBUG
            Info(__func__, "Trying to draw UserWindow: %s (Event draw routine)", window_name); 
#endif
            //tell this user window that its canvas is the active one. 
            // this is important, because users may ask the kernel for access to the current canvas, and this method
            // makes sure that they are given the correct one corresponding to this window. 
            user_window->DoDrawEvent(fMyKey);

#ifdef DEBUG
            Info(__func__, "Trying to draw UserWindow: %s (Timestamp draw routine)", window_name); 
#endif
            //std::printf("in <EventDisplayKernel::%s>: attempting to evaluate DrawFunction '%s'...\n", __func__, fCurrentDrawFunction.c_str()); 

            user_window->DoDrawTimestamp(fMyKey); 

        } catch (const std::exception& e) {

            Error(__func__, 
                "Exception caught drawing window.\n"
                "   Current window / object: %s / %s"
                "\n what(): %s", 
                fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str(), e.what()); 
            std::exit(1); 
            return; 
        }
    }

}
//________________________________________________________________________________________________
void EventDisplayKernel::DoDrawTimestamp(Key<EventGUI>, double timestamp)
{
    fTimestamp = timestamp; 

    //run each (active) event-drawing function (in order!)
    /*for (const auto& draw_function : fDrawFunctions) {
        if(draw_function.is_active) { draw_function.fcn(); } 
    }*/ 
    //loop over all drawn functions, to register each of the requested branches
    for (auto& user_window : fUserWindows) {

#ifdef DEBUG
        const char* const window_name = user_window->GetName().c_str(); 
#endif
        //if the winow is inactive, skip it. 
        if (fAppState == AppState::kActive) {
            if (!user_window->IsActive()) {
#ifdef DEBUG
                Info(__func__, "UserWindow %s is inactive, skipping it.", window_name); 
#endif        
                continue;      
            } 
        }

        try {
#ifdef DEBUG
            Info(__func__, "Trying to draw UserWindow: %s", user_window->GetName().c_str()); 
#endif
            //tell this user window that its canvas is the active one. 
            // this is important, because users may ask the kernel for access to the current canvas, and this method
            // makes sure that they are given the correct one corresponding to this window. 
            user_window->DoDrawTimestamp(fMyKey);

#ifdef DEBUG
            Info(__func__, "Trying to draw UserWindow: %s", user_window->GetName().c_str()); 
#endif
        } catch (const std::exception& e) {

            Error(__func__, 
                "Exception caught drawing window.\n"
                "   Current window / object: %s / %s"
                "\n what(): %s", 
                fCurrentUserWindow->GetName().c_str(), fCurrentUserWindow->GetCurrentDrawFunctionName().c_str(), e.what()); 
            std::exit(1); 
            return; 
        }
    }

}
//________________________________________________________________________________________________
void EventDisplayKernel::AddDrawFunction(std::string window_name, std::string object_name, const std::function<void(void)>& fcn,  Frequency::Type type)
{
    //search through the list of window names, to find the one the user wants to add this function to. 
    UserWindow* my_window=nullptr; 
    for (auto& window : fUserWindows) {
        if (window->GetName() == window_name) { my_window = window.get(); break; }
    }

    if (my_window == nullptr) {
        Error(__func__,
            "Cannot add drawn item '%s' to window '%s'; a window with this name could not be found in the list of added windows. Make sure:\n"
            "   1. the name is spelled exactly as it was when the window was added, and\n"
            "   2. this draw function is added *after* the window is.",
            object_name.c_str(), window_name.c_str()
        );  
        std::exit(1);
    }   

    DrawFunction draw_function{ .name=object_name, .fcn=fcn, .is_active=true };

    //now, we have our window. we can add the function. 
    my_window->AddDrawnItem(fMyKey, draw_function, type);
}
//________________________________________________________________________________________________
void EventDisplayKernel::AddUserWindow(std::string window_name, UInt_t width, UInt_t height)
{
#ifdef DEBUG
    Info(__func__, "in body. adding new window '%s'", window_name.c_str()); 
#endif

    //first, let's search the list of user-defined windows, to make sure this isn't a duplicate (not allowed -- fatal error!)
    for (const auto& window : fUserWindows) {
        if (window->GetName() == window_name) {
            Error(__func__, "Tried to add new user window with name '%s', but a window with this same name has already been added to the list!", window_name.c_str());
            std::exit(1);
            return; 
        }
    }

    //now, we construct a new user-window. 
    fUserWindows.emplace_back(
        std::make_unique<UserWindow>(fMyKey, window_name, width, height)
    ); 

#ifdef DEBUG
    Info(__func__, "added new window to list"); 
#endif
}
//________________________________________________________________________________________________
void EventDisplayKernel::CloseApp(Key<EventGUI>) { gApplication->Terminate(0); };
//________________________________________________________________________________________________
void EventDisplayKernel::DoToggleWindow(Key<EventGUI>, std::string window_name, bool activate_window)
{
#ifdef DEBUG
    Info(__func__, "searching for window '%s'...", window_name.c_str());
#endif
    //search the list of windows, to make sure it exsits. 
    auto find_it = std::find_if(
        fUserWindows.begin(),
        fUserWindows.end(),
        [&window_name](const auto& window){ return window->GetName() == window_name; }
    ); 

    if (find_it == fUserWindows.end()) {
        Error(__func__,
            "Cannot toggle window '%s'; a window with this name could not be found in the list of added windows.",
            window_name.c_str()
        );  
        std::exit(1);
    }   
#ifdef DEBUG
    Info(__func__, "window found.");
#endif
    auto& my_window = *find_it; 

    // if the window is inactive AND the GUI signaled to activate it... 
    if (activate_window==true &&  my_window->IsActive()==false) {
#ifdef DEBUG
        Info(__func__, "Activating window '%s'...", window_name.c_str());
#endif
        my_window->DoActivate(fMyKey);
#ifdef DEBUG 
        Info(__func__, "Updating event picture...");
#endif
        DrawCurrentEvent(); 
#ifdef DEBUG 
        Info(__func__, "done.");
#endif
        return; 
    } 

    // if the window is active AND the GUI singnaled to deactivate it... 
    if (activate_window==false && my_window->IsActive()==true) {
#ifdef DEBUGf
        Info(__func__, "Deactivating window '%s'...", window_name.c_str());
#endif
        my_window->DoDeactivate();
#ifdef DEBUG 
        Info(__func__, "done.");
#endif
        return; 
    } 
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//explicity instantiate valid templates
template double EventDisplayKernel::GetData<double>(std::string branch_name);
//
template ROOT::VecOps::RVec<double> EventDisplayKernel::GetData<ROOT::VecOps::RVec<double>>(std::string branch_name);
//
template unsigned int EventDisplayKernel::GetData<unsigned int>(std::string branch_name);
//

template<> std::string inst_helpers::GetBranchCode<double>() { return "F"; }
