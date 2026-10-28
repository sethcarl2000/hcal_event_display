#include <EDK/UserSlider.hpp>
#include <EDK/Singleton.hpp>
#include <EDK/EventGUI.hpp>


namespace EDK 
{


//____________________________________________________________________________________________________________
UserSlider::UserSlider(Key<UserParameterPanel>, const TGWindow* ptr, UserParameterPanel* parent, UInt_t width, Info info)
    : TGHorizontalFrame(ptr, width), fInfo{info}
{
    //label representing the name of the parameter
    fParamNameLabel = new TGLabel(this, fInfo.name.c_str()); 
    AddFrame(
        fParamNameLabel, 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,5,5)
    );

    //label represeting the value of the parameter 
    fParamValueLabel = new TGLabel(this, Form(fInfo.format.c_str(), (fInfo.min + fInfo.max)/2.)); 
    AddFrame(
        fParamValueLabel, 
        new TGLayoutHints(kLHintsRight | kLHintsCenterY, 5,5,5,5)
    );

    // now, we add the actual slider
    fSlider = new TGHSlider(this, width);
    fSlider->SetRange(0, fInfo.n_steps-1); 
    fSlider->Connect("PositionChanged(Int_t)", "EDK::UserSlider", this, "DoSliderMoved()");
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

    double val = fInfo.min + slider_pos*(fInfo.max - fInfo.min);

    auto& kernel = Singleton::Instance(); 

    // set this new event index 
    kernel.SetParameter<double>(fInfo.name, val); 
    
    // refresh all the drawn windows p-
    // [[[[[[[[[[[[[[[[[[[[[[[[          7-l=kvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvmddddddddddddddddddddddddddddddddddddddddddddddddddddddddcX]]]]]]]]]]]]]]]]]]]]]]]]
    //      - muon's comment

    kernel.DrawCurrentEvent(); 
    //kernel.DoDrawTimestamp(fMyKey, kernel.GetTimestamp()); 
    
    fParamValueLabel->SetText(Form(fInfo.format.c_str(), val)); 
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

}