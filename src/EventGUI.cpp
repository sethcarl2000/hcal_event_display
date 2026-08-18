#include <EventGUI.hpp>
#include <EventDisplayKernel.hpp>
//TGUI headers
#include <TApplication.h>
#include <TError.h> 
//stdlib headers
#include <cstdlib> 
#include <algorithm> 

//________________________________________________________________________________________________
//________________________________________________________________________________________________
EventGUI::EventGUI(Key<EventDisplayKernel> key, const TGWindow* ptr, UInt_t w, UInt_t h, const std::vector<std::string>& window_names, size_t max_event_index)
    : TGMainFrame( gClient->GetRoot(), w, h ) 
{
    SetCleanup(kDeepCleanup); 

    // Add the event control panel
    fEventControlPanel = new EventControlPanel(fMyKey, this, this, max_event_index);
    AddFrame(
        fEventControlPanel,
        new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5,5,5,5)
    );

    // Add the user window control panel 
    AddFrame(
        new ToggleWindowsPanel(fMyKey, this, this, window_names),
        new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5,5,5,5)
    ); 

    SetWindowName("Event Display controls");
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
void EventGUI::SetEventIndex(size_t index)
{
    EventDisplayKernel::Instance().SetEventIndex(fMyKey, index);
}
//________________________________________________________________________________________________
void EventGUI::ToggleWindow(const std::string& window_name, bool do_activate) 
{
    auto& kernel = EventDisplayKernel::Instance(); 

#ifdef DEBUG
    Info(__func__, "Propagating signal to kernel...");
#endif

    //tell the kernel to update the window 
    kernel.DoToggleWindow(fMyKey, window_name, do_activate);
}
//________________________________________________________________________________________________
void EventGUI::NewEventIndex(Key<EventDisplayKernel>, size_t index)
{
    fEventControlPanel->NewEventIndex(fMyKey, index);
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________