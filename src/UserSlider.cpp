#include <UserSlider.hpp>
#include <EventDisplayKernel.hpp>
#include <EventGUI.hpp>

//____________________________________________________________________________________________________________
UserSlider::UserSlider(Key<UserParameterPanel>, const TGWindow* ptr, UserParameterPanel* parent, UInt_t width, const std::string& param_name, double min, double max)
    : TGHorizontalFrame(ptr, width), 
    fParamName{param_name}, 
    fMinVal{min}, fMaxVal{max}
{
    //label representing the name of the parameter
    fParamNameLabel = new TGLabel(this, param_name.c_str()); 
    AddFrame(
        fParamNameLabel, 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,5,5)
    );

    //label represeting the value of the parameter 
    fParamValueLabel = new TGLabel(this, Form(fFormatString.c_str(), (min+max)/2.)); 
    AddFrame(
        fParamValueLabel, 
        new TGLayoutHints(kLHintsRight | kLHintsCenterY, 5,5,5,5)
    );

    // now, we add the actual slider
    fSlider = new TGHSlider(this, width);
    fSlider->Connect("PositionChanged(Int_t)", "UserSlider", this, "DoSliderMoved()");
    AddFrame(
        fSlider, 
        new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX, 5,5,5,5)
    );
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
    
    // refresh all the drawn windows p-
    // [[[[[[[[[[[[[[[[[[[[[[[[          7-l=kvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvmddddddddddddddddddddddddddddddddddddddddddddddddddddddddcX]]]]]]]]]]]]]]]]]]]]]]]]
    //      - muon's comment

    kernel.DoDrawTimestamp(fMyKey, kernel.GetTimestamp()); 
    
    fParamValueLabel->SetText(Form(fFormatString.c_str(), val)); 
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