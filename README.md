# SoftRays
This is a very simple C implementation of the ray tracing algorithm that uses SSE instructions. It is designed to help novice developers understand the basics of computer graphics.
# Compiling and running on Windows
( Download and install the MinGW compiler )  
( Download and unpack this repository )  
mkdir build  
mingw32-make  
SoftRays  
# Compiling and running on Linux (X.Org)
sudo apt install --no-install-recommends libxcb1-dev libxcb-image0-dev  
git clone https://github.com/dmsmdms/SoftRays  
cd SoftRays  
make linux_xcb  
./SoftRays  
