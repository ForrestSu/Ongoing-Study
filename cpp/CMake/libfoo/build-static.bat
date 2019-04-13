mkdir build-static
cd build-static
cmake .. -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON
cmake --build . --config Release
ctest . -C Release
cd ..
pause
