vcpkg install nonius:x64-windows
mkdir build-x64-windows-static
cd build-x64-windows-static
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_GENERATOR_PLATFORM=x64 -DUSE_STATIC_RTL=ON
cmake --build . --config Release
cd ..
pause
