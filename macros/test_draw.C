
#include <EventDisplayKernel.hpp>
// ROOT headers
#include <TBox.h>
#include <TStyle.h> 
#include <ROOT/RVec.hxx>
// stdlib headers
#include <cmath> 

template<typename T> using br_array = ROOT::VecOps::RVec<T>; 

//___________________________________________________________________________________________________________
void draw_goodblock_energy()
{
    //get access to the kernel 
    auto& kernel = EventDisplayKernel::Instance(); 

    //get our data for this event
    
    //energy 
    br_array<double> blocks_e   = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.e");  

    //row & column 
    br_array<double> blocks_row = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.row");
    br_array<double> blocks_col = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.col");   

    //now, loop over all of them, and draw a 'box' for each block

    int n_blocks = blocks_e.size(); 

    double max_energy = 0.2; 
    double min_energy = 0.0; 

    for (int i=0; i<n_blocks; i++) {

        // Now, draw the fill of the box based on how much energy there is. 

        // get the row/column for this block
        double row = blocks_row[i]; 
        double col = blocks_col[i];

        // get the energy of this block 
        double energy = blocks_e[i]; 

        auto block = new TBox(row-0.5,col-0.5, row+0.5,col+0.5); 

        // Draw the frame for each block that's part of 'goodblocks'
        block->SetLineColor(kBlack);
        //set bounds on the energy 

        // energy normalized in the range 0-1
        double norm_energy = (energy - min_energy)/(max_energy - min_energy); 
        if (norm_energy > 1.) norm_energy =1.;
        if (norm_energy < 0.) norm_energy =0.; 

        // set the transparency of the block according to the energy 
        block->SetFillColorAlpha(kBlack, norm_energy); 

        //we call this to tell the kernel to draw this object on the canvas (fresh for each event)
        kernel.Draw(block, "SAME"); 
    }
}
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________

//___________________________________________________________________________________________________________
void test_draw()
{
    // get access to the kernel
    auto& kernel = EventDisplayKernel::Instance(); 

    // set the dimensions of the canvas we want to draw
    kernel.SetCanvasDim(-0.5,-0.5, 12-0.5,24-0.5); 

    // tell the kernel about our function that draws blocks
    kernel.AddDrawnItem("goodblock", draw_goodblock_energy);
    
    // tell the kernel about the ROOT file we want to look at
    kernel.SetFile("e1209016_fullreplay_3013_stream0_2_seg1_1.root"); 
    kernel.SetTreeName("T"); 

    // launch the interactive app 
    kernel.LaunchApp(); 
}
