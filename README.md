# ChucKDesigner

[TouchDesigner](https://derivative.ca/) => [ChucK](https://chuck.stanford.edu/) => [TouchDesigner](https://derivative.ca/);

ChucKDesigner is an integration of the ChucK music/audio programming language with the TouchDesigner visual programming language. With ChucKDesigner,
* TouchDesigner can run ChucK code at any sample rate, with any number of input and output channels.
* TouchDesigner can set ChucK global float variables with TouchDesigner UI and channel streams.
* TouchDesigner can receive and post-process ChucK's output such as audio or melodic information.
* TouchDesigner can "listen" to ChucK global float variables.

Please use the [issues](https://github.com/DBraun/ChucKDesigner/issues) and [discussions](https://github.com/DBraun/ChucKDesigner/discussions) pages on GitHub.

Demo:

[![Demo Video Screenshot](https://img.youtube.com/vi/TmJQh1lWXso/0.jpg)](https://www.youtube.com/watch?v=TmJQh1lWXso "ChucKDesigner - Music Programming in TouchDesigner")

## Installation

### ChucK

[Downloading ChucK](https://chuck.stanford.edu/release/) is optional but highly encouraged! Use miniAudicle and the examples that come with the installation to learn ChucK. More educational resources are available from the [ChucK homepage](https://chuck.stanford.edu/).

### Python

Install Python 3.9. It is necessary because ChucKDesigner uses the new Custom Operator API for communicating between C++ and Python.

### Windows

Download and unzip the latest Windows [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.dll` files to this project's `Plugins` folder or `%USERPROFILE%/Documents/Derivative/Plugins`.

### MacOS

Due to some difficulties with codesigning, for the moment you must compile ChucKDesigner on your own computer.

1. Clone this repository with git. Then update all submodules in the root of the repository with `git submodule update --init --recursive`
2. Install Xcode.
3. [Install CMake](https://cmake.org/download/) and confirm that it's installed by running `cmake --version` in Terminal.
4. Find your Development Profile. Open Keychain Access, go to 'login' on the left, and look for something like `Apple Development: example@example.com (ABCDE12345)`. Then in Terminal, run `export CODESIGN_IDENTITY="Apple Development: example@example.com (ABCDE12345)"` with your own info substituted. If you weren't able to find your profile, you need to create one. Open Xcode, go to "Accounts", add your Apple ID, click "Manage Certificates", and use the plus icon to add a profile. Then check Keychain Access again.
5. In the same Terminal window, navigate to the root of this repository and run `sh build_macos.sh`
6. Open `ChucKDesigner.toe`

<!-- Download and unzip the latest macOS [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.plugin` and `.dylib` files to this project's `Plugins` folder or `~/Library/Application Support/Derivative/TouchDesigner099/Plugins`. -->

## The Future

The 2021 **experimental** build of TouchDesigner supports a more [powerful interface for Custom Operators](https://docs.derivative.ca/Release_Notes/2021.30000#Custom_Operators). Currently, with ChucKDesigner, TouchDesigner can tell ChucK to set global floats and listen to them with the `ChucK Listener CHOP`. In the future, it will be possible to do the same for more variable types such as integers, bools, strings, float arrays, and integer arrays. Additionally, there will be a callback DAT associated with the `ChucK Listener CHOP` to make it easy to respond to changes in global variables. If you're curious what this will be like, you can get a feel by reading about [Chunity](https://chuck.stanford.edu/chunity/) and the [Chunity tutorials](https://chuck.stanford.edu/chunity/tutorials/).