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
 
class UserApp; 

////////////////////////////////////////////////////////////////////////////////////////
//
//  UserSlider
//  
//  this panel holds all time controls the user can navigate & choose different timestamps with. 
//
class UserSlider : public TGGroupFrame {
private:    

    std::string fParamName{"none"}; 
    
    double fMaxVal; 
    double fMinVal; 

    UserApp* fParent; 

    TGHSlider* fSlider;

    TGLabel* fParamLabel;

    Key<UserSlider> fMyKey{};

public: 

    //info needed to construct a slider
    struct SliderInfo { std::string name; double min, max; }; 

    UserSlider(Key<UserApp>, const TGWindow*, UserApp* parent, UInt_t width, std::string param_name, double min, double max);

    // Signals (to propagate back to the kernel)
    // signal (when the slider is moved)
    void DoSliderMoved();

    ClassDef(UserSlider,0);
};


#endif