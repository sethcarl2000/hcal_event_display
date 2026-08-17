#ifndef UserWindow_hpp
#define UserWindow_hpp

#include <DrawFunction.hpp>
#include <UserApp.hpp> 
#include <Key.hpp> 
#include <Frequency_Type.hpp>
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

    /// this is the kernel's 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<UserWindow> fMyKey{}; 

    //one is for all the primitives which are persistent per-event, and the other is for all primitives persistent for a given timestep only. 
    TList fPrimitiveList_event{}, fPrimitiveList_timestep{}; 

    //'name', which uniquely identifies this window. 
    std::string fName; 

    //the 'app' this window actually owns
    UserApp* fApp{nullptr}; 

    std::vector<DrawFunction> fDrawFunctions_event{}; 
    std::vector<DrawFunction> fDrawFunctions_timestep{}; 

    UInt_t fWidth, fHeight; 

    Frequency::Type fCurrentDrawFunctionFrequency{Frequency::Type::kEachEvent}; 

    std::string fCurrentDrawFunctionName{"none"};

    void DrawObjects(Frequency::Type); 

    // make these functions private later 
    void AddPrimitive(TObject* obj, Frequency::Type type) { 
        if (type==Frequency::Type::kEachEvent) {
            fPrimitiveList_event.Add(obj); 
        } else {
            fPrimitiveList_timestep.Add(obj);
        }
    } 

public: 

    // Default constructor. We add a 'kernel key' as an argument,
    // because it's an object that only the kernel can create. 
    UserWindow(Key<EventDisplayKernel>, std::string name, UInt_t width, UInt_t height) 
        : fName{name}, fWidth{width}, fHeight{height}
    {}; 

    //default copy-constructor (delete this, so we never accidentally make copies of any user windows!)
    UserWindow(const UserWindow&) = delete; 

    ~UserWindow(); 

    // select this window as the active one. 
    void cd(); 

    /// @return 'true' if app is active, 'false' otherwise. 
    bool IsActive() const; 

    /// @return the unique name of this window
    std::string GetName() const { return fName; } 

    /// @return The window-unique name of the drawing function
    std::string GetCurrentDrawFunctionName() const { return fCurrentDrawFunctionName; }

    /// @return The current draw function frequnecy 'type' (kEachEvent or kEacTimeStamp)
    Frequency::Type CurrentDrawFunctionFreqeuncy() const { return fCurrentDrawFunctionFrequency; } 

    /////////////////////////
    //
    // This is a list of signals that only EventDisplayKernel can emit
    //
    //

    //Add a drawn object to this window. 
    void AddDrawnItem(Key<EventDisplayKernel>, DrawFunction draw_function, Frequency::Type); 

    /// @brief Activate this user window, if it's deactivated. noop if it's already active. 
    /// see implemenation for a list of valid senders. 
    void DoActivate(Key<EventDisplayKernel>);  

    /// @brief Deactivate this user window, if it's active. noop if it's already deactivated. 
    void DoDeactivate(); 

    /// @brief Draw all objects fresh for a new event. 
    void DoDrawEvent(Key<EventDisplayKernel>) { DrawObjects(Frequency::Type::kEachEvent); } 

    /// @brief Draw all objects fresh for a new timestamp. 
    void DoDrawTimestamp(Key<EventDisplayKernel>) { DrawObjects(Frequency::Type::kEachTimeStep); } 

    /// @brief Add drawn TObject to list of drawn TObjects
    /// @param obj 
    void AddPrimitive(Key<EventDisplayKernel>, TObject* obj) { AddPrimitive(obj, CurrentDrawFunctionFreqeuncy()); }
};

// equivalence operator for two 'UserWindow' instances 
inline bool operator==(const UserWindow& lhs, const UserWindow& rhs) { return lhs.GetName() == rhs.GetName(); } 

#endif