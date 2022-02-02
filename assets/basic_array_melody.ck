// Connect a Sine oscillator to the digital-to-audio-converter
SinOsc sine => dac;

// an array of some notes in a scale
[ 0, 2, 4, 7, 9, 11 ] @=> int scale[];

// infinite time loop
while( true )
{
	scale[Math.random2(0,scale.size()-1)] => int randomNote;
    
    33 +=> randomNote;

	// use midi-to-frequency function
	Math.random2(0,3) * 12 + randomNote => Std.mtof => sine.freq;
	//sine.freq(Std.mtof(33 + Math.random2(0,3) * 12 + randomNote));

	// chuck time to now.
	100::ms => now;
}