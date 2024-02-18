if "%PYTHONVER%"=="" (
    set PYTHONVER=3.11
)
echo "Using Python version: %PYTHONVER%"

cmake . -DCMAKE_BUILD_TYPE=Release -Bbuild -DPYTHONVER=%PYTHONVER%
cmake --build build --config Release
echo "All Done!"
