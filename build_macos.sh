if [ "$TOUCHDESIGNER_APP" == "" ]; then
    # a reasonable default in case you forget to set the path to TouchDesigner.
    export TOUCHDESIGNER_APP=/Applications/TouchDesigner.app
fi

# if building on Apple Silicon
if [[ $(uname -m) == 'arm64' ]]; then
    echo "Building universal for Apple Silicon."
    export CMAKE_OSX_ARCHITECTURES="arm64"
else
    echo "Building for x86_64."
    export CMAKE_OSX_ARCHITECTURES="x86_64"
fi

# export CMAKE_OSX_ARCHITECTURES="x86_64"

echo Assuming TouchDesigner is located at $TOUCHDESIGNER_APP

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
cmake -Bbuild -G "Xcode" -DCMAKE_OSX_ARCHITECTURES=$CMAKE_OSX_ARCHITECTURES
cmake --build build --config Release

# Steps so that libChucKDesignerShared.dylib is found as a dependency
install_name_tool -change @rpath/libChucKDesignerShared.dylib @loader_path/../../../libChucKDesignerShared.dylib  build/Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
install_name_tool -change @rpath/libChucKDesignerShared.dylib @loader_path/../../../libChucKDesignerShared.dylib  build/Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

# This is pretty hacky because we didn't originally link to the TouchDesigner python library when compiling.
install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.9/Python $TOUCHDESIGNER_APP/Contents/Frameworks/Python.framework/Versions/3.9/lib/libpython3.9.dylib build/Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.9/Python $TOUCHDESIGNER_APP/Contents/Frameworks/Python.framework/Versions/3.9/lib/libpython3.9.dylib build/Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

if [ -n "$CODESIGN_IDENTITY" ]; then
    echo "Doing codesigning."
    # codesigning
    # Open Keychain Access. Go to "login". Look for "Apple Development".
    # run `export CODESIGN_IDENTITY="Apple Development: example@example.com (ABCDE12345)"` with your own info substituted.
    codesign --force --deep --sign "$CODESIGN_IDENTITY" build/Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
    codesign --force --deep --sign "$CODESIGN_IDENTITY" build/Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

    # Confirm the codesigning
    codesign -vvvv build/Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
    codesign -vvvv build/Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP
else
    echo "Skipping codesigning."
fi

# Copy to Plugins directory
cp build/Release/libChucKDesignerShared.dylib Plugins
cp -R build/Release/ChucKDesignerCHOP.plugin Plugins
cp -R build/Release/ChucKListenerCHOP.plugin Plugins

echo "All Done!"