rem Make sure Python 3.11 is installed and in your system PATH

cmake . -DCMAKE_BUILD_TYPE=Release -Bbuild
cmake --build build --config Release
echo "All Done!"
