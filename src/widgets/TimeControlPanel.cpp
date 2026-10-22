
#include <EDK/EventGUI.hpp>
#include <EDK/Singleton.hpp>
// TGUI headers
#include <TGLabel.h>
#include <TGSlider.h>

namespace EDK 
{

//________________________________________________________________________________________________
TimeControlPanel::TimeControlPanel(Key<EventGUI>, const TGWindow* ptr, EventGUI* parent, UInt_t width)
    : TGGroupFrame(ptr, "Time Controls", kVerticalFrame), fParent{parent}
{

    ///////////////////////////////////////////////////////////////
    //
    //  This is the event information frame
    //
    auto frame_info = new TGHorizontalFrame(this, width, 50);

    fTimeLabel = new TGLabel(frame_info, "Time: none ");
    frame_info->AddFrame(fTimeLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kLHintsExpandX, 5,5,5,5)); 
    AddFrame(frame_info, new TGLayoutHints(kLHintsBottom | kLHintsCenterX | kLHintsExpandX, 5,5,5,5));


    ///////////////////////////////////////////////////////////////
    //
    //  This is the slider's frame
    //
    auto frame_slider = new TGHorizontalFrame(this, width, 50);

    fSlider = new TGHSlider(frame_slider, width);
    fSlider->Connect("PositionChanged(Int_t)", "TimeControlPanel", this, "DoSliderMoved()");
    frame_slider->AddFrame(fSlider, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX, 5,5,5,5)); 
    AddFrame(frame_slider, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX, 5,5,5,5));

}
//________________________________________________________________________________________________
void TimeControlPanel::DoSliderMoved()
{
    //normalized in the range [0,1]
    double slider_pos = (double)(fSlider->GetPosition() - fSlider->GetMinPosition()); 
    slider_pos = slider_pos / ((double)(fSlider->GetMaxPosition() - fSlider->GetMinPosition()));

    double time = fMinTimestamp + slider_pos*(fMaxTimestamp - fMinTimestamp);

    auto& kernel = Singleton::Instance(); 

    //set this new event index 
    kernel.DoSetTimestamp(fMyKey, time); 
}
//________________________________________________________________________________________________
void TimeControlPanel::SetTimestamp(Key<Singleton>, double timestamp)
{
#ifdef DEBUG
    Info(__func__, "in body. timestamp: %f", timestamp);
#endif
    fTimestamp = timestamp; 

    //update the label
    fTimeLabel->SetText(Form("Time: %.2f", fTimestamp)); 

    //update the slider position
    double slider_pos = (fTimestamp - fMinTimestamp) / (fMaxTimestamp - fMinTimestamp); 
    int slider_int_pos = (Int_t)(slider_pos * (double)(fSlider->GetMaxPosition() - fSlider->GetMinPosition())) + fSlider->GetMinPosition(); 
    fSlider->SetPosition(slider_int_pos); 

#ifdef DEBUG
    Info(__func__, "leaving body");
#endif
}
//________________________________________________________________________________________________
//________________________________________________________________________________________________

}