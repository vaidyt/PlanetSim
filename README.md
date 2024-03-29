# PlanetSim
Simulation of planetry Motion around the Sun


## Build instructions and dependencies
- Open "PlanetSimulation.sln" in VS 2019 or later (earlier versions should also work as long as it supports C++11 or higher) and simply build and run (as you would do any other program). 
- You need to have desktop development for Windows in C++ installed as part of VS (if not, you can easily do this by running the VS installer). See [here](https://docs.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-160) for details.
- Alternatively, to just run the .exe, go to the release/debug folder and double click on "PlanetSimulation.exe".

## Usage instructions
- The input JSON file name that defines the planetary bodies is hardcoded in the application. So, to change the input, change the JSON file name (in the code) and put a copy of it in the same folder as the solution.
- "Space bar" pauses/continues (i.e. toggle) the application.
- The letter "r" resets the application to its start point.

## Assumptions and Limitations
- This application is written using Win32 SDK and so will only run in Windows (and not in Mac or Linux).
- Some amount of interaction with bodies other than the sun is considered (although it is crude).
- No double buffering implemented and hence you will see animation flickering.
- Very minimal error checking is done and so GIGO!
- Assumes the first body in the input JSON is always the Sun.
- Simulation speed is 100 weeks / sec. 
