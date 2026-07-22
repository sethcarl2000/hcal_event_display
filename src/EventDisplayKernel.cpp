#include <EventDisplayKernel.hpp>
// ROOT headers
#include <TError.h> 
#include <TObjArray.h>
#include <TObject.h>
#include <TBranch.h>
#include <TString.h>
// stdlib headers
#include <stdexcept> 
#include <cstdlib> 
#include <cstdio> 
#include <map> 

namespace inst_helpers {
    template<typename T> std::string GetBranchCode(); 
}

//________________________________________________________________________________________________
EventDisplayKernel::EventDisplayKernel()
{
    //set the app state
    fAppState = AppState::kInit; 
}
//________________________________________________________________________________________________
void EventDisplayKernel::LaunchApp()
{
    //try and open the TFile

    try {
        fInputFile = std::make_unique<TFile>(fFilePath, "READ"); 

        if (!IsFileOpen()) throw std::logic_error("failed to open TFile");
    
        fTree = fInputFile->Get<TTree>(fTreeName.c_str()); 

        if (!fTree) throw std::logic_error("Unable to get tree from file");
        
    } catch (const std::exception& e) {
        Error(__func__, "Something went wrong trying to open the file / tree.\n"
            " file:     %s\n"
            " tree:     %s\n"
            " what():   %s", fFilePath.c_str(), fTreeName.c_str(), e.what()); 
        std::exit(1);
    }

    // do stuff that actually launches the GUI app...   
    // 
    // 
    // ...

    fAppState = AppState::kActive; 
}
//________________________________________________________________________________________________
bool EventDisplayKernel::IsFileOpen() const
{
    // check status of TFile
    if (!fInputFile || fInputFile->IsZombie() || !fInputFile->IsOpen()) return false;
    
    return true; 
}
//________________________________________________________________________________________________
template<typename T, dtype DataType> T EventDisplayKernel::GetData(std::string branch_name, bool is_array) 
{
    static std::map<std::string,T> fBranches; 

    static_assert(std::is_trivially_constructible_v<T>, "Template arg 'T' is not trivially constructable");

    //first, check the status of the app. 
    switch (fAppState) {
        case AppState::kNone; return T{};

        case AppState::kActive : { // We're in the 'run' phase 
            
            //we're in the run phase. -------------------------------------------------------------------
            if (!fTree) {
                throw std::logic_error("in <GetData(run phase)>: TTree is null"); 
                return T{};
            }

            //check if branch exists
            if (!DoesBranchExist(branch_name)) {
                throw std::logic_error(Form("in <GetData(run phase)>: Branch '%s' does not exist in tree '%s'",branch_name.c_str(),fTreeName.c_str())); 
                return T{}; 
            }

            auto find_it = fBranches.find(branch_name);
            if (find_it != fBranches.end()) {

                //we've found our value. return it: 
                return find_it->second; 

            } else {

                throw std::logic_error(Form("in <GetData(run phase)>: Requested branch '%s' appears in tree, but not in app's list of branches. (this shouldn't be possible...)",branch_name.c_str())); 
                return T{};
            }
            break; 
        }

        //we're in the initialization phase. ---------------------------------------------------------------
        case AppState::kInit {

            //check if the TTree is ok 
            if (!fTree) {
                throw std::logic_error("in <GetData(init phase)>: TTree is null"); 
                return T{};
            }

            //check if branch exists
            if (!DoesBranchExist(branch_name)) {
                throw std::logic_error(Form("in <GetData(init phase)>: Branch '%s' does not exist in tree '%s'",branch_name.c_str(),fTreeName.c_str())); 
                return T{}; 
            }

            auto find_it = fBranches.find(new_branch); 
            if (find_id == fBranches.end()) {

                //add a new element to the map corresponding to this branch. 
                fBranches.insert({branch_name, T{}}); 

                //turn this branch 'on' 
                fTree->SetBranchStatus(branch_name.c_str(), 1); 

                //set the address of this branch to our new map element
                fTree->SetBranchAddress(branch_name.c_str(), &fBranches[new_val]);
            
                //let the user know this branch was added successfully 
                std::printf("in <EventDisplayKernel::GetData(init)>: Requested branch '%s' added.\n", branch_name.c_str());
            }
            break; 
        } 

    }
}
//________________________________________________________________________________________________
bool EventDisplayKernel::DoesBranchExist(std::string branch_name) const 
{
    //check if the TTree is ok 
    if (!fTree) {
        throw std::logic_error("in <DoesBranchExist>: TTree is null"); 
        return false;
    }

    auto b_list = fTree->GetListOfBranches(); 
    for (auto it=b_list->begin(); it != b_list->end(); ++it) {

        auto branch = dynamic_cast<TBranch*>(*it); 
        if (branch->GetName() == branch_name) return true; 
    }

    return false; 
}
//________________________________________________________________________________________________
//explicity instantiate valid templates
template double EventDisplayKernel::GetData<double,dtype::d>(std::string branch_name, bool is_array);
//
template unsigned int EventDisplayKernel::GetData<unsigned int,dtype::uint>(std::string branch_name, bool is_array);
//
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________
//________________________________________________________________________________________________

template<> std::string inst_helpers::GetBranchCode<double>() { return "F"; }