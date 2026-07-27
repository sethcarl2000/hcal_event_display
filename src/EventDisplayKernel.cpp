#include <EventDisplayKernel.hpp>
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

namespace inst_helpers {
    template<typename T> std::string GetBranchCode(); 
}

//________________________________________________________________________________________________
EventDisplayKernel::EventDisplayKernel()
{
    //set the app state
    fAppState = AppState::kNone; 
}
//________________________________________________________________________________________________
void EventDisplayKernel::LaunchApp()
{
    //try and open the TFile
    try {
        
        fDataFrame = std::make_unique<ROOT::RDataFrame>(fTreeName, fFilePath); 
        
    } catch (const std::exception& e) {
        Error(__func__, "Something went wrong trying to open the file / tree.\n"
            " file:     %s\n"
            " tree:     %s\n"
            " what():   %s", fFilePath.c_str(), fTreeName.c_str(), e.what()); 
        std::exit(1);
    }

    fAppState = AppState::kInit; 

    //loop over all drawn functions, to register each of the requested branches
    for (auto& draw_function : fDrawFunctions) {

        try {
            std::printf("in <EventDisplayKernel::%s>: attempting to evaluate function '%s'...\n", __func__, draw_function.name.c_str()); 

            //try executing the function 
            draw_function.fcn(); 

        } catch (const std::exception& e) {

            Error(__func__, "Something went wrong evaluating draw-function for '%s'.\n what(): %s", draw_function.name.c_str(), e.what()); 
            std::exit(1); 
            return; 
        }
    }
    std::cout << "done evaluating all draw functions.\n"; 
    std::cout << "Looping over DataFrame..." << std::flush;
    
    size_t n_events; 

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

    LaunchGUI(1400, 700); 
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
    //static_assert(std::is_trivially_constructible_v<T>, "Template arg 'T' is not trivially constructable");

    //first, check the status of the app. 
    switch (fAppState) {
        case AppState::kNone : return T{};

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

                Error(  "GetData(run phase)", "Requested branch '%s' appears in tree, but not in app's list of branches. "
                        "(this shouldn't be possible...)",
                    branch_name.c_str()
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
                Error("GetData(init phase)", "Branch '%s' does not exist in tree '%s'", branch_name.c_str(), fTreeName.c_str()); 
                std::exit(1);  
                return T{};
            }

            //check to make sure the branch type matches the type given. 
            std::string branch_type = GetDataFrame()->GetColumnType(branch_name); 
            if (!BranchTypeMatches(branch_type, typeid(T))) {

                auto data_type = TDataType::GetDataType(TDataType::GetType(typeid(T)));

                Error("GetData(init phase)", "Branch '%s' is reported as having type '%s', but this function was invoked with type '%s'.",
                    branch_name.c_str(), 
                    branch_type.c_str(), 
                    (data_type ? data_type->GetTypeName().Data() : "null")
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
                std::printf("in <EventDisplayKernel::GetData(init phase)>: Requested branch '%s' added.\n", branch_name.c_str());
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
//explicity instantiate valid templates
template double EventDisplayKernel::GetData<double>(std::string branch_name);
//
template ROOT::VecOps::RVec<double> EventDisplayKernel::GetData<ROOT::VecOps::RVec<double>>(std::string branch_name);
//
template unsigned int EventDisplayKernel::GetData<unsigned int>(std::string branch_name);
//
//________________________________________________________________________________________________
bool EventDisplayKernel::BranchTypeMatches(std::string branch_name, const std::type_info& type)
{
    ROOT::RDataFrame *df; 
    if (df = GetDataFrame()) {
        Error(__func__, "Dataframe is null"); 
        std::exit(1); 
    }   

    if (!DoesBranchExist(branch_name)) {
        Error(__func__, "Branch '%s' does not exist in RDataFrame.", branch_name.c_str());
        return false; 
    }

    //Claude Fable showed me how to do this 

    const char* branch_type_str = df->GetColumnType(branch_name).c_str(); 
    
    // if the type passed is a ROOT type, then we can compare it & the column this way: 
    if (TClass *expected_type = TClass::GetClass(type)) {
    
        // the GetClass() method return nullptr if it's not a ROOT type
        return TClass::GetClass(branch_type_str) == expected_type; 
    }

    // if not, we use TDataType for fundamental types: 
    TDataType* data_type = gROOT->GetType(branch_type_str); 

    return (data_type != nullptr) && data_type->GetType() == TDataType::GetType(type);  
}
//________________________________________________________________________________________________
void EventDisplayKernel::AddDrawnItem(std::string item_name, const std::function<void(void)>& draw_function)
{
    //check to make sure another item with this same name is not already present. 
    auto find_it = std::find_if(fDrawFunctions.begin(), fDrawFunctions.end(), [item_name](const draw_fcn_and_state_t& rhs){ return rhs.name == item_name; });
    if (find_it != fDrawFunctions.end()) {
        Warning(__func__, "User attepmted to add drawn item '%s', but this already exists in list (duplicate will not be added).", item_name.c_str());
        return;  
    }

    //add this function to list of drawin functions
    fDrawFunctions.emplace_back( item_name, draw_function, true ); 

    std::printf("Added drawn item '%s'.\n", item_name.c_str()); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::Draw(TObject* object, const char* option)
{
    
    if (fAppState == AppState::kActive) {
        
        if (!object) {
            Warning(__func__, "Object called to be drawn is null."); 
            return; 
        }
        //set ownership over this object (so that ROOT doesn't have to manage it itself).
        object->SetBit(kMustCleanup); 
        object->ResetBit(kCanDelete); 

        fPrimitiveList.Add(object); 
        object->Draw(option); 
        return; 
    
    } else {

        if (object) { 
            object->SetBit(kMustCleanup); 
            object->ResetBit(kCanDelete); 
            delete object; 
        }
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
void EventDisplayKernel::LaunchGUI(UInt_t w, UInt_t h)
{
    TGMainFrame(
        gClient->GetRoot(), 
        w, h
    ); 

    SetCleanup(kDeepCleanup); 

    fFrame_canv = new TGHorizontalFrame(this, w, h); 

    fECanvas = new TRootEmbeddedCanvas("ECanvas_data", fFrame_canv, w, h); 

    TCanvas* canvas = GetCanvas(); 

    //draw an empty frame
    canvas->DrawFrame(fX0,fY0, fX1,fY1);
    
    fFrame_canv->AddFrame(fECanvas, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX | kLHintsCenterX, 0,0,0,5)); 

    AddFrame(fFrame_canv, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0)); 


    //now, we can start adding buttons
    fFrame_buttons = new TGHorizontalFrame(this, w, 50);

    //Next event button
    fGButton_next      = new TGTextButton(fFrame_buttons, "&Next event", 1); 
    fGButton_next->Connect("Clicked()", "EventDisplayKernel", this, "DoNextEvent()"); 
    fFrame_buttons->AddFrame(fGButton_next, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    //Prev. event button
    fGButton_prev      = new TGTextButton(fFrame_buttons, "&Prev. event", 1); 
    fGButton_prev->Connect("Clicked()", "EventDisplayKernel", this, "DoPrevEvent()"); 
    fFrame_buttons->AddFrame(fGButton_prev, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    AddFrame(fFrame_buttons, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 5, 10));

    //draw the first event
    DrawEventIndex(0); 

    SetWindowName("Event display");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}
//________________________________________________________________________________________________
EventDisplayKernel::~EventDisplayKernel()
{
    Cleanup(); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::CloseWindow()
{
    gApplication->Terminate(0); 
}
//________________________________________________________________________________________________
TCanvas* EventDisplayKernel::GetCanvas()
{
    if (fAppState != AppState::kActive) return nullptr; 

    if (fECanvas && fECanvas->GetCanvas()) {
        return fECanvas->GetCanvas(); 
    }

    Error(__func__, "Embedded canvas (%p) and / or TCanvas it contains (%p) are null.", fECanvas, (fECanvas ? fECanvas->GetCanvas() : nullptr)); 
    std::exit(1); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DoNextEvent()
{
    if (fEventIndex < fEventNumbers.size()-1) {
        ++fEventIndex; 
    } else {
        Info(__func__, "Cannont load next event; %u is already the max. event in the list.", fEventNumbers.back());
    }

    DrawEventIndex(fEventIndex); 
}
//________________________________________________________________________________________________
void EventDisplayKernel::DoPrevEvent()
{
    if (fEventIndex >= 0) {
        --fEventIndex; 
    } else {
        Info(__func__, "Cannont load prev. event; %u is already the min. event in the list.", fEventNumbers.front());
    }

    DrawEventIndex(fEventIndex); 
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

    std::printf("drawing event %u...", fEventNumber);

    //delete all drawn events. 
    fPrimitiveList.Delete(); 

    //run each (active) event-drawing function (in order!)
    for (const auto& draw_function : fDrawFunctions) {
        if(draw_function.is_active) { draw_function.fcn(); } 
    }

    auto canv = GetCanvas(); 
    canv->Modified(); 
    canv->Update(); 

    std::cout << "done.\n" << std::flush; 
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
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________

template<> std::string inst_helpers::GetBranchCode<double>() { return "F"; }