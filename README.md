# ChucKDesigner

[TouchDesigner](https://derivative.ca/) => [ChucK](https://chuck.stanford.edu/) => [TouchDesigner](https://derivative.ca/);

ChucKDesigner is an integration of the ChucK music/audio programming language with the TouchDesigner visual programming language. With ChucKDesigner,
* TouchDesigner can run ChucK code at any sample rate, with any number of input and output channels.
* TouchDesigner can set ChucK global float variables with TouchDesigner UI and [channel](https://docs.derivative.ca/Channel) streams.
* TouchDesigner can receive and post-process ChucK's output.
* TouchDesigner can "listen" to ChucK global float variables.

Please use the [issues](https://github.com/DBraun/ChucKDesigner/issues) and [discussions](https://github.com/DBraun/ChucKDesigner/discussions) pages on GitHub.

## Installation

### ChucK

[Downloading ChucK](https://chuck.stanford.edu/release/) separately is optional but highly encouraged! Use miniAudicle and the examples that come with the installation to learn ChucK. More resources are available from the [ChucK homepage](https://chuck.stanford.edu/).

### Windows

Go to the [Releases](https://github.com/DBraun/ChucKDesigner/releases) and download the latest `.dll` files from the Windows distribution. Place them in the Plugins folder.

### MacOS

**The macOS builds are not yet working.**

Go to the [Releases](https://github.com/DBraun/ChucKDesigner/releases) and download the latest `.plugin` and `.dylib` files from the macOS distribution. Place them in the Plugins folder.

## The Future

The 2021 **experimental** build of TouchDesigner supports a more [powerful interface for Custom Operators](https://docs.derivative.ca/Release_Notes/2021.30000#Custom_Operators). Currently, TouchDesigner can set global floats and listen to them with the `ChucK Listener CHOP`. In the future, it will be possible to do the same for more variable types such as integers, bools, strings, float arrays, and integer arrays. Additionally, there will be a callback DAT associated with the `ChucK Listener CHOP` to make it easy to respond to changes in global variables. If you're curious what this will be like, you can get a feel by  reading about [Chunity](https://chuck.stanford.edu/chunity/) and the [Chunity tutorial](https://chuck.stanford.edu/chunity/tutorials/).