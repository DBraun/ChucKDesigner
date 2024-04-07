if [ "$TOUCHDESIGNER_APP" == "" ]; then
    # a reasonable default in case you forget to set the path to TouchDesigner.
    export TOUCHDESIGNER_APP=/Applications/TouchDesigner.app
fi
echo Assuming TouchDesigner is located at $TOUCHDESIGNER_APP

# if building on Apple Silicon
if [[ $(uname -m) == 'arm64' ]]; then
    echo "Building universal for Apple Silicon."
    export CMAKE_OSX_ARCHITECTURES="arm64"
else
    echo "Building for x86_64."
    export CMAKE_OSX_ARCHITECTURES="x86_64"
fi

if [ "$PYTHONVER" == "" ]; then
    # Guess which Python version TD uses.
    export PYTHONVER="3.11"
fi
echo Building for Python $PYTHONVER

# Remove any old plugins and dylib
rm Plugins/libChucKDesignerShared.dylib
rm -r Plugins/ChucKDesignerCHOP.plugin
rm -r Plugins/ChucKListenerCHOP.plugin

# Setup steps, related to compiling ChucK
cd thirdparty/chuck/src/core
bison -dv -b chuck chuck.y
flex -ochuck.yy.c chuck.lex
cd ../../../..

# Steps for making the Xcode project and compiling with it
cmake -Bbuild -G "Xcode" -DCMAKE_OSX_ARCHITECTURES=$CMAKE_OSX_ARCHITECTURES -DPYTHONVER=$PYTHONVER -DPython_ROOT_DIR=$TOUCHDESIGNER_APP/Contents/Frameworks/Python.framework/Versions/$PYTHONVER
cmake --build build --config Release

# Copy to Plugins directory
cp build/Release/libChucKDesignerShared.dylib Plugins
cp -R build/Release/ChucKDesignerCHOP.plugin Plugins
cp -R build/Release/ChucKListenerCHOP.plugin Plugins

echo "All Done!"