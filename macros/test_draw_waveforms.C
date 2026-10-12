
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
#include <TString.h> 
#include <TGraph.h> 
// stdlib headers
#include <cmath> 
#include <vector>
#include <memory> 
#include <stdexcept> 

namespace {
    constexpr double timing_window_size = 20.; //size of the timing window, in ns.

    constexpr double max_waveform_energy = 0.1; 
    constexpr double min_waveform_energy = 0.0;

    constexpr int n_samples = 50; 
    constexpr int n_blocks  = 12*24;
};

// given val from 0-1, return greyscale TColor
Color_t GetGreyscale(double val); 


template<typename T> using br_array = ROOT::VecOps::RVec<T>; 

//draw energy of goodblocks
void draw_waveforms();

//draw hcal frame
void draw_hcal_frame();

//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
//___________________________________________________________________________________________________________
void draw_waveform_profile_timestamp(); 
//___________________________________________________________________________________________________________
void draw_waveform_profile(); 
//___________________________________________________________________________________________________________
void draw_time_hist();
//___________________________________________________________________________________________________________
void test_draw_waveforms()
{
    // get access to the kernel
    auto& kernel = EventDisplayKernel::Instance(); 


    kernel.AddUserSlider("max_amplitude", 0, 0.5); 

    // add our first user window (hcal)
    kernel.AddUserWindow("hcal", 500, 800);


    // add our first drawn item
    kernel.AddDrawFunction("hcal", "frame", draw_hcal_frame, Frequency::Type::kEachEvent);

    kernel.AddDrawFunction("hcal", "waveform_amplitude", draw_waveforms, Frequency::Type::kEachTimeStep);

    // Add the 'kernel time hist' 
    kernel.AddUserWindow("time", 900, 400);

    kernel.AddDrawFunction("time", "goodblock_atime", draw_time_hist, Frequency::Type::kEachTimeStep);

    // Add the waveform profile window
    kernel.AddUserWindow("Waveform Profile", 900, 400); 

    kernel.AddDrawFunction("Waveform Profile", "Amplitude sum", draw_waveform_profile, Frequency::Type::kEachTimeStep); 
    

    // tell the kernel we want a 'time slider', and also specify what we want the time-range to be. 
    kernel.AddTimeSlider(0., 50.); 

    // tell the kernel about the ROOT file we want to look at
    kernel.SetFile("data/e1209016_replayed_2034_stream0_2_seg0_0_firstevent0_nevent10000.root"); 
    kernel.SetTreeName("T"); 

    // launch the interactive app 
    kernel.LaunchApp(); 
}

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
void draw_waveforms()
{
    // This should be the anatomy of any given script: 

    // First, get access to the kernel:                          
    auto& kernel = EventDisplayKernel::Instance();  
    // Note here the type declaration: auto& kernel = .... 
    // This means that this is a _reference_ to the kernel object. 
    // Since the EventDisplayKernel is implemented as a Meyer's singleton,
    // this is the typical pattern. In words: 
    //      Everyone is allowed to get a modifiable _reference_ to the _single_ kernel object. 
    //      This is opposed to singleton-object which is accessible via a global ptr, because 
    //      a user might accidentally dereference or delete the EDK singleton! This is not possible
    //      if the users only have a _reference_ to the EDK, rather than a _pointer_. 


    // Next, we're going to request some parameters.
    
    // maximum cutoff for waveform-amplitude to draw. s
    double max_amplitude = kernel.GetParameter<double>("max_amplitude");

    //get the current-selected time. 
    double timestamp = kernel.GetTimestamp();


    // Next, we want to get access to the data. 
    // waveform samples
    const br_array<double>& waveform_samps  = kernel.GetData<br_array<double>>("sbs.hcal.samps");

    // IMPORTANT ________________________________________________________________________________________________________
    // Now that we have our branches and parameters, this next line is very important:

    if (!kernel.IsActive()) return;     

    // Basically, the kernel will first execute this user function 'draw_waveforms' in 'init' mode. 
    // 'Init' mode happens before anything is drawn (Before you even see the EDK GUI!)
    // The point of 'Init' mode is that the kernel is building a list of all branches you want to 
    // access, and all user parameters. 
    // However, nothing is drawn yet, so if you try to access canvases / histograms, you might 
    // run into problems with them being deleted as soon as you tell the kernel about them. 
    // To avoid this, we tell the kernel that it should quit this function if it isn't in 'Active' (drawing) mode. 
    // 
    // Basically, we're saying to the kernel: 
    //      Hi EDK, I've given you the list of all the branches / parameters I want for this fcn. 
    //      If you aren't drawing anything right now, you can go ahed and quit execution of this function for now, 
    //      and come back later when you're ready to start drawing stuff. 


    //now, loop over all of them, and draw a 'box' for each block
    if (waveform_samps.size() != n_samples*n_blocks && !kernel.IsActive()) {
        throw std::runtime_error(Form("in <draw_waveforms>:sbs.hcal.samps array has wrong size! expected 288x50, got %zi", waveform_samps.size())); 
        return;
    }
    
    const double time_cut = timing_window_size/2.; // +/- this value, blocks are kept (ns) 

   
    // first waveform sample 
    if (timestamp < 0) timestamp = 0;
    if (timestamp > (double)n_samples-1) timestamp = (double)n_samples-1;

    int time_index = (int)std::floor(timestamp);

    double time_interpolate_factor = timestamp - (double)time_index;

    int starting_index = time_index;

    for (int i=0; i<n_blocks; i++) {

        int index_last = i*n_samples + starting_index;
        int index_next = index_last + 1; 
        if (index_next >= (int)waveform_samps.size()) index_next = index_last; //if we're at the last sample, just use the last sample again.

        double amplitude_last = waveform_samps.at(index_last);
        double amplitude_next = waveform_samps.at(index_next);

        //interpolate between the two samples
        double amplitude = amplitude_last + (amplitude_next - amplitude_last)*time_interpolate_factor;

        //get the row and column of this block
        // i'm **assuming** that the blocks are indexed by row-major order, i.e. the first 12 samples are for row=0, col=0-11, then row=1, col=0-11, etc.
        int row = std::floor(i/12);
        int col = i%12;

        //scale the amplitude to energy (MeV)
        amplitude = std::min(std::max(amplitude, min_waveform_energy), max_amplitude); //clamp to range

        amplitude = amplitude / (max_amplitude - min_waveform_energy); 

        double block_center_x = (double)col;
        double block_center_y = (double)row;

        auto block = new TBox(
            col-amplitude/2., row-amplitude/2.,
            col+amplitude/2., row+amplitude/2.
        ); 

        block->SetFillColor(kBlack);

        // Look at how we draw this. Ususally, in ROOT, we would draw this block like this: 
        //
        //  block->Draw("SAME")
        // 
        // But, what we want to do is let the kernel do this for us, so it can control when the object is drawn/deleted. 
        kernel.Draw(block, "SAME");

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
void draw_waveform_profile()
{
    auto& kernel = EventDisplayKernel::Instance(); 

    //waveform samples
    const br_array<double>& waveform_samps  = kernel.GetData<br_array<double>>("sbs.hcal.samps");

    if (!kernel.IsActive()) return; //if the app is not active, don't draw anything.
    
    static std::unique_ptr<std::vector<double>> pts_profile, pts_t; 

    if (!pts_profile || pts_profile->empty()) {
        pts_profile = std::make_unique<std::vector<double>>(n_samples, 0.); 
    }
    if (!pts_t || pts_t->empty()) {
        pts_t = std::make_unique<std::vector<double>>(n_samples, 0.); 
    }

    double min{+1e30}, max{-1e30};

    for (int t=0; t<n_samples; t++) {

        auto& sum = (*pts_profile)[t]; 
        sum =0.; 

        (*pts_t)[t] = 50.*(double)t; 

        for (int i=0; i<n_blocks; i++) {

            int t_index = i*n_samples + t;
            
            double amplitude = waveform_samps.at(t_index); 
            
            sum += amplitude; 
        }

        min = std::min( min, sum );
        max = std::max( max, sum );
        std::printf("<draw_waveform_profile>: t: %4.1f, amp: %+6.4f\n", (double)t, sum); 
    }

    auto graph_sum = new TGraph(n_samples, pts_t->data(), pts_profile->data()); 

    graph_sum->SetTitle("Sum of all waveform samples;timestamp;#Sigma waveform amps."); 
    
    auto pad = kernel.GetCanvas(); 
    pad->DrawFrame(0,min - 0.1*(max-min), 50.,max + 0.1*(max-min), "FRAME"); 

    kernel.Draw(graph_sum, "HIST"); 
}
//___________________________________________________________________________________________________________
void draw_waveform_profile_timestamp()
{
    auto& kernel = EventDisplayKernel::Instance(); 

    if (!kernel.IsActive()) return; 

    auto canv = kernel.GetCanvas(); 
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



