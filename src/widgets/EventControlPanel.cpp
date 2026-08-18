
#include <EventGUI.hpp>
#include <EventDisplayKernel.hpp>
//
#include <TGLabel.h>

//________________________________________________________________________________________________
EventControlPanel::EventControlPanel(Key<EventGUI>, const TGWindow* ptr, EventGUI* parent, size_t max_event_index)
    : TGGroupFrame(ptr, "Event Controls", kVerticalFrame), fParent{parent}, fMaxEventIndex{max_event_index}, fRand{}
{

    ///////////////////////////////////////////////////////////////
    //
    //  This is the event button frame. It will host our controls. 
    //
    auto frame_buttons = new TGHorizontalFrame(this, 500, 50);

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
    auto frame_info = new TGHorizontalFrame(this, 500, 50);

    fEventLabel = new TGLabel(frame_info, "Event: none");
    frame_info->AddFrame(fEventLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,5,5)); 
    AddFrame(frame_info, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));
    
}
//________________________________________________________________________________________________
void EventControlPanel::DoRandomEvent()
{
    //pick a random number
    size_t rand_index = (size_t)fRand.Rndm() * fMaxEventIndex;
    fParent->SetEventIndex(rand_index); 
}
//________________________________________________________________________________________________
void EventControlPanel::NewEventIndex(Key<EventGUI>, size_t index)
{
    fEventIndex = index; 
    fEventNumber = (UInt_t)index; //EventDisplayKernel::Instance().GetEventNumber(index);
    fEventLabel->SetText(Form("Event: %u",fEventNumber));
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________