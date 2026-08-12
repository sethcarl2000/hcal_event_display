#ifndef UserWindow_hpp
#define UserWindow_hpp

#include <AppDrawFunction.hpp>
#include <DrawFunction.hpp>
#include <UserApp.hpp> 
//TGUI headers
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h> 
//ROOT headers
#include <TList.h> 
// stdlib headers
#include <functional> 
#include <vector> 
#include <utility> 

class EventDisplayKernel; 

class UserWindow {
private: 

    //list of all drawn primitives
    TList fPrimitiveList; 

    //'name', which uniquely identifies this window. 
    std::string fName; 

    //the 'app' this window actually owns
    UserApp* fApp; 

    TRootEmbeddedCanvas *fECanvas; 

    // this is the user-supplied function which is responsible for actually drawing the user-defined window. It must define a canvas!  
    AppDrawFunction fAppDrawFunction; 

    std::vector<DrawFunction> fDrawFunctions; 

    UInt_t fWidth{800}, fHeight{500}; 

public: 

    UserWindow(const std::string& name, const AppDrawFunction& draw_app_fcn); 

    ~UserWindow(); 

    /// @brief This signal is how the app notifies the UserWindow parent that its 'CloseWindow' signal has been emitted.  
    void WindowClosed(); 


    /// @return 'true' if app is active, 'false' otherwise. 
    bool IsActive() const; 

    /// @brief Activate this user window, if it's deactivated. noop if it's already active. 
    void Activate(); 

    /// @brief Deactivate this user window, if it's active. noop if it's already deactivated. 
    void Deactivate();

    /// @brief Draw the window, if it isn't already drawn. 
    void DrawWindow(); 
    
    /// @brief Draw all objects which are assigned to this user window
    void DrawObjects(); 

    //Add a drawn object to this window. 
    void AddDrawnItem(std::string item_name, const std::function<void(void)>& draw_function);

    // select this window as the active one. 
    void cd(); 

    inline std::string GetName() const { return fName; } 

    //make these functions private later 
    inline void AddPrimitive(TObject* obj) { fPrimitiveList.Add(obj); } 

};

// equivalence operator for two 'UserWindow' instances 
inline bool operator==(const UserWindow& lhs, const UserWindow& rhs) { return lhs.GetName() == rhs.GetName(); } 

#endif