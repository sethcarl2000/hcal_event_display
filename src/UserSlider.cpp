#include <UserSlider.hpp>
#include <EventDisplayKernel.hpp>

//____________________________________________________________________________________________________________
UserSlider::UserSlider(Key<UserApp>, const TGWindow* ptr, UserApp* parent, UInt_t width, std::string param_name, double min, double max)
    : TGGroupFrame(ptr, param_name.c_str()), 
    fParamName{param_name}, 
    fMinVal{min}, fMaxVal{max}
{
    ///////////////////////////////////////////////////////////////
    //
    //  This is the param information frame
    //
    auto frame_info = new TGHorizontalFrame(this, width, 50);

    fParamLabel = new TGLabel(frame_info, Form("%s: none              :", fParamName.c_str()));
    frame_info->AddFrame(fParamLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,5,5)); 
    AddFrame(frame_info, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));


    ///////////////////////////////////////////////////////////////
    //
    //  This is the slider's frame
    //
    auto frame_slider = new TGHorizontalFrame(this, width, 50);

    fSlider = new TGHSlider(frame_slider, width);
    fSlider->Connect("PositionChanged(Int_t)", "UserSlider", this, "DoSliderMoved()");
    frame_slider->AddFrame(fSlider, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5,5,5,5)); 
    AddFrame(frame_slider, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));
}
//____________________________________________________________________________________________________________
void UserSlider::DoSliderMoved()
{
    //normalized in the range [0,1]
    double slider_pos = (double)(fSlider->GetPosition() - fSlider->GetMinPosition()); 
    slider_pos = slider_pos / ((double)(fSlider->GetMaxPosition() - fSlider->GetMinPosition()));

    double val = fMinVal + slider_pos*(fMaxVal - fMinVal);

    auto& kernel = EventDisplayKernel::Instance(); 

    // set this new event index 
    kernel.SetParameter<double>(fParamName, val); 
    
    // refresh all the drawn windows 
    kernel.DoDrawTimestamp(fMyKey, kernel.GetTimestamp()); 
    
    fParamLabel->SetText(Form("%s: %f", fParamName.c_str(), val)); 
}
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________
//____________________________________________________________________________________________________________