# ChucKDesigner

[TouchDesigner](https://derivative.ca/) => [ChucK](https://chuck.stanford.edu/) => [TouchDesigner](https://derivative.ca/);

ChucKDesigner is an integration of the ChucK music/audio programming language with the TouchDesigner visual programming language. With ChucKDesigner,
* TouchDesigner can run ChucK code at any sample rate, with any number of input and output channels.
* TouchDesigner can receive and post-process ChucK's output such as audio or melodic information.
* TouchDesigner can set ChucK global variables with TouchDesigner UI, CHOP streams, and Python scripting.
* TouchDesigner can use Python callbacks to respond to changes in ChucK global variables.

Please use the [issues](https://github.com/DBraun/ChucKDesigner/issues) and [discussions](https://github.com/DBraun/ChucKDesigner/discussions) pages on GitHub.

Demo:

[![Demo Video Screenshot](https://img.youtube.com/vi/TmJQh1lWXso/0.jpg)](https://www.youtube.com/watch?v=TmJQh1lWXso "ChucKDesigner - Music Programming in TouchDesigner")

## Installation

### ChucK

[Downloading ChucK](https://chuck.stanford.edu/release/) is optional but highly encouraged! Use miniAudicle and the examples that come with the installation to learn ChucK. More educational resources are available from the [ChucK homepage](https://chuck.stanford.edu/). ChucKDesigner is very similar to [Chunity](https://chuck.stanford.edu/chunity/) (the integration of ChucK with Unity), so you are also encouraged to learn about Chunity!

### Windows

Download and unzip the latest Windows [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.dll` files to this project's `Plugins` folder or `%USERPROFILE%/Documents/Derivative/Plugins`.

<details>
<summary>Building on Windows</summary>
<br>
Clone this repository with git. Then update all submodules in the root of the repository with `git submodule update --init --recursive`.
<br>
Install Python 3.9 and confirm it's in your system PATH.
<br>
Install CMake and confirm that it's installed by running <code>cmake --version</code> in a command prompt.
<br>
Then in this repository,
<br>
<code>
cmake . -DCMAKE_BUILD_TYPE=Release -Bbuild
</code>
<br>
Then open <code>build/ChucKDesignerCHOP.sln</code> and compile.
</details>

### MacOS

Due to some difficulties with codesigning, for the moment you must compile ChucKDesigner on your own computer.

1. Clone this repository with git. Then update all submodules in the root of the repository with `git submodule update --init --recursive`
2. Install Python 3.9 and confirm it's in your system PATH.
3. Install Xcode.
4. [Install CMake](https://cmake.org/download/) and confirm that it's installed by running `cmake --version` in Terminal.
5. Find your Development Profile. Open Keychain Access, go to 'login' on the left, and look for something like `Apple Development: example@example.com (ABCDE12345)`. Then in Terminal, run `export CODESIGN_IDENTITY="Apple Development: example@example.com (ABCDE12345)"` with your own info substituted. If you weren't able to find your profile, you need to create one. Open Xcode, go to "Accounts", add your Apple ID, click "Manage Certificates", and use the plus icon to add a profile. Then check Keychain Access again.
6. Similarly export a variable to the TouchDesigner.app to which you'd like to support. For example: `export TOUCHDESIGNER_APP=/Applications/TouchDesigner-2021.app`, assuming this version is a 2021.30000 build or higher.
7. In the same Terminal window, navigate to the root of this repository and run `sh build_macos.sh`
8. Open `ChucKDesigner.toe`

<!-- Download and unzip the latest macOS [release](https://github.com/DBraun/ChucKDesigner/releases). Copy the latest `.plugin` and `.dylib` files to this project's `Plugins` folder or `~/Library/Application Support/Derivative/TouchDesigner099/Plugins`. -->

## API

The 2021.30000+ builds of TouchDesigner support a more [powerful interface for Custom Operators](https://docs.derivative.ca/Release_Notes/2021.30000#Custom_Operators). This interface allows Python code in TouchDesigner to invoke methods inside a custom operator. The ChucK Audio CHOP is where ChucK code is run, and therefore it's the operator which can be told what to do via Python methods. In the other direction, the ChucK Listener CHOP can use a callback DAT to respond to changes in a ChucK Audio CHOP's global variables. Let's discuss some examples for each.

### Python interface to ChucK Audio CHOP

Methods:

* `.set_global_float(name: str, val: float)`
* `.set_global_int(name: str, val: int)`
* `.set_global_string(name: str, val: int)`
* `.set_global_float_array(name: str, vals: List[float])` **not numpy arrays!**
* `.set_global_int_array(name: str, vals: List[int])` **not numpy arrays!**
* `.set_global_float_array_value(name: str, index: int, val: float)`
* `.set_global_int_array_value(name: str, index: int, val: int)`
* `.set_global_associative_float_array_value(name: str, key: str, val: float)`
* `.set_global_associative_int_array_value(name: str, key: str, val: int)`
* `.broadcast_event(name: str)`
* `.set_log_level(level: int)`

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

In TouchDesigner, we can execute the python code
`op('chuckaudio1').set_global_float("freq", 880.)`
This will set the global float variable named "freq" to 880. The code has been written to update the sine oscillator's frequency every 10 milliseconds, so you will immediately hear a change in the frequency. Note that the code below would **not** have led to a change in sound.

```chuck
SinOsc s => dac;

440. => global float freq => s.freq;

while(true) {
	10::ms => now;
}
```

The reason is that global variables are not `UGen`. Although `freq` has been chucked to `s.freq`, we still need code in the `while(true)` loop to update the oscillator's frequency.

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

On the ChucK Audio Listener, there is a custom parameter for "Float Variables". In this field you can type any number of global variables, each separated by a single space. In this example, there's just one global float, so you can type "freq". Similarly, in the "Int Variables" custom parameter, you can type "randInt". The ChucK Listener will then output as ordinary CHOP information a single-sample with one channel named "freq" and another channel named "randInt". The ordinary CHOP output of the Listener CHOP is only for global floats and global integers. However, all variable types can be received as Python callbacks, as explained in the next section.

### Python Callbacks in ChucK Listener CHOP

In the example above, we used a `global float freq` and a `global int randInt`. Find the `Float Variables` and `Int Variables` custom parameters on the ChucK Listener CHOP and set them to `freq` and `randInt` respectively. Now `freq` will appear in the `getGlobalFloat` callback, and `randInt` will appear in the `getGlobalInt` callback. 

```python
# This is an example callbacks DAT for a ChucK Audio Operator.
# In all callback methods, "listener" is the ChucK Listener operator doing the callback.

def getGlobalFloat(listener, name, val):
    print(f'getGlobalFloat(name="{name}", val={val})')

def getGlobalInt(listener, name, val):
    print(f'getGlobalInt(name="{name}", val={val})')

def getGlobalString(listener, name, val):
    print(f'getGlobalString(name="{name}", val={val})')

def getGlobalEvent(listener, name):
    print(f'getGlobalEvent(name="{name}")')

def getGlobalFloatArray(listener, name, vals):
    print(f'getGlobalFloatArray(name="{name}", vals={vals})')

def getGlobalIntArray(listener, name, vals):
    print(f'getGlobalIntArray(name="{name}", vals={vals})')

def getGlobalEvent(listener, name):
    print(f'getGlobalEvent(name="{name}")')
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
	
	// invoke getGlobalEvent("notifier") in TouchDesigner
	notifier.broadcast();
}

while( true ) {
	pulse => now;
	spork ~ playImpact();
}
```

At the beginning, there will be 2 channels of output by default. In TouchDesigner, we can run the Python code:
```python
op('chuckaudio1').broadcast_event('pulse')
```

This will spork a shred of `playImpact()`, which will play a short sound. After 1 second of playing the sound, ChucK will broadcast an event named "notifier" back to TouchDesigner. This event will show up in the `getGlobalEvent()` method, if "notifier" is in custom parameter `Event Variables`.
