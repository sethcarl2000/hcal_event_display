#ifndef EventDisplayKernel_hpp
#define EventDisplayKernel_hpp

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
// stdlib headers
#include <vector>
#include <string> 
#include <functional> 
#include <vector> 
#include <memory> 

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
private: 

    //constructor 
    EventDisplayKernel(); 

    //destructor 
    ~EventDisplayKernel(); 

    //list of all drawn primitives
    TList fPrimitiveList; 

    //File & tree to get our data 
    std::unique_ptr<TFile> fInputFile; 
    TTree *fTree; 

    
    enum class AppState { kNone=0, kInit, kActive };
    AppState fAppState{AppState::kNone}; 

    std::string fFilePath{""}, fTreeName{""}; 

    /// @brief checks status of file and TTree
    /// @return 'true' if file and tree appear to be open & active, 'false' otherwise
    bool IsFileOpen() const; 

    /// @brief 
    /// @param branch Branch to check for
    /// @return 'true' if branch exists, 'false' otherwise. 
    bool DoesBranchExist(std::string branch) const; 


    struct draw_fcn_and_state_t { 
        std::function<void(void)> fcn; 
        bool is_active{false}; 
    };
    std::vector<draw_fcn_and_state_t> fFunctions; 

public: 

    //delete copy constructor & copy assign. operator 
    EventDisplayKernel(const EventDisplayKernel&) = delete; 
    EventDisplayKernel& operator=(const EventDisplayKernel&) = delete; 

    //Access to single, static instance 
    static inline EventDisplayKernel& Instance() { 
        static EventDisplayKernel instance; 
        return instance; 
    }

    // set the input file
    inline void SetFile(std::string path_file) { fFilePath=path_file; }; 

    // set the tree name
    inline void SetTreeName(std::string tree_name) { fTreeName=tree_name; }; 

    //Launch interactive app 
    void LaunchApp();

    //Add drawn objects. 
    void AddDrawnItem(std::string item_name, std::function<void(void)> draw_function);

    //Request to fetch a specific branch from the TTree
    template<typename T, dtype DataType> T GetData(std::string branch_name, bool is_array=false); 


    ClassDef(EventDisplayKernel,1); 
}; 


#endif