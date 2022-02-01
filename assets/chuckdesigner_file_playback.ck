// sound file
me.dir() + "60988__folktelemetry__crash-fast-14.wav" => string filename;
if( me.args() ) me.arg(0) => filename;

// 2 channel Sound Buffer
SndBuf2 buf => dac;

// load the file
filename => buf.read;

// time loop
while( true )
{
    0 => buf.pos;
    Math.random2f(.2,.5) => buf.gain;
    Math.random2f(.5,1.5) => buf.rate;
    500::ms => now;
}
