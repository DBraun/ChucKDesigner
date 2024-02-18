# ChucKDesigner

`TouchDesigner => ChucK => TouchDesigner;`

ChucKDesigner is an integration of the [ChucK](https://chuck.stanford.edu/) music/audio programming language with the [TouchDesigner](https://derivative.ca) visual programming language. With ChucKDesigner,
* TouchDesigner can run ChucK code at any sample rate, with any number of input and output channels.
* TouchDesigner can receive and post-process ChucK's output such as audio or melodic information.
* TouchDesigner can set ChucK global variables with TouchDesigner UI, CHOP streams, and Python scripting.
* TouchDesigner can use Python callbacks to respond to changes in ChucK global variables.

ChucKDesigner consists of two custom operators, the ChucK Audio CHOP and the ChucK Listener CHOP. The ChucK Audio CHOP "runs" the ChucK code. TouchDesigner can also tell it how to update global variables. The Listener CHOP "listens" to a ChucK Audio CHOP. It can continuously stream out global floats and integers as ordinary CHOP data. Using a Python-based callback DAT, it can also access many more global variable types such as arrays, strings, and events.

Please use the [issues](https://github.com/DBraun/ChucKDesigner/issues) and [discussions](https://github.com/DBraun/ChucKDesigner/discussions) pages on GitHub. You can also participate in the #ChucKDesigner channel in the [ChucK](https://chuck.stanford.edu/) Discord server.

## Tutorials

Part 1 (Introduction):

[![Demo Video Screenshot](https://img.youtube.com/vi/TmJQh1lWXso/0.jpg)](https://www.youtube.com/watch?v=TmJQh1lWXso "ChucKDesigner - Music Programming in TouchDesigner")

Part 2 (Python API):

[![Make a Drum Sequencer with ChucK in TouchDesigner](https://img.youtube.com/vi/U34PyL_zMJ0/0.jpg)](https://www.youtube.com/watch?v=U34PyL_zMJ0 "Make a Drum Sequencer with ChucK in TouchDesigner")

## Installation

### ChucK

ChucKDesigner is currently being built for ChucK 1.5.2 for TouchDesigner with 3.11 (2023 builds) and Python 3.9 (2022 builds). If you need to support something else, please check out earlier [releases](https://github.com/DBraun/ChucKDesigner/releases) or commits of this repository.

[Downloading ChucK](https://chuck.stanford.edu/release/) is optional but highly encouraged! Use miniAudicle and the examples that come with the installation to learn ChucK. More educational resources are available from the [ChucK homepage](https://chuck.stanford.edu/). ChucKDesigner is very similar to [Chunity](https://chuck.stanford.edu/chunity/) (the integration of ChucK with Unity), so you are also encouraged to learn about [Chunity](https://github.com/ccrma/chunity/)!

### Windows

Download and unzip the latest Windows [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.dll` files to this project's `Plugins` folder or `%USERPROFILE%/Documents/Derivative/Plugins`. That's all!

<details>
<summary>Building on Windows (Optional)</summary>
<br>
Clone this repository with git. Then update all submodules in the root of the repository with <code>git submodule update --init --recursive</code>.
<br>
Install <a href="https://www.python.org/downloads/release/python-3117/">Python 3.11</a> to <code>C:/Python311/</code> and confirm it's in your system PATH.
<br>
Install CMake and confirm that it's installed by running <code>cmake --version</code> in a command prompt.
<br>
Then in this repository,
<br>
<code>
cmake . -DCMAKE_BUILD_TYPE=Release -Bbuild -DPYTHONVER="3.11"
</code>
<br>
Finally, open <code>build/ChucKDesignerCHOP.sln</code> and compile.
</details>

### MacOS

[@DBraun](https://github.com/DBraun) doesn't have a macOS App Distribution Developer License, so you must compile ChucKDesigner on your own computer.

1. Clone this repository with git. Then update all submodules in the root of the repository with `git submodule update --init --recursive`
2. Install Xcode.
3. [Install CMake](https://cmake.org/download/) and confirm that it's installed by running `cmake --version` in Terminal. You may need to run `export PATH="/Applications/CMake.app/Contents/bin":"$PATH"`
4. In a Terminal Window, export a variable to the TouchDesigner.app to which you'd like to support. For example: `export TOUCHDESIGNER_APP=/Applications/TouchDesigner.app`, assuming this version is a 2023 build or higher.
5. Optional: depending on the Python version associated with the TouchDesigner you intend to use, run `export PYTHONVER=3.11` or `export PYTHONVER=3.9`.
6. In the same Terminal window, navigate to the root of this repository and run `sh build_macos.sh`
7. Open `ChucKDesigner.toe` and play around!

<!-- Download and unzip the latest macOS [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.plugin` and `.dylib` files to this project's `Plugins` folder or `~/Library/Application Support/Derivative/TouchDesigner099/Plugins`. -->

## API

### Python interface to ChucK Audio CHOP

The ChucK Audio CHOP's functions:

* `.set_float(name: str, val: float)`
* `.set_int(name: str, val: int)`
* `.set_string(name: str, val: int)`
* `.set_float_array(name: str, vals: List[float])` **not numpy arrays!**
* `.set_int_array(name: str, vals: List[int])` **not numpy arrays!**
* `.set_float_array_value(name: str, index: int, val: float)`
* `.set_int_array_value(name: str, index: int, val: int)`
* `.set_associative_float_array_value(name: str, key: str, val: float)`
* `.set_associative_int_array_value(name: str, key: str, val: int)`
* `.broadcast_event(name: str)`
* `.set_log_level(level: int)` **0 is None and 10 is "Crazy"**

Suppose the ChucK Audio CHOP has compiled this code:

```chuck
440. => global float freq;

SinOsc s => dac;

while(true) {
    freq => s.freq;
    10::ms => now;
}
```

This can be seen in the following image:

![Float Example TouchDesigner Screenshot](docs/float_example.png?raw=true "Float Example TouchDesigner Screenshot")

In TouchDesigner, we can execute the Python code
`op('chuckaudio1').set_float("freq", 880.)`
This will set the global float variable named `freq` to 880. The code has been written to update the sine oscillator's frequency every 10 milliseconds, so you will immediately hear a change in the frequency. Note that the code below would **not** have led to a change in sound.

```chuck
SinOsc s => dac;

440. => global float freq => s.freq;

while(true) {
    10::ms => now;
}
```

The reason is that global variables are not [Unit Generators](https://chuck.stanford.edu/doc/program/ugen.html). Although `freq` has been chucked to `s.freq`, we still need code in the `while(true)` loop to update the oscillator's frequency.

### Streaming global floats and integers in ChucK Listener CHOP

The ChucK Listener has a custom parameter for the ChucK Audio CHOP to which it should listen. Suppose we had used a ChucK Audio CHOP to compile this:

```chuck
440. => global float freq;
0 => global int randInt;

SinOsc s => dac;

while(true) {
    freq => s.freq;
    Std.rand2(0, 10) => randInt;
    10::ms => now;
}
```

On the ChucK Audio Listener, there is a custom parameter for "Float Variables". In this field you can type any number of global variables, each separated by a single space. In this example, there's just one global float, so you can type `freq`. Similarly, in the "Int Variables" custom parameter, you can type `randInt`. The ChucK Listener will then output as ordinary CHOP information a single-sample with one channel named `freq` and another channel named `randInt`. The ordinary CHOP output of the Listener CHOP is only for global floats and global integers. However, all variable types can be received as Python callbacks, as explained in the next section.

### Python Callbacks in ChucK Listener CHOP

In the example above, we used a `global float freq` and a `global int randInt`. Find the `Float Variables` and `Int Variables` custom parameters on the ChucK Listener CHOP and set them to `freq` and `randInt` respectively. Now `freq` will appear in the `getFloat` callback, and `randInt` will appear in the `getInt` callback. 

```python
# This is an example callbacks DAT for a ChucK Listener operator.
# In all callback methods, "listener" is the ChucK Listener operator.

def getFloat(listener, name, val):
    print(f'getFloat(name="{name}", val={val})')

def getInt(listener, name, val):
    print(f'getInt(name="{name}", val={val})')

def getString(listener, name, val):
    print(f'getString(name="{name}", val={val})')

def getEvent(listener, name):
    print(f'getEvent(name="{name}")')

def getFloatArray(listener, name, vals):
    print(f'getFloatArray(name="{name}", vals={vals})')

def getIntArray(listener, name, vals):
    print(f'getIntArray(name="{name}", vals={vals})')
```

Most of these callbacks are straightforward to understand, but the `Event` type syntax is worth discussing. Let this be the compiled ChucK code:

```chuck
global Event pulse;
global Event notifier;

fun void playImpact() {
    SndBuf buf => dac;
    "special:dope" => buf.read;

    // chuck enough time so that the buf plays
    1::second => now; 
    
    // invoke getEvent("notifier") in TouchDesigner
    notifier.broadcast();
}

while( true ) {
    pulse => now;
    spork ~ playImpact();
}
```

At the beginning, there will be 2 channels of silent output by default. The line `pulse => now;` consumes time until we broadcast an event in TouchDesigner with Python:
```python
op('chuckaudio1').broadcast_event('pulse')
```

The ChucK code will then spork a shred of `playImpact()`, which will play a short sound. After 1 second of playing the sound, ChucK will broadcast an event named `notifier` back to TouchDesigner. This event will show up in the `getEvent()` callback method, if `notifier` is in the ChucK Listener's custom parameter "Event Variables".


### Chugins

ChucKDesigner supports [Chugins](https://github.com/ccrma/chugins/), which are custom pre-compiled ChucK "plugins". The chugin should be located in a subfolder named "Chugins" of the "working directory" custom parameter on the ChucK Audio CHOP. For example, if the working directory parameter is `assets`, then the chugins should be in `assets/Chugins`.
