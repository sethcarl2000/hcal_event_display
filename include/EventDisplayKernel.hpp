#ifndef EventDisplayKernel_hpp
#define EventDisplayKernel_hpp

#include <Key.hpp>
#include <DrawFunction.hpp>
#include <UserWindow.hpp>
#include <Frequency_Type.hpp>
// TGUI headers
#include <TGFrame.h>
#include <TGWindow.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TGSlider.h>
#include <TGButton.h>
// ROOT headers
#include <TList.h>
#include <TFile.h>
#include <TTree.h> 
#include <ROOT/RDataFrame.hxx>
#include <TCanvas.h> 
#include <TPad.h> 
// stdlib headers
#include <vector>
#include <string> 
#include <vector> 
#include <memory> 
#include <typeinfo> 
#include <utility> 

//enum to identify different data type requests 
enum class dtype {
    // double 
    d,        
    // array of doubles   
    d_array,
    // unsigned integer
    uint
}; 

//put here a list of classes that we're going to allow to propagate signals to the EventDisplayKernel (not the user!)
class EventGUI; 

class EventGUI; 
class TimeControlPanel;

//implementing this class as a meyer's singleton 
class EventDisplayKernel {
public: 
    enum class AppState { kNone=0, kInit, kActive, kExit };
private: 

    /// this is the kernel's 'Key' which lets the kernel call some methods of other classes that no other class can call.  
    Key<EventDisplayKernel> fMyKey{}; 

    //constructor 
    EventDisplayKernel(); 

    //destructor 
    ~EventDisplayKernel() = default; 

    std::unique_ptr<ROOT::RDataFrame> fDataFrame{nullptr}; 

    //return access to the above ptr (or throw an exception if it's null)
    ROOT::RDataFrame* GetDataFrame(); 

    /// List of branches available in the TTree
    std::vector<std::string> fBranchList{}; 

    AppState fAppState{AppState::kNone}; 

    std::string fFilePath{""}, fTreeName{""}; 

    /// @brief 
    /// @param branch Branch to check for
    /// @return 'true' if branch exists, 'false' otherwise. 
    bool DoesBranchExist(std::string branch) const; 

    //resets each user window's canvases, and draws the current event.  
    void DrawCurrentEvent(); 

    /// @brief Check to seee if the type of a branch matches a given type
    /// @param branch_name name of branch in RDataFrame
    /// @param type std::type_info corresponding to type being tested. 
    /// @return 'true' if type of branch is compatible with type_info, 'false' otherwise. Aborts program if RDataFrame is null, reports error if column is absent. 
    bool BranchTypeMatches(std::string branch_name, const std::type_info& type); 

    
    //std::vector<DrawFunction> fDrawFunctions; 
    
    //list of user-windows to draw
    std::vector<std::unique_ptr<UserWindow>> fUserWindows{}; 

    //the 'event index', which starts from '0' and goes in order until 'n_events-1'.  
    size_t fEventIndex; 
    UInt_t fEventNumber;
    double fTimestamp{0.};  

    ROOT::VecOps::RVec<UInt_t> fEventNumbers; 

    size_t GetEventIndex() const { return fEventIndex; } 

    //Launches GUI 
    void LaunchGUI();

    /// @brief Find event index corresponding to event number
    /// @param event_number event number from ROOT file
    /// @return event index in indexed list of events 
    size_t FindEventIndex(UInt_t event_number); 

    /// @brief The GUI (user's graphical input interface)
    EventGUI* fGUI; 

    // GUI items
    //TGHorizontalFrame *fFrame_canv;      
    //TRootEmbeddedCanvas *fECanvas; 
    TCanvas *fCurrentCanvas{nullptr}; 
    //currently active window
    UserWindow* fCurrentUserWindow{nullptr}; 

    //dimensions of canvas to draw. 
    double fX0{-24.*15./2.}, fY0{-12.*15./2.}, fX1{+24.*15./2.}, fY1{+12.*15./2.}; 

