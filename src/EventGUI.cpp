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
EventGUI::EventGUI(Key<EventDisplayKernel> key, const TGWindow* ptr, UInt_t w, UInt_t h, const std::vector<std::string>& window_names)
    : TGMainFrame( gClient->GetRoot(), w, h ) 
{
    SetCleanup(kDeepCleanup); 

    ///////////////////////////////////////////////////////////////
    //
    //  This is the event button frame
    //
    auto frame_buttons = new TGHorizontalFrame(this, w, 50);

    //Next event button
    auto button_next_event  = new TGTextButton(frame_buttons, "&Next event", 1); 
    button_next_event->Connect("Clicked()", "EventGUI", this, "DoNextEvent()"); 
    frame_buttons->AddFrame(button_next_event, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    //Prev. event button
    auto button_prev_event  = new TGTextButton(frame_buttons, "&Prev. event", 1); 
    button_prev_event->Connect("Clicked()", "EventGUI", this, "DoPrevEvent()"); 
    frame_buttons->AddFrame(button_prev_event, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    AddFrame(frame_buttons, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5,5,5,10));


    ///////////////////////////////////////////////////////////////
    //
    //  This is the 'toggle windows' frame
    //
    auto frame_toggle_window = new TGVerticalFrame(this, w, 50);

    //now, let's create the list of all toggled buttons
    fToggleWindowButtons.reserve(window_names.size()); 

    int button_id=0; 
    for (size_t i=0; i<window_names.size(); i++) {

        const auto name = window_names[i];

        auto button = new TGCheckButton(frame_toggle_window, name.c_str(), button_id);
        button->SetState(kButtonDown);
        button->Connect("Toggled(Bool_t)", "EventGUI", this, "ToggleWindow()"); 
        ++button_id; 

        frame_toggle_window->AddFrame(button, new TGLayoutHints(kLHintsBottom | kLHintsLeft, 5, 5, 0, 0)); 

        fToggleWindowButtons.emplace_back( name, button );
    }

    AddFrame(frame_toggle_window, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5,5,5,10)); 


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
void EventGUI::ToggleWindow() 
{
    auto clicked_button = static_cast<TGCheckButton*>(gTQSender);

    if (!clicked_button) {
        Error(__func__, "Button which emitted this signal is null"); 
        std::exit(1);
        return; 
    }
#ifdef DEBUG
    Info(__func__, "Searching for matching button in list. emitting button name: %s", clicked_button->GetName());
#endif

    //find which button sent this signal 
    auto find_it = std::find_if (
        fToggleWindowButtons.begin(),
        fToggleWindowButtons.end(),
        [clicked_button](const WindowCheckbox& rhs) { return rhs.button == clicked_button; }
    );


    if (find_it == fToggleWindowButtons.end()) {
        Error(__func__, "Requested to toggle window with button '%s', but a matching button could not be found.", clicked_button->GetName());
        std::exit(1);
        return; 
    }
    auto& window_checkbox = *find_it;
#ifdef DEBUG
    Info(__func__, "Button '%s' has been found in list. window name: %s", clicked_button->GetName(), window_checkbox.name.c_str());
#endif
    if (!window_checkbox.button) {
        Error(__func__, "Somehow, the ptr to the checkbox object is null (this should not be possible...)");
        std::exit(1);
        return;  
    }   
    
    bool is_active = (window_checkbox.button->IsOn()); 

#ifdef DEBUG
    Info(__func__, "Task: %s", (is_active ? "Activate" : "Deactivate"));
#endif

    auto& kernel = EventDisplayKernel::Instance(); 

    const auto& name = window_checkbox.name; 

#ifdef DEBUG
    Info(__func__, "Propagating signal to kernel...");
#endif

    //tell the kernel to update the window 
    kernel.DoToggleWindow(fMyKey, name, is_active);
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________