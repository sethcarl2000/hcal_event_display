
//#include "draw_basic_window.h"

#include <EventDisplayKernel.hpp>
#include <UserWindow.hpp>
// ROOT headers
#include <TBox.h>
#include <TStyle.h> 
#include <ROOT/RVec.hxx>
#include <TColor.h>
#include <TH2D.h> 
#include <TAxis.h> 
#include <TLine.h> 
// stdlib headers
#include <cmath> 
#include <vector>
#include <memory> 

namespace {
    constexpr double timing_window_size = 20.; //size of the timing window, in ns.
};

// given val from 0-1, return greyscale TColor
Color_t GetGreyscale(double val); 


template<typename T> using br_array = ROOT::VecOps::RVec<T>; 

//draw energy of goodblocks
void draw_goodblock_energy();

//draw hcal frame
void draw_hcal_frame();

//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
void draw_time_hist();
//___________________________________________________________________________________________________________
void test_modular_windows()
{
    // get access to the kernel
    auto& kernel = EventDisplayKernel::Instance(); 

    // add our first user window (hcal)
    kernel.AddUserWindow("hcal", 500, 800);

    // add our first drawn item
    kernel.AddDrawFunction("hcal", "frame", draw_hcal_frame, Frequency::Type::kEachEvent);

    kernel.AddDrawFunction("hcal", "goodblock_e", draw_goodblock_energy, Frequency::Type::kEachTimeStep);

    // Add the 'kernel time hist' 
    kernel.AddUserWindow("time", 900, 400);

    kernel.AddDrawFunction("time", "goodblock_atime", draw_time_hist, Frequency::Type::kEachTimeStep);
    
    // tell the kernel about the ROOT file we want to look at
    kernel.SetFile("e1209016_fullreplay_3013_stream0_2_seg1_1.root"); 
    kernel.SetTreeName("T"); 

    // launch the interactive app 
    kernel.LaunchApp(); 
}

//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
void draw_hcal_frame()
{
    auto th2d = new TH2D("h_2d", "H.Cal 'goodblocks';column;row;block energy (MeV)", 12,-0.5,12-0.5, 24,-0.5,24-0.5);
    th2d->GetZaxis()->SetRangeUser(0, 200); 
    th2d->SetStats(false);

    auto& kernel = EventDisplayKernel::Instance(); 
    kernel.Draw(th2d, "colz"); 
}
//___________________________________________________________________________________________________________
void draw_goodblock_energy()
{
    //get access to the kernel 
    auto& kernel = EventDisplayKernel::Instance(); 

    //get our data for this event
    
    const double time_cut = timing_window_size/2.; // +/- this value, blocks are kept (ns) 

    //get the current-selected time. 
    double timestamp = kernel.GetTimestamp();

    //energy 
    br_array<double> blocks_e   = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.e");  

    //row & column 
    br_array<double> blocks_row     = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.row");
    br_array<double> blocks_col     = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.col");   
    br_array<double> blocks_time    = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.atime");

    //now, loop over all of them, and draw a 'box' for each block

    int n_blocks = blocks_e.size(); 

    double max_energy = 0.05; //GEV
    double min_energy = 0.00; 

    for (int i=0; i<n_blocks; i++) {

        if (std::fabs(blocks_time[i] - timestamp) > time_cut) continue;
        // Now, draw the fill of the box based on how much energy there is. 

        // get the row/column for this block
        double row = blocks_row[i]; 
        double col = blocks_col[i];

        // get the energy of this block 
        double energy = blocks_e[i]; 

        auto block = new TBox(
            col-0.5,row-0.5,    //x1,y1
            col+0.5,row+0.5     //x2,y2
        ); 

        // Draw the frame for each block that's part of 'goodblocks'

        
        // energy normalized in the range 0-1
        double norm_energy = (energy - min_energy)/(max_energy - min_energy); 
        if (norm_energy > 1.) norm_energy =1.;
        if (norm_energy < 0.) norm_energy =0.; 

        auto color = GetGreyscale(norm_energy); 
        // set the transparency of the block according to the energy 
        block->SetFillColor(color);
        block->SetLineColor(kBlack); 

        
        //set bounds on the energy 
        //std::printf("block (row/col, energy, color): %2i/%2i, %5.3f, %i\n", (int)row,(int)col, energy, (int)color); 

        //we call this to tell the kernel to draw this object on the canvas (fresh for each event)
        kernel.Draw(block, "SAME"); 

        
        // now, draw another block do show where the 'gooblocks' are: 
        auto block_frame =  new TBox(
            col-0.5,row-0.5,    //x1,y1
            col+0.5,row+0.5     //x2,y2
        ); 

        block_frame->SetFillColor(kBlack);
        block_frame->SetFillStyle(3004);
        block_frame->SetLineColor(kBlack); 
        block_frame->SetLineWidth(1); 
        kernel.Draw(block_frame, "SAME");

    }
}
//___________________________________________________________________________________________________________
void draw_time_hist()
{
    auto hist = new TH1D("h_time", "H.Cal 'goodblocks' time;block time (ns);block energy (MeV)", 50, -100., +150.);
    auto& kernel = EventDisplayKernel::Instance();
    //kernel.Draw(hist, "HIST");

    const br_array<double>& blocks_time = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.atime");
    const br_array<double>& blocks_e    = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.e");

    for (size_t i=0; i<blocks_time.size(); i++) { hist->Fill(blocks_time[i], 100.*blocks_e[i]); }

    kernel.Draw(hist, "HIST");

    if (kernel.GetAppState() != EventDisplayKernel::AppState::kActive) return;

    double max = hist->GetMaximum(); 
    double timestamp = kernel.GetTimestamp();
    auto box = new TBox(timestamp-timing_window_size/2., 0., timestamp+timing_window_size/2., max);
    box->SetFillColor(kRed);
    box->SetFillStyle(3004);
    kernel.Draw(box);
    return;
    
    /*auto coinc_line = new TLine(kernel.GetTimestamp(), 0., kernel.GetTimestamp(), hist->GetMaximum());
    coinc_line->SetLineColor(kRed); 
    kernel.Draw(coinc_line);*/ 
}
//___________________________________________________________________________________________________________
Color_t GetGreyscale(double val) {
    constexpr Color_t min_color =19;
    constexpr Color_t max_color =12;
    constexpr short color_span = min_color - max_color;

    short color = min_color - ((short)(val * color_span));

    if (color > min_color) color = min_color;
    if (color < max_color) color = max_color;

    return color; 
    /*
    static constexpr int n_colors=100; 
    static std::vector<TColor> color_list;

    if (color_list.empty()) {
        for (int i=0; i<n_colors; i++) {
            double mag = ((double)i)/((double)n_colors-1);
            color_list.emplace_back(mag,mag,mag);
        }
    }

    int index = (int)val*n_colors; 
    if (index < 0) index =0;
    if (index > n_colors-1) index =n_colors-1; 
    
    return &color_list.at(index); */ 
}



