Step s[128] => dac;

while( true )
{
	for (0 => int i; i<128; i++) {
		Std.randf() => s[i].next;
	}

	10::ms => now;
}