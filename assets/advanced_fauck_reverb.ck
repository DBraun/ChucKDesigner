// name: reverb.ck
// desc: demo of Faust chugin in action!

// instantiate and connect faust => ck
adc => Faust reverb => dac;

// evaluate Faust code
reverb.eval(`
  process = dm.zita_light ;
`);

// print the parameters of the Faust object
reverb.dump();

1. => global float Faust_reverb_mix;

// time loop
while( true )
{
  reverb.v("/Zita Light/Dry/Wet Mix", Faust_reverb_mix);
  // advance time
  10::ms => now;
}