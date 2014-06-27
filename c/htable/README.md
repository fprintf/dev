 Simple C Hash Table implementation
 ===
 
 Description
 This is just a simple hash table that I wrote for use with the IRC bot 'cbot'. It does have various hash functions
 stored in the source but it is currently just using a shift_add_xor hash which is fast, simple and seems to be about as
 effective as the others on strings (Which is the required key format...currently...I will likely be switching this to use void
 types which is trivial to change..)
