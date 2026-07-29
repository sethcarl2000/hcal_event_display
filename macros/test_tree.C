
// ROOT headers
#include <ROOT/RDataFrame.hxx> 
#include <ROOT/RVec.hxx> 
#include <TFile.h>
#include <TTree.h> 
// stdlib headers
#include <iostream> 
#include <memory> 

void test_tree()
{

    auto file = std::make_unique<TFile>("e1209016_fullreplay_3013_stream0_2_seg1_1.root", "READ"); 

    // get the main data tree
    auto tree = file->Get<TTree>("T");

    ULong64_t n_entries = tree->GetEntries(); 

    std::cout << "Entries: " << n_entries << "\n"; 

    using array_d = ROOT::RVec<double>; 

    constexpr bool kOff{false}, kOn{true};

    tree->SetBranchStatus("*", kOff);
    
    array_d br_goodblock_e; 

    tree->SetBranchStatus("sbs.hcal.goodblock.e", kOn); 
    tree->SetBranchAddress("sbs.hcal.goodblock.e", &br_goodblock_e); 

    for (ULong64_t i=0; i<n_entries; i++) {

        tree->GetEntry(i); 
        
        std::cout << "loaded entry: " << i << ", sbs.hcal.goodblock.e array has " << br_goodblock_e.size() << " entries.\n"; 
    }

    file->Close(); 
}