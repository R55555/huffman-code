# huffman-code
My implementation of Huffman code algorithm
<br />
huffman.cpp is just to print the huffman code
<br />
huff-enc-dec.cpp can be used to encode and decode a file. 
Currently it is done together. encode function can encode and store as encode.huff decode takes this file and print to output.txt.
Changes can be made to the interface so that we can ask for what operation to perform.
<br />
Sample usage: 
<br />g++ huff-enc-dec.cpp -o huff
<br />./huff filename
