// more music for replicants

me.dir() + "stk_rhodey.ck" => string filepath;

<<< "Adding shred: " + filepath >>>;

Machine.add(filepath);

// our main loop
while( true )
{ 
    100::ms => now;
}
