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


/***************************************************************************
   Tables provided by Sharlock93: https://handmade.network/forums/wip/t/2363-implementing_a_basic_png_reader_the_handmade_way
   
   These tables can also be found in the RFC as well: https://www.ietf.org/rfc/rfc1951.txt
****************************************************************************/


#include "zlibinflate.h"

std::vector <unsigned int> COMPRESSION::ZlibInflate::fixedAlphabet;
std::vector <unsigned int> COMPRESSION::ZlibInflate::fixedCodeBitLengths;
std::vector <unsigned int> COMPRESSION::ZlibInflate::fixedDistAlphabet;
std::vector <unsigned int> COMPRESSION::ZlibInflate::fixedDistCodeBitLengths;

unsigned int COMPRESSION::ZlibInflate::baseLengthsExtraBits[] = {
	0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
	1, 1, 1, 1, //265 - 268
	2, 2, 2, 2, //269 - 273
	3, 3, 3, 3, //274 - 276
	4, 4, 4, 4, //278 - 280
	5, 5, 5, 5, //281 - 284
	0           //285
};

unsigned int COMPRESSION::ZlibInflate::baseLengths[] = {
	3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
	11, 13, 15, 17,          //265 - 268
	19, 23, 27, 31,          //269 - 273
	35, 43, 51, 59,          //274 - 276
	67, 83, 99, 115,         //278 - 280
	131, 163, 195, 227,      //281 - 284
	258                      //285
};

unsigned int COMPRESSION::ZlibInflate::baseDistance[] = {
	1, 2, 3, 4,    //0-3
	5, 7,          //4-5
	9, 13,         //6-7
	17, 25,        //8-9
	33, 49,        //10-11
	65, 97,        //12-13
	129, 193,      //14-15
	257, 385,      //16-17
	513, 769,      //18-19
	1025, 1537,    //20-21
	2049, 3073,   //22-23
	4097, 6145,   //24-25
	8193, 12289,  //26-27
	16385, 24577, //28-29
	0   , 0      //30-31, error, shouldn't occur
};

unsigned int COMPRESSION::ZlibInflate::baseDistanceExtraBits[] = {
	0, 0, 0, 0, //0-3
	1, 1,       //4-5
	2, 2,       //6-7
	3, 3,       //8-9
	4, 4,       //10-11
	5, 5,       //12-13
	6, 6,       //14-15
	7, 7,       //16-17
	8, 8,       //18-19
	9, 9,       //20-21
	10, 10,    //22-23
	11, 11,    //24-25
	12, 12,    //26-27
	13, 13,     //28-29
	0 , 0      //30-31 error, they shouldn't occur
};

unsigned char COMPRESSION::ZlibInflate::bitstream::grabBit()
{
	if (data[index / 8] & (1 << (index % 8))) return 1;
	return 0;
}

unsigned int COMPRESSION::ZlibInflate::bitstream::grabBits(unsigned int numBits)
{
	unsigned int retVal = 0;
	for (int i = 0; i != numBits; i++) {
		retVal |= (unsigned int)grabBit() << i;
		index++;
	}
	return retVal;
}

unsigned int COMPRESSION::ZlibInflate::bitstream::peekBitsRev(unsigned int numBits)
{
	unsigned int cachedIndex = index;

	unsigned int retVal = 0;
	for (int i = 0; i != numBits; i++) {
		retVal |= (unsigned int)grabBit() << ((numBits-1) - i);
		index++;
	}

	index = cachedIndex;

	return retVal;
}

void COMPRESSION::ZlibInflate::getBitLengthCount(std::map<unsigned int, unsigned int>& bitLengthCount, std::vector<unsigned int>& codeBitLengths)
{
	for (int i = 0; i != codeBitLengths.size(); i++)
		bitLengthCount[codeBitLengths[i]] = 0;

	for (int i = 0; i != codeBitLengths.size(); i++)
		bitLengthCount[codeBitLengths[i]]++;
}

void COMPRESSION::ZlibInflate::getFirstCode(std::map<unsigned int, unsigned int>& firstCodes, std::map<unsigned int, unsigned int>& bitLengthCount)
{
	unsigned int code = 0;
	unsigned int prevBLCount = 0;
	for (unsigned int i = 1; i <= (unsigned int)bitLengthCount.size(); i++)
	{
		if (i >= 2) prevBLCount = bitLengthCount[i - 1];
		code = (code + prevBLCount) << 1;
		firstCodes[i] = code;
	}
}

void COMPRESSION::ZlibInflate::assignHuffmanCodes(std::vector<unsigned int>& assignedCodes, std::vector<unsigned int>& codeBitLengths, std::map<unsigned int, unsigned int>& firstCodes)
{
	assignedCodes.resize(codeBitLengths.size());

	for (unsigned int i = 0; i < (unsigned int)codeBitLengths.size(); i++)
		if (codeBitLengths[i])
			assignedCodes[i] = firstCodes[codeBitLengths[i]]++;
		else
			assignedCodes[i] = 0;
}

void COMPRESSION::ZlibInflate::buildHuffmanAlphabet(std::vector<unsigned int>& alphabet, std::vector<unsigned int>& codeBitLengths)
{
	std::map <unsigned int, unsigned int> bitLengthCount, firstCodes;

	getBitLengthCount(bitLengthCount, codeBitLengths);
	getFirstCode(firstCodes, bitLengthCount);
	assignHuffmanCodes(alphabet, codeBitLengths, firstCodes);
}

