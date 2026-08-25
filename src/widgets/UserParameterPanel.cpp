
#include <EventGUI.hpp>
#include <UserSlider.hpp>
#include <EventDisplayKernel.hpp>
// TGUI headers
#include <TGLabel.h>
#include <TGSlider.h>

//________________________________________________________________________________________________
UserParameterPanel::UserParameterPanel(Key<EventGUI>, const TGWindow* ptr, EventGUI* parent, UInt_t width)
    : TGGroupFrame(ptr, "Time Controls", kVerticalFrame), fParent{parent}, fWidth{width}
{
    //this constructor is unique; we don't add any sliders! 
}
//________________________________________________________________________________________________
void UserParameterPanel::AddUserSlider(Key<EventGUI>, const std::string& param_name, double min, double max)
{
    AddFrame(
        new UserSlider(fMyKey, this, this, fWidth, param_name, min, max), 
        new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 2,2,2,2)
    ); 
}   
//________________________________________________________________________________________________
//________________________________________________________________________________________________