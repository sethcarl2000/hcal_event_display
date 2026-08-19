
#include <EventGUI.hpp>
#include <EventDisplayKernel.hpp>
// TGUI headers
#include <TGLabel.h>
#include <TGSlider.h>

//________________________________________________________________________________________________
EventControlPanel::EventControlPanel(Key<EventGUI>, const TGWindow* ptr, EventGUI* parent, UInt_t width)
    : TGGroupFrame(ptr, "Event Controls", kVerticalFrame), fParent{parent}, fRand{}
{

    ///////////////////////////////////////////////////////////////
    //
    //  This is the event button frame. It will host our controls. 
    //
    auto frame_buttons = new TGHorizontalFrame(this, width, 50);

    //Next event button
    auto button_next_event  = new TGTextButton(frame_buttons, "&Next event", 1); 
    button_next_event->Connect("Clicked()", "EventControlPanel", this, "DoNextEvent()"); 
    frame_buttons->AddFrame(button_next_event, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    //Prev. event button
    auto button_prev_event  = new TGTextButton(frame_buttons, "&Prev. event", 1); 
    button_prev_event->Connect("Clicked()", "EventControlPanel", this, "DoPrevEvent()"); 
    frame_buttons->AddFrame(button_prev_event, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    //Random event button
    auto button_rand_event  = new TGTextButton(frame_buttons, "&Random event", 1); 
    button_rand_event->Connect("Clicked()", "EventControlPanel", this, "DoRandomEvent()"); 
    frame_buttons->AddFrame(button_rand_event, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 20, 10, 5, 5)); 

    AddFrame(frame_buttons, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5,5,5,5));


    ///////////////////////////////////////////////////////////////
    //
    //  This is the event information frame
    //
    auto frame_info = new TGHorizontalFrame(this, width, 50);

    fEventLabel = new TGLabel(frame_info, "Event: none              :");
    frame_info->AddFrame(fEventLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,5,5)); 
    AddFrame(frame_info, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));


    ///////////////////////////////////////////////////////////////
    //
    //  This is the event information frame
    //
    auto frame_slider = new TGHorizontalFrame(this, width, 50);

    fSlider = new TGHSlider(frame_slider, width);
    fSlider->Connect("Released()", "EventControlPanel", this, "DoSliderReleased()");
    frame_slider->AddFrame(fSlider, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5,5,5,5)); 
    AddFrame(frame_slider, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));

}
//________________________________________________________________________________________________
void EventControlPanel::DoRandomEvent()
{
    //pick a random number
    size_t rand_index = (size_t)(fRand.Rndm() * (EventDisplayKernel::Instance().GetNEvents() - 1));
#ifdef DEBUG
    Info(__func__, "Random event index: %zi / %zi", rand_index, EventDisplayKernel::Instance().GetNEvents()-1);
#endif 
    fParent->SetEventIndex(rand_index); 
}
//________________________________________________________________________________________________
void EventControlPanel::NewEventIndex(Key<EventGUI>, size_t index)
{
    fEventIndex = index; 
    
    auto& kernel = EventDisplayKernel::Instance(); 

    size_t max_event_index = kernel.GetNEvents()-1; 
    
    // update the slider position 
    //normalized from [0, 1]
    double slider_pos = ((double)index)/((double)max_event_index);

    slider_pos *= (double)(fSlider->GetMaxPosition() - fSlider->GetMinPosition());

    fEventNumber = kernel.GetEventNumber(index);
    fEventLabel->SetText(Form("Event: %u",fEventNumber));

    fSlider->SetPosition((Int_t)slider_pos); 
}
//________________________________________________________________________________________________
void EventControlPanel::DoSliderReleased()
{
    //normalized in the range [0,1]
    double slider_pos = (double)(fSlider->GetPosition() - fSlider->GetMinPosition()); 
    slider_pos = slider_pos / ((double)(fSlider->GetMaxPosition() - fSlider->GetMinPosition()));

    auto& kernel = EventDisplayKernel::Instance(); 

    size_t max_event_index = kernel.GetNEvents()-1; 
    
    size_t index = max_event_index * slider_pos; 
    
    //set this new event index 
    fParent->SetEventIndex(index); 
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________