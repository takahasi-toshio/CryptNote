cd debug
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
nmake
cd ..\release
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
cd ..
