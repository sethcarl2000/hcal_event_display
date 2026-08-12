#ifndef EventDisplayKernel_hpp
#define EventDisplayKernel_hpp

#include <DrawFunction.hpp>
#include <UserWindow.hpp>
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

//enum to identify different data type requests 
enum class dtype {
    // double 
    d,        
    // array of doubles   
    d_array,
    // unsigned integer
    uint
}; 

//implementing this class as a meyer's singleton 
class EventDisplayKernel : public TGMainFrame {
public: 
    enum class AppState { kNone=0, kInit, kActive };
private: 

    //constructor 
    EventDisplayKernel(); 

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
    std::vector<UserWindow> fUserWindows; 

    //the 'event index', which starts from '0' and goes in order until 'n_events-1'.  
    size_t fEventIndex; 
    UInt_t fEventNumber; 

    ROOT::VecOps::RVec<UInt_t> fEventNumbers; 


    inline size_t GetEventIndex() const { return fEventIndex; } 

    //Launches GUI 
    void LaunchGUI(UInt_t w, UInt_t h);

    // 
    void DrawEventIndex(size_t event_index); 

    /// @brief Find event index corresponding to event number
    /// @param event_number event number from ROOT file
    /// @return event index in indexed list of events 
    size_t FindEventIndex(UInt_t event_number); 


    // GUI items
    //TGHorizontalFrame *fFrame_canv;      
    //TRootEmbeddedCanvas *fECanvas; 
    TCanvas *fCurrentCanvas{nullptr}; 
    //currently active window
    UserWindow* fCurrentUserWindow{nullptr}; 

    std::string fCurrentObjDrawFunctionName{"none"}, fCurrentAppDrawFunctionName{"none"}; 

    TGHorizontalFrame *fFrame_buttons; 
    TGTextButton *fGButton_next, *fGButton_prev; 
    TGLabel *fGLabel_eventNumber; 

    //dimensions of canvas to draw. 
    double fX0{-24.*15./2.}, fY0{-12.*15./2.}, fX1{+24.*15./2.}, fY1{+12.*15./2.}; 

    //this variable stores the name of the current user-defined draw funciton. 
    std::string fCurrentDrawFunction{"none"}; 

    friend class PrivateMessenger; 

public: 

    //destructor 
    ~EventDisplayKernel(); 

    //delete copy constructor & copy assign. operator 
    EventDisplayKernel(const EventDisplayKernel&) = delete; 
    EventDisplayKernel& operator=(const EventDisplayKernel&) = delete; 

    //Access to single, static instance 
    static inline EventDisplayKernel& Instance() { 
        static EventDisplayKernel instance; 
        return instance; 
    }

    /// @return current app state (none, init, active)
    inline AppState GetAppState() const { return fAppState; }

    // set the input file
    inline void SetFile(std::string path_file) { fFilePath=path_file; }; 

    // set the tree name
    inline void SetTreeName(std::string tree_name) { fTreeName=tree_name; }; 

    //Launch interactive app 
    void LaunchApp();

    //Request to fetch a specific branch from the TTree
    template<typename T> T GetData(std::string branch_name); 

    //this must be called for each object you want to draw on the display. 
    void Draw(TObject* object, const char* option=""); 

    //dimensions of canvas to be drawn. 
    inline void SetCanvasDim(double x0, double y0, double x1, double y1) { fX0=x0; fX1=x1; fY0=y0; fY1=y1; }; 

    /// @brief Returns canvas on which the current objects are being drawn 
    /// @return ptr to canvas on which objects are being drawn. nullptr if it does not exist
    TCanvas* GetCanvas(); 

    //the 'event number', as it appears in the rootfile
    inline UInt_t GetEventNumber() const { return fEventNumber; } 

    //methods that connect to GUI buttons
    void DoNextEvent(); 
    void DoPrevEvent(); 

    void CloseWindow(); 

}; 




#endif