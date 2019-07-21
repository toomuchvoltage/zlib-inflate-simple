/****************************************************************************
Copyright(c) 2019 TooMuchVoltage Software Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

#include "zlibinflate.h"
#include "include/zlib.h"

using namespace COMPRESSION;

int main(void)
{
	char a[8000];
	char b[8000];
	
	for (int i = 0;i != 7998;i++)
	{
		if ( i % 3 == 0 )
			a[i] = '1';
		else if ( i % 3 == 1 )
			a[i] = '2';
		else if ( i % 3 == 2 )
			a[i] = '3';
	}
	a[7999] = '\0';

	z_stream defstream;
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	defstream.avail_in = (uInt)strlen(a) + 1; // size of input, string + terminator
	defstream.next_in = (Bytef *)a; // input char array
	defstream.avail_out = (uInt)sizeof(b); // size of output
	defstream.next_out = (Bytef *)b; // output char array

	deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
	deflate(&defstream, Z_FINISH);
	deflateEnd(&defstream);
	
	std::cout << "Compressed size: " << (unsigned int)((char*)defstream.next_out - b) << "\n\n";

	ZlibInflate inflator;
	ZlibInflate::bitstream inflatorBitStream;
	inflatorBitStream.data = (unsigned char *)b;
	std::vector <unsigned char>uncompressedOutput;
	inflator.ZlibInflateDo(inflatorBitStream, uncompressedOutput, (unsigned int)((char*)defstream.next_out - b));
	
	std::cout << "Uncompressed output: \n";

	for (unsigned char c : uncompressedOutput)
		std::cout << c;

	std::cout << "\n\n";

	return 0;
}