    //this variable stores the name of the current user-defined draw funciton. 
    std::string fCurrentDrawFunction{"none"}; 

    //update the timestamp in the GUI and in each user window.
    void UpdateTimestamp();

public: 

    //delete copy constructor & copy assign. operator 
    EventDisplayKernel(const EventDisplayKernel&) = delete; 
    EventDisplayKernel& operator=(const EventDisplayKernel&) = delete; 

    //Access to single, static instance 
    static EventDisplayKernel& Instance(); 

    /// @return current app state (none, init, active)
    AppState GetAppState() const { return fAppState; }

    /// @return get the current timestamp 
    double GetTimestamp() const { return fTimestamp; }

    // set the input file
    void SetFile(std::string path_file) { fFilePath=path_file; }; 

    // set the tree name
    void SetTreeName(std::string tree_name) { fTreeName=tree_name; }; 

    /// @brief Add a new user window
    /// @param name The name of the window. This must be unique; if another window with the same name has already been added, the program will exit. 
    /// @param width width of the window, in pixels.
    /// @param height height of the window, in pixels.
    void AddUserWindow(std::string window_name, UInt_t width, UInt_t height);
    
    /// @brief Add a a new object drawing function. 
    /// @param window_name name of the window to add it to. the winodow with this name must be added first! 
    /// @param object_name unique name of this drawing routine. If the window 'window_name' already has an object with the same name, the program will exit. 
    /// @param draw_frequency How often to re-run this drawing routine. Frequency::kEachEvent = call this once for every new event. Frequency::kEachTimeStep = call this each time a new event and/or time is set.
    void AddDrawFunction(std::string window_name, std::string object_name, const std::function<void(void)>& fcn, Frequency::Type);

    //Launch interactive app 
    void LaunchApp();

    //Request to fetch a specific branch from the TTree
    template<typename T> T GetData(std::string branch_name); 

    //this must be called for each object you want to draw on the display. 
    void Draw(TObject* object, const char* option=""); 

    //dimensions of canvas to be drawn. 
    void SetCanvasDim(double x0, double y0, double x1, double y1) { fX0=x0; fX1=x1; fY0=y0; fY1=y1; }; 

    /// @brief Returns canvas on which the current objects are being drawn 
    /// @return ptr to canvas on which objects are being drawn. nullptr if it does not exist
    TCanvas* GetCanvas(); 

    //the 'event number', as it appears in the rootfile
    UInt_t GetEventNumber() const { return fEventNumber; } 

    UInt_t GetEventNumber(size_t index); 

    /// @return total number of events loaded from the current file. 
    size_t GetNEvents() const { return fEventNumbers.size(); };

    // These are methods that the user _cannot_ invoke, so we require that classes with explicit permission pass us a 'signal key' object
    // (which only those classes can construct).
    //
    //methods that connect to GUI buttons 
    void DoNextEvent(Key<EventGUI>); 
    void DoPrevEvent(Key<EventGUI>);  
    void CloseApp(Key<EventGUI>);
    void DoDrawTimestamp(Key<EventGUI>, double timestamp);
    void DoToggleWindow(Key<EventGUI>, std::string window_name, bool is_active);

    void DoSetTimestamp(Key<TimeControlPanel>, double timestamp);

    // checek the status of each user window, and update the GUI accordingly.
    void SetWindowStatus(); 
    
    /// @brief Draw requested event index. abort app if illegal request is made. 
    /// @tparam T The class which is allowed to call this method (EventGUI or EventDisplayKernel ONLY)
    /// @param event_index the index of event to draw. 
    template<typename T> void SetEventIndex(Key<T>, size_t event_index); 

    // methods that connect to UserWindow
    void SetCanvas(Key<UserWindow>, TCanvas* canv) { fCurrentCanvas=canv; }
    void SetUserWindow(Key<UserWindow>, UserWindow* window) { fCurrentUserWindow=window; }

}; 




#endif