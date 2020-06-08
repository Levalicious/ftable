# ftable
A decently fast hash table written in C based on https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/.

Thanks to Malte Skarupke for the writeup of his implementation.

This uses a prime modulus, at some point I might attempt using fibonacci hashing as recommended by Skarupke.

If you can recommend or implement any optimizations for it, please feel free.


## Stats from last time I ran it:

This is timing data from an insert of 8 million keys.

key = i, val = i for 8 mill to 1

Inserts: 18181.818359 per ms, 440 ms for 8000000 keys

55.070999 ns per insert.

Reads: 133333.328125 per ms, 60 ms for 8000000 keys

7.508125 ns per read.

Memory use: 316079480 bytes.
