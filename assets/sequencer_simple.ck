// number of tracks
1 => int NUM_TRACKS;

// TouchDesigner sets these int arrays, one per track
global float noteVels[16];

// ChucK will own the following global variables,
// and TouchDesigner will receive them in callbacks
global Event notifier;

global float adsr_curves[NUM_TRACKS];

// playheadPos loops between 0 and 1 every measure
global float playheadPos;

// Pick your own beats per minute.
110. => global float BPM;

// update BPM once:
// Our video rate is 60 frames per second,
// so we pick something less than (1000./60.)=16.6 ms
10::ms => dur POS_RATE;

// MEASURE_SEC is the amount of seconds in 4 beats
(60. * 4./ BPM)::second => dur MEASURE_SEC;

// posInc is how much playheadPos needs to increment
// when now advances by POS_RATE
POS_RATE/MEASURE_SEC => float posInc;
10::ms => now; 
// end bpm

fun void updateBPM() {
    while(true) {
        // Our video rate is 60 frames per second,
        // so we pick something less than (1000./60.)=16.6 ms
        10::ms => POS_RATE;

        // MEASURE_SEC is the amount of seconds in 4 beats
        (60. * 4./ BPM)::second => MEASURE_SEC;

        // posInc is how much playheadPos needs to increment
        // when now advances by POS_RATE
        POS_RATE/MEASURE_SEC => posInc;
        10::ms => now; 
    }
}

spork ~updateBPM();

class Track {

    string myName;
    float mySeq[];

    SndBuf buf => Pan2 outlet;

    0 => int trackID;

    int previousZone;
    float previousPlayhead;

    -1 => previousZone;

    Event myNotifier;

    // ADSR adsr => blackhole;
    // In order for the adsr to "cook",
    // it needs something driving it,
    // so we'll use a Step UGen whose value is set to 1.
    Step step => ADSR adsr => blackhole;
    1. => step.next;

    .0625::second => dur minSustain;

    time offTime;

    fun void setup(int id, string name, string file_path,
                   float seq[], Event notifier) {
        id => trackID;
        name => myName;
        seq @=> mySeq;
        me.dir() + file_path => buf.read;
        0 => buf.pos;
        0. => buf.rate;
        notifier @=> myNotifier;

        (30.5::ms, 20::ms, 1., 62.5::ms) => adsr.set;
        0.2 => buf.gain;
    }

    fun void play() {

        while (true) {

            (playheadPos * mySeq.size()) $ int => int newZone;

            if (newZone != previousZone || previousPlayhead > playheadPos) {
                mySeq[newZone] => float vel;
                if (vel > 0.5) {
                    // <<< "Note! " + myName>>>;
                    now + minSustain => offTime;
                    vel => buf.gain;
                    adsr.keyOn();
                    0 => buf.pos;
                    1. => buf.rate;
                    myNotifier.broadcast();
                }
            }

            if (now >= offTime) {
                adsr.keyOff();
            }

            newZone => previousZone;
            playheadPos => previousPlayhead;

            adsr.last() => adsr_curves[trackID];

            // <<< adsr.last() >>>;

            POS_RATE => now;  
        }
    }
}

Track tracks[NUM_TRACKS];

tracks[0].setup(0, "track0", "Closed Hat.wav", noteVels, notifier);

Pan2 allTracks => dac;

for (int i; i< NUM_TRACKS; i++) {
    tracks[i].outlet => allTracks;
    spork ~tracks[i].play();
}

// updates the global playheadPos with fine granularity,
// for visualizing the playhead smoothly in TouchDesigner
fun void playheadPosUpdate()
{
    while( true )
    {
        // increment
        (playheadPos+posInc) % 1. => playheadPos;

        // <<< playheadPos >>>;

        // advance time
        POS_RATE => now;
    }
}

// this function will go forever
playheadPosUpdate();