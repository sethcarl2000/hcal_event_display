#ifndef draw_basic_window_h
#define draw_basic_window_h

#include <AppDrawFunction.hpp>
#include <UserApp.hpp>
// ROOT headers
// TGUI headers
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>
#include <TGLayout.h>


AppDrawFunction draw_basic_window(UInt_t w, UInt_t h)
{
    AppDrawFunction ret{
        .fcn = [w,h](UserApp* app, TRootEmbeddedCanvas* embed_canvas) 
        {
                //reset the canvas & create a new one
                embed_canvas = nullptr; 
                //
                embed_canvas = new TRootEmbeddedCanvas("embedded_canvas", app, w,h); 

                //add the canvas as a sub-window of our app
                app->AddFrame(embed_canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,10));
        }, 
        .is_active = true
    };

    return ret; 
}

#endif