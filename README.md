# ChucKDesigner

[ChucK](https://chuck.stanford.edu/) + [TouchDesigner](https://derivative.ca/)

ChucKDesigner is an integration of the ChucK music/audio programming language with the TouchDesigner visual programming language. With ChucKDesigner, TouchDesigner can control ChucK code via user interfaces and [channel operator](https://docs.derivative.ca/Channel) streams. TouchDesigner can also fetch audio streams and float variables from ChucK. Please use the [issues](https://github.com/DBraun/ChucKDesigner/issues) and [discussions](https://github.com/DBraun/ChucKDesigner/discussions) pages on Github.

## Installation

### ChucK

[Downloading ChucK](https://chuck.stanford.edu/release/) separately is optional but highly encouraged! Use miniAudicle and the examples that come with the installation to learn ChucK.

### Windows

Go to the [Releases](https://github.com/DBraun/ChucKDesigner/releases) and download the latest `.dll` files. Place them in the Plugins folder.

### MacOS

**The macOS builds are not yet working.**

Go to the [Releases](https://github.com/DBraun/ChucKDesigner/releases) and download the latest `.plugin` and `.dylib` files. Place them in the Plugins folder.

## The Future

The 2021 **experimental** build of TouchDesigner supports a more [powerful interface for Custom Operators](https://docs.derivative.ca/Release_Notes/2021.30000#Custom_Operators). In the future, ChucKDesigner will use this feature to share more variable types such as bools, integers, and more special cases of arrays bidirectionally between TouchDesigner and ChucK. It will also be possible to have Python callbacks inside TouchDesigner when global ChucK variables change.

## Background

ChucKDesigner is similar in spirit to [Chunity](https://chuck.stanford.edu/chunity/), the combination of ChucK and the Unity game engine.