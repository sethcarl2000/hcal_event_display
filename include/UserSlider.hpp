#ifndef UserSlider_hpp
#define UserSlider_hpp

#include <Key.hpp>
// TGUI headers
#include <TGFrame.h> 
#include <TGButton.h> 
#include <TGLabel.h>
#include <TGSlider.h>
// stdlib headers 
#include <map> 
#include <string> 
#include <vector> 
#include <memory> 
 
class UserParameterPanel; 

////////////////////////////////////////////////////////////////////////////////////////
//
//  UserSlider
//  
//  this panel holds all time controls the user can navigate & choose different timestamps with. 
//
class UserSlider : public TGHorizontalFrame {
private:    

    std::string fParamName{"none"}; 
    
    double fMaxVal; 
    double fMinVal; 

    UserParameterPanel* fParent; 

    TGHSlider* fSlider;

    //labels for the name & format of the string. 
    TGLabel *fParamNameLabel, *fParamValueLabel;

    // the printf-format string. 
    std::string fFormatString={"%+.4e"}; 

    Key<UserSlider> fMyKey{};

public: 

    //info needed to construct a slider
    struct SliderInfo { std::string name; double min, max; }; 

    UserSlider(Key<UserParameterPanel>, const TGWindow*, UserParameterPanel* parent, UInt_t width, const std::string& param_name, double min, double max);

    // Signals (to propagate back to the kernel)
    void DoSliderMoved(); // signal (when the slider is moved)

    ClassDef(UserSlider,0);
};


#endif