# Interactive Event Display 

I probably need to change the name from 'hcal_event_display', considering that the main point of the project is its general applicability to other experiments / detectors. 

I will try to keep the 'main' branch clean & stable, but feel free to checkout the other 'feature' branches, where I will be experimenting with additional features (although most are merged with 'main' now). 


## Compile Instructions

 > [!NOTE]
 > I develop using the `bash` terminal, so I am going to write these instructions assuming your are using `bash`, as well. Bear in mind, however, that Jefferson lab's `ifarm` ssh terminal does **not** use bash by default (it uses `tsch`). So, if you want to follow these instructions verbatim to compile this project over an `ifarm` ssh connection, just enter the command `bash` in the terminal window to open a bash terminal.

To compile, you just need a working version of ROOT 6. I have been building the project using ROOT 6.20, but I don't anticipate that a newer version of ROOT 6.xx would break functionality (don't quote me on that, though). 
You also will need to use `cmake` (minimum version: 3.16) 

I develop in a [conda](https://github.com/conda/conda) environment, but any other configuration of ROOT which cmake's package handler can find _should_ work OK. 
1. First, compile the project. To do this, navigate to the root directory of this repo, and execute:
   ```bash
   cmake -B build -S .
   ```
   
    > If you want to try to modify the compiled code yourself, it may be useful to enable verbose debug printing. to do this, replace the above command with `cmake -DDEBUG=ON -B build -S .`
   
   This will create a new directory called `build` where the shared library will go.

2. Then call:
   ```bash
   cmake --build build -j 4 
   ```
   This will compile the project. The `-j 4` option specifies that 4 CPU threads will be used to compile the project concurrently; omit this option if you only want to use 1 thread at a time.
   
3. Finally, you must set the environment. Just call:
   ```bash
   source setenv.sh
   ```
   This script appends the path to this project's `build` directory to the user's `LD_LIBRARY_PATH` (so ROOT can find the project's shared library), and appends the `include` directory to the user's `ROOT_INCLUDE_PATH` (so ROOT can find the project's headers).

4. Now, any macro using the Event Display can be invoked as an ordinary ROOT macro:

   ```bash
   root -l my_macro.C
   ```
   Or, just as well with the Analyzer:
   ```bash
   analyzer -l my_macro.C
   ``` 
