# Interactive Event Display 

I probably need to change the name from 'hcal_event_display', considering that the main point of the project is its general applicability to other experiments / detectors. 

I will try to keep the 'main' branch clean & stable, but feel free to checkout the other 'feature' branches, where I will be experimenting with additional features (although most are merged with 'main' now). 


## Compile / Download Instructions

 > [!NOTE]
 > I develop using the `bash` terminal, so I am going to write these instructions assuming your are using `bash`, as well. Bear in mind, however, that Jefferson lab's `ifarm` ssh terminal does **not** use bash by default (it uses `tsch`). So, if you want to follow these instructions verbatim to compile this project over an `ifarm` ssh connection, just enter the command `bash` in the terminal window to open a bash terminal.


### Quick start: 
If you want to get started right away, the following series of commands (one command per line) will download a copy of all necessary code, compile it, and let ROOT know where to find the code it needs to use it in your ROOT macros: 
```bash
cd /directory/to/place/download
git clone https://github.com/sethcarl2000/hcal_event_display.git
cd hcal_event_display
cmake -B build -S .
cmake --build build -j
source build/set_edisp_env.sh
```
### Telling ROOT where to find the EDK source code
The EventDisplayKernel (EDK) is designed to be used within cling-compiled ROOT macros (i.e., `vis_data.C`, executed on the command line via `root -l vis_data.C`). 
To use it in this way, you must tell root where the _headers_ and the _shared library_ for the EDK are located.
Both of these are accomplished by running the following command once each time you launch a new terminal: 
```bash
source build/set_edisp_env.sh
```
If you want to have the EDK library loaded each time you launch a new bash terminal, open your bash login script under `~/.bashrc`, and add the following line: 
```bash
source /directory/to/place/download/hcal_event_display/build/set_edisp_env.sh 
```
Where, as before, `/directory/to/place/download` is the path to the directory where you have downloaded and compiled this repo. This way, each time you log in, ROOT is automatically told where to find the EDK library. 
### Using the EDK in your own macros
Once you have `source`'d the `set_edisp_env.sh` script, you just need to add the EDK header to your macro: 

```c++
// in your macro, 'vis_data.C'

#include <EventDisplayKernel.hpp>
// ... other ROOT headers
// ...
```
