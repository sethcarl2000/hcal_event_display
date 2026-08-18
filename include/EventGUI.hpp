#ifndef EventGUI_hpp
#define EventGUI_hpp

#include <Key.hpp>
// ROOT headers
#include <TRandom3.h> 
// TGUI headers
#include <TGFrame.h> 
#include <TGButton.h> 
#include <TGLabel.h>
// stdlib headers 
#include <map> 
#include <string> 
#include <vector> 

class EventDisplayKernel;

struct WindowCheckbox { std::string name; TGCheckButton* button; };  

class EventControlPanel; 

class EventGUI : public TGMainFrame {

    /// this is the class' 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<EventGUI> fMyKey{}; 

    EventControlPanel* fEventControlPanel; 
    //
    //TGHorizontalFrame *fFrame_buttons; 
    //TGTextButton *fGButton_next, *fGButton_prev; 
    //TGLabel *fGLabel_eventNumber; 

public: 

    // Default constructor. We add a 'kernel key' as an argument,
    // because it's an object that only the kernel can create. 
    EventGUI(
        Key<EventDisplayKernel>,
        const TGWindow* ptr,
        UInt_t w, UInt_t h,
        const std::vector<std::string>& window_names,
        size_t max_event_index
    ); 

    // some methods we need to override
    ~EventGUI() { CloseWindow(); } 

    // Signals (to propagate back to the kernel)
    //
    // Move to the next event
    void DoNextEvent(); 
    // Move to the prev. event
    void DoPrevEvent();

    void SetEventIndex(size_t index); 

    // toggles visibility of each window. window id's start at 0 for the first window, and go in the order in which they are listed for us by the kernel in the EventGUI constructor. 
    void ToggleWindow(const std::string& window_name, bool do_activate);

    void CloseWindow() override; 


    // this siganl tells the GUI that a new event has beeen picked
    void NewEventIndex(Key<EventDisplayKernel>, size_t index); 

    ClassDefOverride(EventGUI,0);
};

//these are widgets that we'll draw.

////////////////////////////////////////////////////////////////////////////////////////
//
//  Toggle windows panel. 
//  
//  this panel has a list of all user-specified windows, so they may be activated / deactivated. 
//
class ToggleWindowsPanel : public TGGroupFrame {
private: 
    EventGUI *fParent; 

    // buttons to toggle the visibility of different windows
    std::vector<WindowCheckbox> fToggleWindowButtons;  

public: 
    ToggleWindowsPanel(Key<EventGUI>, const TGWindow*, EventGUI* parent, const std::vector<std::string>& window_names);

    void ToggleWindow(); // slot


    ClassDefOverride(ToggleWindowsPanel,0);
};


////////////////////////////////////////////////////////////////////////////////////////
//
//  Event panel
//  
//  this panel holds all event controls the user can navigate & choose different events with. 
//
class EventControlPanel : public TGGroupFrame {
private: 

    TRandom3 fRand; 

    TGLabel* fEventLabel; 

    size_t fMaxEventIndex, fEventIndex; 
    UInt_t fEventNumber; 

    EventGUI* fParent; 

    // buttons to toggle the visibility of different windows
    std::vector<WindowCheckbox> fToggleWindowButtons;  

public: 
    EventControlPanel(Key<EventGUI>, const TGWindow*, EventGUI* parent, size_t fMaxEventIndex);

    // Signals (to propagate back to the kernel)
    //
    // Move to the next event
    void DoNextEvent() { fParent->DoNextEvent(); } 
    // Move to the prev. event
    void DoPrevEvent() { fParent->DoPrevEvent(); }
    // Pick a random event
    void DoRandomEvent(); 

    // this siganl tells the GUI that a new event has beeen picked
    void NewEventIndex(Key<EventGUI>, size_t index); 

    ClassDefOverride(EventControlPanel,0);
};

#endif