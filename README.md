# Iowa State University Robotics Autonomous Snowplow Software Repository
This repository contains code for the 2019 Autonomous Snowplow competition.
# Version Goals
v0.9 - Path planning using A*.
v1.0 - Stop sign detection as well capability to read and navigate a list of points.
# Main Execution Steps
1. Setup all interfaces with correct references except nav(control setting speed and operation)
2. Start stopsign thread, orientation thread, and location thread
3. Wait until an orientation and location has been read.
4. Initialize nav. In manual mode this means ask for the first location to go to and set the goal orientation. This doesn't modify the drive packet at all. All of that happens in the update function. 
5. Start while loop that should run forever
6. Look for stop sign, stop if it sees one.
7. Perform lidar scan.
8. Translate scan to usable data points.
9. Update map of lidar hits. 
10. Update nav. This means that it will check if its at it's goal point. If not check orientation. If orientation is ok then check speed. Update the drive packet accordingly. 
11. If the drive packet is updated, send it to the motors.
12. Print useful information. Then repeat the while loop.


# Installation of OpenCV in VS2017
1. Click Tools > NuGet Package Manager > Manage NuGet...
2. Search for opencv.win.native
3. Click install.
4. Right click the solution in Solution Explorer and open Properties.
5. Under Linker>Input>Additional Dependencies, add:

opencv_core310.lib

opencv_imgproc310.lib

opencv_videoio310.lib

opencv_highgui310.lib

opencv_ml310.lib

opencv_video310.lib

opencv_features2d310.lib

opencv_calib3d310.lib

opencv_objdetect310.lib

opencv_flann310.lib

6. Under VC++ Directories > Library Directories in the Properties page, add:
C:\ISU Robotics\AutonomousSnowplow2019\packages\opencv.win.native.310.3.0\build\native\lib\x64\v140\Release
7. Add this to your system path by searching "environment variables", opening "environment variables", finding "PATH" under system variables, clicking edit, and adding the path listed below.
C:\ISU Robotics\AutonomousSnowplow2019\packages\opencv.win.native.redist.310.3.0\build\native\bin\x64\v140\Release

# Installation of dlib:
1. Download dlib from here: http://dlib.net
2. Extract it to "C:/ISU Robotics/"
3. In visual studio, add "dlib/all/source.cpp" to your project by right clicking "Sources" in the Solution Explorer and "add existing".
4. Open solution properties.  Add "C:/ISU Robotics/dlib-version" to your Additional Include Directories, located under C++>General.

# First time compilation:
You may get an error compiling the first time.  Click one of the errors.  It may take you to a file with the line CUSTOM=-1.  Comment this line out and recompile.
Be sure to set mode from "Debug" to "Release", and set the platform to x64.
