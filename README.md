# zlib-inflate-simple

This is a very simple/clean but highly inefficient C++ implementation of the zlib inflate (decompression) algorithm for folks studying information theory. This is meant as a complement to reading the actual [RFC](https://www.ietf.org/rfc/rfc1951.txt)... since it is somewhat confusing.

The code was also written to clearly demonstrate how Huffman coding plays into all of this.

The entire implementation (including the header) is **334 lines**. If you do manage to optimize it, you could do great justice to your global namespace (in terms of cleanliness ;).

# Build prerequisites

Visual Studio (with C++) or G++ or any C++ compiler you can get your hands on.

# How to build

**On Windows with VC++:** just run build.bat. It'll try to even find the VC environment variables batch script for you. Then it'll proceed to compile the code with the MS compiler. The executable tests the code against zlib itself.

**With literally anything else:** you need to compile it against the zlib source code provided. It doesn't really need this... this is just for testing.

The only files required to use the code are just `zlibinflate.cpp` and `zlibinflate.h` and everything is in the **COMPRESSION** namespace.

*If someone is willing to make a build file for this it would make me very happy :).*

# License

M.I.T

Knock yourself out.