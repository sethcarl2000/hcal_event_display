#ifndef EDK_UserSlider_hpp
#define EDK_UserSlider_hpp

#include <EDK/Key.hpp>
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

namespace EDK 
{

class UserParameterPanel; 

////////////////////////////////////////////////////////////////////////////////////////
//
//  UserSlider
//  
//  this panel holds all time controls the user can navigate & choose different timestamps with. 
//
class UserSlider : public TGHorizontalFrame {
public:     

    /// @brief Information for this slider 
    struct Info { 
        std::string name{""}; 
        double min{0.}, max{0.}; 
        int n_steps{1000}; 
        std::string format{"%.4e"}; 
    }; 

private:    

    std::string fParamName{"none"}; 
    
    Info fInfo{}; 

    UserParameterPanel* fParent; 

    TGHSlider* fSlider;

    //labels for the name & format of the string. 
    TGLabel *fParamNameLabel, *fParamValueLabel;

    // the printf-format string. 
    std::string fFormatString={"%+.4e"}; 

    Key<UserSlider> fMyKey{};

public: 


    UserSlider(Key<UserParameterPanel>, const TGWindow*, UserParameterPanel* parent, UInt_t width, Info info);

    // Signals (to propagate back to the kernel)
    void DoSliderMoved(); // signal (when the slider is moved)

    ClassDef(UserSlider,0);
};

}

#endif