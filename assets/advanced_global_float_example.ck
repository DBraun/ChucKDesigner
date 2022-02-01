SinOsc foo => dac;

300. => global float freq => foo.freq;

while( true )
{
	freq => foo.freq;

	10::ms => now;
}