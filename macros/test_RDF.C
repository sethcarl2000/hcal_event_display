
// ROOT headers
#include <ROOT/RDataFrame.hxx> 
#include <ROOT/RVec.hxx> 
#include <TFile.h>
#include <TTree.h> 
// stdlib headers
#include <iostream> 
#include <memory> 
#include <vector> 

void test_RDF()
{

    ROOT::RDataFrame df("T", "e1209016_fullreplay_3013_stream0_2_seg1_1.root"); 

    
    using array_d = ROOT::RVec<double>; 

    constexpr bool kOff{false}, kOn{true};

    //get the branches we want 

    ROOT::RDF::RResultPtr<std::vector<array_d>> rptr_goodblock_e = df.Take<array_d>("sbs.hcal.goodblock.e"); 
    // other branches... 

    ULong64_t n_entries = *df.Count(); 

    std::cout << "Entries: " << n_entries << "\n"; 

    for (ULong64_t i=0; i<n_entries; i++) {

        const auto& goodblock_e = (*rptr_goodblock_e)[i]; 

        std::cout << "loaded entry: " << i << ", sbs.hcal.goodblock.e array has " << goodblock_e.size() << " entries.\n"; 
    }

}