void COMPRESSION::ZlibInflate::buildFixedHuffmanAlphabet()
{
	if (fixedCodeBitLengths.size()) return;

	fixedCodeBitLengths.resize(288);
	fixedDistCodeBitLengths.resize(32);
	for (int i = 0; i <= 287; i++) {
		if (i >= 0 && i <= 143)
			fixedCodeBitLengths[i] = 8;
		else if (i >= 144 && i <= 255)
			fixedCodeBitLengths[i] = 9;
		else if (i >= 256 && i <= 279)
			fixedCodeBitLengths[i] = 7;
		else if (i >= 280 && i <= 287)
			fixedCodeBitLengths[i] = 8;
	}

	for (int i = 0; i != 32; i++)
		fixedDistCodeBitLengths[i] = 5;

	buildHuffmanAlphabet(fixedAlphabet, fixedCodeBitLengths);
	buildHuffmanAlphabet(fixedDistAlphabet, fixedDistCodeBitLengths);
}

unsigned int COMPRESSION::ZlibInflate::decodeHuffman(bitstream & inpStream, std::vector<unsigned int>& alphabet, std::vector<unsigned int>& codeBitLengths)
{
	for (unsigned int i = 0; i < (unsigned int)codeBitLengths.size(); i++) {
		if (codeBitLengths[i] == 0) continue;
		unsigned int code = inpStream.peekBitsRev(codeBitLengths[i]);
		if (alphabet[i] == code) {
			inpStream.grabBits(codeBitLengths[i]);
			return i;
		}
	}
	return 0;
}

void COMPRESSION::ZlibInflate::ZlibInflateDo(bitstream & input, std::vector<unsigned char>& output, unsigned int outputSize)
{
	input.grabBits(16); // zLib header

	output.reserve(outputSize);

	std::vector <unsigned int> codeLengthOfCodeLengthOrder = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	std::vector <unsigned int> codeLengthOfCodeLength = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	while (true)
	{
		unsigned int bfinal = input.grabBits(1);
		unsigned int btype = input.grabBits(2);

		if (btype == 0)
		{
			input.index += 5;
			unsigned int len = input.data[input.index / 8] + (input.data[(input.index / 8) + 1] << 8);
			input.index += 32;
			for (int i = 0; i != len; i++)
				output.push_back((unsigned char)input.grabBits(8));
		}
		else if (btype == 1 || btype == 2)
		{
			std::vector <unsigned int> litLenCodeBitLength, litLenAlphabet;
			std::vector <unsigned int> distCodeBitLength, distAlphabet;

			if (btype == 1)
			{
				buildFixedHuffmanAlphabet();
			}
			else
			{
				unsigned int hlit = input.grabBits(5) + 257;
				unsigned int hdist = input.grabBits(5) + 1;
				unsigned int hclen = input.grabBits(4) + 4;

				for (unsigned int i = 0; i < hclen; i++) {
					codeLengthOfCodeLength[codeLengthOfCodeLengthOrder[i]] = input.grabBits(3);
				}

				std::vector <unsigned int> litLenAndDistAlphabets;
				buildHuffmanAlphabet(litLenAndDistAlphabets, codeLengthOfCodeLength);

				std::vector <unsigned int> litLenAndDistTreesUnpacked;
				while (litLenAndDistTreesUnpacked.size() < (hdist + hlit))
				{
					unsigned int decodedValue = decodeHuffman(input, litLenAndDistAlphabets, codeLengthOfCodeLength);

					if (decodedValue < 16) {
						litLenAndDistTreesUnpacked.push_back(decodedValue);
						continue;
					}

					unsigned int repeatCount = 0;
					unsigned int codeLengthToRepeat = 0;

					switch (decodedValue)
					{
					case 16:
						repeatCount = input.grabBits(2) + 3;
						codeLengthToRepeat = litLenAndDistTreesUnpacked[litLenAndDistTreesUnpacked.size() - 1];
						break;
					case 17:
						repeatCount = input.grabBits(3) + 3;
						break;
					case 18:
						repeatCount = input.grabBits(7) + 11;
						break;
					}

					for (int i = 0; i != repeatCount; i++)
						litLenAndDistTreesUnpacked.push_back(codeLengthToRepeat);
				}

				litLenCodeBitLength = std::vector <unsigned int>(litLenAndDistTreesUnpacked.begin(), litLenAndDistTreesUnpacked.begin() + hlit);
				distCodeBitLength = std::vector <unsigned int>(litLenAndDistTreesUnpacked.begin() + hlit, litLenAndDistTreesUnpacked.end());

				buildHuffmanAlphabet(litLenAlphabet, litLenCodeBitLength);
				buildHuffmanAlphabet(distAlphabet, distCodeBitLength);
			}

			while (true)
			{
				unsigned int decodedSymbol = decodeHuffman(input, btype == 1 ? fixedAlphabet : litLenAlphabet, btype == 1 ? fixedCodeBitLengths : litLenCodeBitLength);
				if (decodedSymbol <= 255)
				{
					output.push_back((unsigned char)decodedSymbol);
				}
				else if (decodedSymbol == 256)
				{
					break;
				}
				else
				{
					unsigned int lengthIndex = decodedSymbol - 257;
					unsigned int totalLength = baseLengths[lengthIndex] + input.grabBits(baseLengthsExtraBits[lengthIndex]);
					unsigned int distCode = decodeHuffman(input, btype == 1 ? fixedDistAlphabet : distAlphabet, btype == 1 ? fixedDistCodeBitLengths : distCodeBitLength);
					unsigned int totalDist = baseDistance[distCode] + input.grabBits(baseDistanceExtraBits[distCode]);
					unsigned char *pos = &output[output.size() - totalDist];
					for (int i = 0; i != totalLength; i++) {
						output.push_back(*pos);
						pos = &output[output.size() - totalDist];
					}
				}
			}
		}

		if (bfinal) return;
	}
}
