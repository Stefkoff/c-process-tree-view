# c-process-tree-view
Simple C executable to print the Linux processes, read from /proc

The program will read from `/proc{PID}/stat` and will display some basic information.

Then all children processes will be listed below. The processes will be sorted first that does not have any child processes.

# Building

Build with CMake:
````
cd c-process-tree-view
mkdir build
cd build
cmake ..
make
````

# Running

The executable will be in ROOT/bin{ptree}

You can run it by `./bin/ptree -p 1` `where -p 1 is the root process to start with`. See `./bin/ptree -h` for more information
