// patch
adc => DelayL delay[2] => dac;
adc => dac;

// set delay parameters
.15::second => delay[0].max => delay[0].delay;
.15::second => delay[1].max => delay[1].delay;

// infinite time loop
while( true ) 1::second => now;

