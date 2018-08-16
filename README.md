# vectorNavGetData
Custom program for sampling data from an INS. Originally intended for VectorNav VN-300 onboard a UAV

Replaced in July 2018 with [BC5-datalogger](https://github.com/ukyuav/BC5-datalogger/), a version that combines this program with [mccGetData](https://github.com/ukyuav/mccGetData) and runs on a Raspberry Pi.

# Building

1. Download the [VectorNav Programming Library](https://www.vectornav.com/docs/default-source/downloads/programming-library/vnproglib-1-1-4.zip?sfvrsn=fe678835_20)
     - Open `\cpp\projects\vs2013\libvncxx.sln` in Visual Studio and build
2. Open the `getting_started.sln` file
     - Right-click on the `VectorNavGetData` project in the Solution Explorer view and select "Properties"
     - Under "C/C++", edit the "Additional Include Directories" to include the `\cpp\include` folder from the VectorNav Programming Library that you downloaded
     - Under "Linker", add the path the folder in the programming library that contains the libvcxx.lib library. This is likely `\cpp\projects\vs2013\Debug`
     - Under "Linker", select "Input" from the sidebar and add `libvncxx.lib` to the list of "Additional dependencies"
     - Right-click "Solution 'getting_started' (3 projects)" from the Solution Explorer view and select "Add -> Existing Project", then add the libvncxx project that you built in step 1
3. Right-click on the solution and select "Build"
  
# Usage
See [DAQ/VectorNav Software Manual](https://docs.google.com/document/d/1w9wU2Ji-shShgHVuYUbm-j-41Ev9MD6pKY9pG9r2kaM/edit?usp=sharing)
