#include <EventGUI.hpp>
#include <UserWindow.hpp>
//stdlib headers
#include <algorithm> 
#include <cstdlib> 

//________________________________________________________________________________________________
ToggleWindowsPanel::ToggleWindowsPanel(Key<EventGUI>, const TGWindow* ptr, EventGUI* parent, const std::vector<std::string>& window_names)
    : TGGroupFrame(ptr, "User Windows", kVerticalFrame), fParent{parent}
{
    ///////////////////////////////////////////////////////////////
    //
    //  This is the 'toggle windows' frame
    //
    //now, let's create the list of all toggled buttons
    fToggleWindowButtons.reserve(window_names.size()); 

    int button_id=0; 
    for (size_t i=0; i<window_names.size(); i++) {

        const auto name = window_names[i];

        auto button = new TGCheckButton(this, name.c_str(), button_id);
        button->SetState(kButtonDown);
        button->Connect("Toggled(Bool_t)", "ToggleWindowsPanel", this, "DoToggleWindow()"); 
        ++button_id; 

        AddFrame(button, new TGLayoutHints(kLHintsBottom | kLHintsLeft, 5, 5, 0, 0)); 

        fToggleWindowButtons.emplace_back( name, button );
    }
}
//________________________________________________________________________________________________
void ToggleWindowsPanel::DoToggleWindow()
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
    auto find_it = std::find_if(
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
    
    bool do_activate = (window_checkbox.button->IsOn()); 
#ifdef DEBUG
    Info(__func__, "Task: %s", (do_activate ? "Activate" : "Deactivate"));
#endif

    fParent->DoToggleWindow(window_checkbox.name, do_activate);
}
//________________________________________________________________________________________________
void ToggleWindowsPanel::SetWindowStatus(Key<EventGUI>, const std::vector<std::unique_ptr<UserWindow>>& windows)
{
    //loop over all windows
    for (const auto& window : windows) {
        
        if (!window) {
            Error(__func__, "Somehow, window that we were handed is null (this should not be possible...)"); 
            std::exit(1);
            return; 
        }

        //name of the window whose status we want to update 
        const auto& name = window->GetName(); 

        //look for the button corresponding to this window 
        auto find_it = std::find_if(
            fToggleWindowButtons.begin(), 
            fToggleWindowButtons.end(),
            [&name](const WindowCheckbox& checkbox) { return checkbox.name == name; }
        ); 
        
        if (find_it == fToggleWindowButtons.end()) {
            Error(__func__, "Requested to toggle window with button '%s', but a matching button could not be found.", name.c_str());
            std::exit(1);
            return;
        } 

        //make sure the button reflects the state of the window
        auto& button = find_it->button; 
        button->SetState(window->IsActive() ? kButtonDown : kButtonUp);
    }
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________