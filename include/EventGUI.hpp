#ifndef EventGUI_hpp
#define EventGUI_hpp

#include <Key.hpp>
// ROOT headers
#include <TRandom3.h> 
// TGUI headers
#include <TGFrame.h> 
#include <TGButton.h> 
#include <TGLabel.h>
#include <TGSlider.h>
// stdlib headers 
#include <map> 
#include <string> 
#include <vector> 
#include <memory> 

class UserWindow; 
class EventDisplayKernel;

struct WindowCheckbox { std::string name; TGCheckButton* button; };  

class EventControlPanel; 
class ToggleWindowsPanel; 
class TimeControlPanel;

class EventGUI : public TGMainFrame {

    /// this is the class' 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<EventGUI> fMyKey{}; 

    EventControlPanel* fEventControlPanel; 
    
    ToggleWindowsPanel* fToggleWindowsPanel; 

    TimeControlPanel* fTimeControlPanel;
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
        const std::vector<std::string>& window_names
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
    void DoToggleWindow(const std::string& window_name, bool do_activate);

    void CloseWindow() override; 


    // this siganl tells the GUI that a new event has beeen picked
    void NewEventIndex(Key<EventDisplayKernel>, size_t index); 
    // update status of all windows
    void SetWindowStatus(Key<EventDisplayKernel>, const std::vector<std::unique_ptr<UserWindow>>& windows);

    TimeControlPanel* GetTimeControlPanel() { return fTimeControlPanel; }   

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

    void SetWindowStatus(Key<EventGUI>, const std::vector<std::unique_ptr<UserWindow>>& windows);

    void DoToggleWindow(); // slot

    ClassDef(ToggleWindowsPanel,0);
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
    TGHSlider* fSlider; 

    size_t fEventIndex; 
    UInt_t fEventNumber; 

    EventGUI* fParent; 

    // buttons to toggle the visibility of different windows
    std::vector<WindowCheckbox> fToggleWindowButtons;  

public: 
    EventControlPanel(Key<EventGUI>, const TGWindow*, EventGUI* parent, UInt_t width);

    // Signals (to propagate back to the kernel)
    //
    // Move to the next event
    void DoNextEvent() { fParent->DoNextEvent(); } 
    // Move to the prev. event
    void DoPrevEvent() { fParent->DoPrevEvent(); }
    // Update the event slider
    void DoSliderReleased(); 
    // Pick a random event
    void DoRandomEvent(); 

    // this siganl tells the GUI that a new event has beeen picked
    void NewEventIndex(Key<EventGUI>, size_t index); 

    ClassDef(EventControlPanel,0);
};


////////////////////////////////////////////////////////////////////////////////////////
//
//  Time Control Panel
//  
//  this panel holds all time controls the user can navigate & choose different timestamps with. 
//
class TimeControlPanel : public TGGroupFrame {
private: 

    double fTimestamp;

    EventGUI* fParent; 

    TGHSlider* fSlider;

    TGLabel* fTimeLabel;

    double fMinTimestamp{-100.}, fMaxTimestamp{+100.};

    Key<TimeControlPanel> fMyKey{};

public: 
    TimeControlPanel(Key<EventGUI>, const TGWindow*, EventGUI* parent, UInt_t width);

    // Signals (to propagate back to the kernel)
    // signal (when the slider is moved)
    void DoSliderMoved();

    void SetTimestamp(Key<EventDisplayKernel>, double timestamp); 

    ClassDef(TimeControlPanel,0);
};

#endif