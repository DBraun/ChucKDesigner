DelayL delay[2] => dac;

0.7 => global float gainDecay;
0.5 => global float delaySec;

1.0::second => dur delayMax;

gainDecay => delay[0].gain => delay[1].gain;

// feedback between each delay
delay[0] => delay[1];
delay[1] => delay[0];

// set delay parameters
delayMax => delay[0].max => delay[1].max;

adc => Gain mono => delay[0];

mono.gain(.6);

adc => dac;

// time loop
while( true )
{
    // use global variables to update parameters
	gainDecay => delay[0].gain => delay[1].gain;
	delaySec::second => delay[0].delay => delay[1].delay;
	
	// chuck time
    10::ms => now;
}
