vcpkg install pfultz2-linq
vcpkg install boost-assign boost-foreach boost-test
mkdir build-x86-windows
cd build-x86-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x86-windows -DCMAKE_GENERATOR_PLATFORM=Win32
cmake --build . --config Release
ctest -C Release
cd ..
pause
