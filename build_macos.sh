# Remove any old plugins and dylib
rm Plugins/libChucKDesignerShared.dylib
rm -r Plugins/ChucKDesignerCHOP.plugin
rm -r Plugins/ChucKListenerCHOP.plugin

# Setup steps, related to compiling ChucK
cd thirdparty/chuck/src/core
bison -dv -b chuck chuck.y
flex -ochuck.yy.c chuck.lex

# Steps for making the Xcode project and compiling with it
cd ../../../..
mkdir build
cd build
cmake .. -G "Xcode" -DCMAKE_OSX_ARCHITECTURES=x86_64
xcodebuild -configuration Release -project ChucKDesignerCHOP.xcodeproj

# Steps so that libChucKDesignerShared.dylib is found as a dependency
install_name_tool -change @rpath/libChucKDesignerShared.dylib @loader_path/../../../libChucKDesignerShared.dylib  Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
install_name_tool -change @rpath/libChucKDesignerShared.dylib @loader_path/../../../libChucKDesignerShared.dylib  Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

# codesigning
# Open Keychain Access. Go to "login". Look for "Apple Development".
# run `export CODESIGN_IDENTITY="Apple Development: example@example.com (ABCDE12345)"` with your own info substituted.
codesign --force --deep --sign "$CODESIGN_IDENTITY" Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
codesign --force --deep --sign "$CODESIGN_IDENTITY" Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

# Confirm the codesigning
codesign -vvvv Release/ChucKDesignerCHOP.plugin/Contents/MacOS/ChucKDesignerCHOP
codesign -vvvv Release/ChucKListenerCHOP.plugin/Contents/MacOS/ChucKListenerCHOP

# Copy to Plugins directory
mv Release/libChucKDesignerShared.dylib ../Plugins
mv Release/ChucKDesignerCHOP.plugin  ../Plugins
mv Release/ChucKListenerCHOP.plugin  ../Plugins

echo "All Done!"