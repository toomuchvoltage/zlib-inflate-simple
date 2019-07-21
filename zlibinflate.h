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

#ifndef TEST_DEFLATE_H
#define TEST_DEFLATE_H

#include <vector>
#include <algorithm>
#include <map>
#include <array>
#include <iostream>

namespace COMPRESSION
{
	class ZlibInflate
	{
	public:
		class bitstream
		{
		public:
			unsigned char *data;
			unsigned int index = 0;

		private:
			inline unsigned char grabBit();

		public:
			inline unsigned int grabBits(unsigned int numBits);
			inline unsigned int peekBitsRev(unsigned int numBits);
		};

	private:
		static std::vector <unsigned int> fixedAlphabet;
		static std::vector <unsigned int> fixedCodeBitLengths;
		static std::vector <unsigned int> fixedDistAlphabet;
		static std::vector <unsigned int> fixedDistCodeBitLengths;
		static unsigned int baseLengthsExtraBits[];
		static unsigned int baseLengths[];
		static unsigned int baseDistance[];
		static unsigned int baseDistanceExtraBits[];

		inline void getBitLengthCount(std::map <unsigned int, unsigned int> & bitLengthCount, std::vector <unsigned int> & codeBitLengths);
		inline void getFirstCode(std::map <unsigned int, unsigned int> & firstCodes, std::map <unsigned int, unsigned int> & bitLengthCount);
		inline void assignHuffmanCodes(std::vector <unsigned int> & assignedCodes, std::vector <unsigned int> & codeBitLengths, std::map <unsigned int, unsigned int> & firstCodes);
		inline void buildHuffmanAlphabet(std::vector <unsigned int> & alphabet, std::vector <unsigned int> & codeBitLengths);
		inline void buildFixedHuffmanAlphabet();
		inline unsigned int decodeHuffman(bitstream & inpStream, std::vector <unsigned int> & alphabet, std::vector <unsigned int> & codeBitLengths);

	public:
		void ZlibInflateDo (bitstream & input, std::vector <unsigned char> & output, unsigned int outputSize);
	};
}

#endif