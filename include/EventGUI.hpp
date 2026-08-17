#ifndef EventGUI_hpp
#define EventGUI_hpp

#include <Key.hpp>
// TGUI headers
#include <TGFrame.h> 
#include <TGButton.h> 
#include <TGLabel.h> 

class EventDisplayKernel;

class EventGUI : public TGMainFrame {

    //
    TGHorizontalFrame *fFrame_buttons; 
    TGTextButton *fGButton_next, *fGButton_prev; 
    TGLabel *fGLabel_eventNumber; 

public: 

    // Default constructor. We add a 'kernel key' as an argument,
    // because it's an object that only the kernel can create. 
    EventGUI(
        Key<EventDisplayKernel>,
        const TGWindow* ptr,
        UInt_t w, UInt_t h
    ); 

    // some methods we need to override
    ~EventGUI() { CloseWindow(); } 

    // Signals (to propagate back to the kernel)
    //
    // Move to the next event
    void DoNextEvent(); 
    // Move to the prev. event
    void DoPrevEvent();

    void CloseWindow() override; 

};


#endif