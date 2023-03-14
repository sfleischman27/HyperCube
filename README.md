# Pivot by HyperCube Studios

current CUGL version: 2.3.2


Mac Libigl Integration Steps:

Download libigl and eigen and unzip them and put the at the root of the hypercube directory. Rename the folders libigl and eigen.
Go inside the libigl folder
Create a folder called build
Run cmake -G Xcode ../  
This should Generate a libigl Xcode project
Go inside the eigen folder
Create a folder called build
Run cmake -G Xcode ../  
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
For both cugl-mac and cugl-ios
Add libigl.a to the build phases link binary with libraries
