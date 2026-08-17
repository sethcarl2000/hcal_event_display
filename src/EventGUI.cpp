#include <EventGUI.hpp>
#include <EventDisplayKernel.hpp>
//TGUI headers
#include <TApplication.h>

//________________________________________________________________________________________________
//________________________________________________________________________________________________
EventGUI::EventGUI(Key<EventDisplayKernel> key, const TGWindow* ptr, UInt_t w, UInt_t h)
    : TGMainFrame( gClient->GetRoot(), w, h ) 
{
    SetCleanup(kDeepCleanup); 

    //now, we can start adding buttons
    fFrame_buttons = new TGHorizontalFrame(this, w, 50);

    //Next event button
    fGButton_next      = new TGTextButton(fFrame_buttons, "&Next event", 1); 
    fGButton_next->Connect("Clicked()", "EventGUI", this, "DoNextEvent()"); 
    fFrame_buttons->AddFrame(fGButton_next, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    //Prev. event button
    fGButton_prev      = new TGTextButton(fFrame_buttons, "&Prev. event", 1); 
    fGButton_prev->Connect("Clicked()", "EventGUI", this, "DoPrevEvent()"); 
    fFrame_buttons->AddFrame(fGButton_prev, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    AddFrame(fFrame_buttons, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 5, 10));

    SetWindowName("Event display");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}
//________________________________________________________________________________________________
void EventGUI::CloseWindow()
{
    Cleanup(); 
    //tell the kernel that the user called for us to close the app
    EventDisplayKernel::Instance().CloseApp(fMyKey);
}
//________________________________________________________________________________________________
void EventGUI::DoNextEvent()
{
    EventDisplayKernel::Instance().DoNextEvent(fMyKey);
}
//________________________________________________________________________________________________
void EventGUI::DoPrevEvent()
{
    EventDisplayKernel::Instance().DoPrevEvent(fMyKey);
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