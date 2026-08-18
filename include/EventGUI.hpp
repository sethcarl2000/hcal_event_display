#ifndef EventGUI_hpp
#define EventGUI_hpp

#include <Key.hpp>
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

class EventGUI : public TGMainFrame {

    /// this is the class' 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<EventGUI> fMyKey{}; 

    // buttons to toggle the visibility of different windows
    std::vector<WindowCheckbox> fToggleWindowButtons;  
    
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

    // toggles visibility of each window. window id's start at 0 for the first window, and go in the order in which they are listed for us by the kernel in the EventGUI constructor. 
    void ToggleWindow();

    void CloseWindow() override; 

    ClassDef(EventGUI,1);
};


#endif