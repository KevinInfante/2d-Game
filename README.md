# 2d-Game
Creating a 2d game with C++

For Linux and Mac, the "bash" command can be used to run .sh files, ignore
the following step.
On Windows, make sure you have a sh.exe in your path environment variables.
If you have Git installed, there should be a sh.exe in Git/bin/. Copy the 
path to this file.
Use the system search to look up "Environment Variables", go to system
variables, select Path, then click edit. Add the path to your sh.exe.
This sh.exe is used to execute the shell scripts in buid.sh

using LLVM for the clang compiler
tested on the installation: LLVM-18.1.8-win64.exe
When installing LLVM, select "Add LLVM to the system PATH for all users".
Clang also requires another compiler installed on your system, such as MSVC
or MinGW. If you have neither, installing visual studio should be sufficient.

To build/compile the code, in VS Code you can press CTRL+SHIFT+B (on Windows), or
in the terminal you can type "bash build.sh" or "sh build.sh" once navigated to 
the project directory.

To run the code, press F5 or FN+F5. Alternatively, in VS Code, press "Run and Debug"
then Start Debugging.