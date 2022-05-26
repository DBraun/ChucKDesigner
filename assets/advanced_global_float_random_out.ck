SinOsc foo => dac;

440. => foo.freq;

Math.random2f(.2,.5) => global float randFloat;

while( true )
{	
	Math.random2f(.0,1.0) => randFloat;

	10::ms => now;
}