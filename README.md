# Pivot by HyperCube Studios

current CUGL version: 2.3.2

# Windows Libigl Instructions:
Download libigl and eigen
Rename these files “libigl” and “eigen” respectively
Put these in the “Hypercube” folder
Run “python cugl Pivot”
Open the project in “Pivot/build/windows/Platformer.sln”
Build and run as usual


# Mac Libigl Integration Steps:
Download libigl and eigen and unzip them and put the at the root of the hypercube directory. Rename the folders libigl and eigen.
Go inside the libigl folder
Create a folder called build
Run cmake -G Xcode ../ in the build folder 
This should Generate a libigl Xcode project
Go inside the eigen folder
Create a folder called build
Run cmake -G Xcode ../ in the build folder
This should Generate a eigen Xcode project
Open libigl.xcodeproject
Automatically create schemes
Click libigl project on the sidebar
Find ig_core in the targets
Remove zero check from target dependencies build phases
Open up our project Xcode project 
Right click cugl.xcodeproject
Add files
Add the libigl.xcodeproject
For cugl-mac add libigl.a to the build phases link binary with libraries


After the integration steps are run, the project can be rebuilt as usual with python cugl Pivot
The integration steps only have to be done once.
If you have any questions or problems please email me at smf267@cornell.edu
