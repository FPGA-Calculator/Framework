# The Framework

## _Building the framework for the Calculator project_
This code accompanies the Calculator project described in this [blog].

## Build Tools
The project uses several tools that need to be installed separately:

* Quartus synthesis tool for Altera FPGA devices ([Install][Quartus] version 13.0 SP1 to support Cyclone II device)
* ModelSim (optional) ([Install][ModelSim] Starter Edition, or SE)
* Verilator ([Install][verilator], suggested to install TOT and recompile it yourself)
* Qt ([Install][Qt] version 5.15.x and support for desktop and WebAssembly targets

Altough you could set everything up in Linux, the development was done in Windows 10 with WSL2 (Windows Subsystem for Linux) running Verilator. The setup is explained in more details [in a blog post here][blog]

## Build
The same Verilog sources are build using different tools, for different purpose.

### FPGA
Load `quartus/*.qpf` project file into Quartus as a project; synthesise and flash the bitstream into a designated board. This project is designed around a [Cyclone II EP2C5 "Mini" dev board][devboard], but you should be able to use any other board or even pick a different FPGA vendor. The source files use generic SystemVerilog and there is nothing specific tying it to this particular board except the pin assignment.

### Verilate
Verilator is used to compile Verilog code into C++. There are two different makefiles depending on what we want to do with the software:

* Compile it, and then run it for a limited number of cycles (until the LCD initializes and prints "Hello World"). Since this a default makefile, there is no need to specify it on the command line. **Note**: this will also run it with trace and coverage turned on, and will produce a large trace file which you can view using gtkwave:
```sh
make
```
* Only compile Verilog code into C++ sources and do nothing. You need to do this step before trying to compile the project in Qt since it generates required verilated files:
```sh
make -f MakefileQt
```

### Qt
Qt compiles verilated sources along with its supporting project files into a software application. The intent is to designate two primary target platforms (which you have to manually do when you load the project in QtCreator):

1. Desktop application
2. WebAssembly target

Both targets allow you to run the Verilog code wrapped as a software app. You can see what's displayed on the LCD screen, observe the state of 3 LEDs and also interact with a push-button.

The WebAssembly project can be opened on the local PC (QtCreator will do that for you when you run the app), or it can be copied to a web server and then loaded by a web browser.

You can start a simple local web server by running this python command in the folder that contains your final WebAssembly files:
```sh
python3 -m http.server
```

[Quartus]: <https://fpgasoftware.intel.com/13.0sp1/>
[ModelSim]: <https://www.intel.com/content/www/us/en/software/programmable/quartus-prime/model-sim.html>
[verilator]: <https://www.veripool.org/projects/verilator/wiki/Installing>
[qt]: <https://www.qt.io/download>
[blog]: <https://baltazarstudios.com/calculator4>
[devboard]: <http://land-boards.com/blwiki/index.php?title=Cyclone_II_EP2C5_Mini_Dev_Board>
