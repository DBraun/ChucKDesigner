// number of tracks
7 => int NUM_TRACKS;

// TouchDesigner sets these int arrays, one per track
global int sequence0[4];
global int sequence1[4];
global int sequence2[4];
global int sequence3[4];
global int sequence4[8];
global int sequence5[8];
global int sequence6[16];

// ChucK will own the following global variables,
// and TouchDesigner will receive them in callbacks
global Event notifier0;
global Event notifier1;
global Event notifier2;
global Event notifier3;
global Event notifier4;
global Event notifier5;
global Event notifier6;

global float adsr_curves[NUM_TRACKS];

// playheadPos loops between 0 and 1 every measure
global float playheadPos;

// Pick your own beats per minute.
110. => float BPM;

// Our video rate is 60 frames per second,
// so we pick something less than (1000./60.)=16.6 ms
10::ms => dur POS_RATE;

// MEASURE_SEC is the amount of seconds in 4 beats
(60. * 4./ BPM)::second => dur MEASURE_SEC;

// posInc is how much playheadPos needs to increment
// when now advances by POS_RATE
POS_RATE/MEASURE_SEC => float posInc;

class Track {

    string myName;
    int mySeq[];

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
                   int seq[], Event notifier) {
        id => trackID;
        name => myName;
        seq @=> mySeq;
        me.dir() + file_path => buf.read;
        0 => buf.pos;
        0. => buf.rate;
        notifier @=> myNotifier;

        (62.5::ms, 20::ms, 1., 62.5::ms) => adsr.set;
        0.2 => buf.gain;
    }

    fun void play() {

        while (true) {

            (playheadPos * mySeq.size()) $ int => int newZone;

            if (newZone != previousZone || previousPlayhead > playheadPos) {
                if (mySeq[newZone]) {
                    // <<< "Note! " + myName>>>;
                    now + minSustain => offTime;
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

tracks[0].setup(0, "track0", "Crash2.wav",     sequence0, notifier0);
tracks[1].setup(1, "track1", "House_Clap.wav", sequence1, notifier1);
tracks[2].setup(2, "track2", "bass slide.wav", sequence2, notifier2);
tracks[3].setup(3, "track3", "WaterDrop.wav",  sequence3, notifier3);
tracks[4].setup(4, "track4", "kick.wav",       sequence4, notifier4);
tracks[5].setup(5, "track5", "House_Clap.wav", sequence5, notifier5);
tracks[6].setup(6, "track6", "Closed Hat.wav", sequence6, notifier6);

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