
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

// *** see below for implementations of each of these functions *** 
//
// Technical note: it's also OK to pass a lambda fcn to the kernel as your draw function. 
// anything that can be converted to a std::function<void(void)> type is allowed. 

//draw energy of goodblocks
void draw_goodblock_energy();

//draw hcal frame
void draw_hcal_frame();

//draw time / amplitude of each goodblock hit 
void draw_time_hist();

//___________________________________________________________________________________________________________
void test_modular_windows()
{
    // get access to the kernel
    auto& kernel = EventDisplayKernel::Instance(); 

    // Here, we add our first window. let's call it 'hcal'. it will be a 2d display of all hcal blocks. 
    //
    // Fist, we call 'AddUserWindow' to add the window itself. 
    // this will be its own separate frame, which we can draw / hide at will. 
    //
    kernel.AddUserWindow("hcal", 500, 800);

    //kernel.AddUserSlider("time", "time_cut", 0, 50); 
    kernel.SetParameter<double>("time_cut", 10.); 

    // Now, we have to add draw funcitons to the window. make sure the name of the window (1st arg) matches! 
    // This first function is just a 'frame', meaning 
    kernel.AddDrawFunction("hcal", "frame", draw_hcal_frame, Frequency::Type::kEachEvent);

    // Now, we add the 'goodblocks' variables (their energies). 
    kernel.AddDrawFunction("hcal", "goodblock_e", draw_goodblock_energy, Frequency::Type::kEachTimeStep);

    kernel.AddUserSlider("hcal", "time_cut", 0, 50); 

    // tell the kernel we want a 'time slider', and also specify what we want the time-range to be. 
    kernel.AddTimeSlider(-100, +150.); 

    // Now, we add our second window. We will call this 'time' as it will show the times / amplitudes of all the 'goodblocks' in our event. 
    //
    //
    kernel.AddUserWindow("time", 900, 400);

    // Next, we add the time of each blocks arrival to the window. 
    kernel.AddDrawFunction("time", "goodblock_atime", draw_time_hist, Frequency::Type::kEachTimeStep);
    

    // tell the kernel about the ROOT file we want to look at. 
    kernel.SetFile("e1209016_fullreplay_3013_stream0_2_seg1_1.root");

    // tell the kernel the name of the TTree within the file to find.  
    kernel.SetTreeName("T"); 



    // launch the interactive app. 
    // Any commands in this macro after this one will be ignored!  
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
    
    double time_cut = kernel.GetParameter<double>("time_cut"); 

    //const double time_cut = timing_window_size/2.; // +/- this value, blocks are kept (ns) 

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

        // energy normalized in the range 0-1
        double norm_energy = (energy - min_energy)/(max_energy - min_energy); 
        if (norm_energy > 1.) norm_energy =1.;
        if (norm_energy < 0.) norm_energy =0.; 


        // the way a TBox is drawn is as follows: 
        /*
        *       -------------(x2,y2)
        *       |               |
        *       |               |
        *       |               |
        *       |               |
        *    (x1,y1)------------- 
        * 
        */
        auto block = new TBox(
            col-norm_energy/2., row-norm_energy/2.,    //x1,y1
            col+norm_energy/2., row+norm_energy/2.     //x2,y2
        ); 

        // Draw the frame for each block that's part of 'goodblocks'
        
        auto color = GetGreyscale(norm_energy); 
        // set the transparency of the block according to the energy 
        block->SetFillColor(kBlack);
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

        block_frame->SetFillStyle(0);
        block_frame->SetLineColor(kBlack); 
        block_frame->SetLineWidth(1);
        block_frame->SetLineStyle(kDashed);  
        kernel.Draw(block_frame, "SAME");

    }
}
//___________________________________________________________________________________________________________
void draw_time_hist()
{
    auto& kernel = EventDisplayKernel::Instance();

    auto hist = new TH1D("h_time", "H.Cal 'goodblocks' time;block time (ns);block energy (MeV)", 
        100, //number of bins  
        kernel.GetMinTimestamp(), //min timestamp  
        kernel.GetMaxTimestamp()  //max timestamp 
    );
    //kernel.Draw(hist, "HIST");

    const br_array<double>& blocks_time = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.atime");
    const br_array<double>& blocks_e    = kernel.GetData<br_array<double>>("sbs.hcal.goodblock.e");

    for (size_t i=0; i<blocks_time.size(); i++) { hist->Fill(blocks_time[i], 1000.*blocks_e[i]); }

    kernel.Draw(hist, "HIST");

    if (kernel.GetAppState() != EventDisplayKernel::AppState::kActive) return;

    double time_cut = kernel.GetParameter<double>("time_cut"); 

    double max = hist->GetMaximum(); 
    double timestamp = kernel.GetTimestamp();
    auto box = new TBox(timestamp - time_cut, 0., timestamp + time_cut, max);
    box->SetFillColor(kRed);
    box->SetFillStyle(3004);
    kernel.Draw(box);


    auto coinc_line = new TLine(kernel.GetTimestamp(), 0., kernel.GetTimestamp(), hist->GetMaximum());
    coinc_line->SetLineColor(kRed); 
    kernel.Draw(coinc_line);

    return;
    
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



