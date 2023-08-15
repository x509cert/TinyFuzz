#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sal.h>

// Written by: Michael Howard (mikehow@microsoft.com)
// Microsoft Corporation
// I have tried to keep this code as simple as possible.
// It is not meant to be a good fuzzer, just a simple one.
// And it makes little use of C++ idioms to make it easier to use. 
// ie; no use of STL

bool Fuzz(
	_Inout_updates_bytes_(*pLen) char* pBuf,
	_Inout_						 size_t* pLen,
	_In_range_(5, 100) 			 size_t fuzzPercent) noexcept {

	// check for nulls
	if (pBuf == nullptr || pLen == nullptr) {
		return false;
	}

	constexpr size_t MIN_BUFF_LEN = 8;

	// if data is too small to fuzz, return false
	if (*pLen < MIN_BUFF_LEN) {
		return false;
	}

	if (fuzzPercent > 100) {
		fuzzPercent = 10;
	}

	if (fuzzPercent < 5) {
		fuzzPercent = 5;
	}

	// don't fuzz everything
	if ((rand() % 100) > 25) {
		return false;
	}

	// get a random range to fuzz
	unsigned int start = rand() % *pLen;
	unsigned int end = rand() % *pLen;
	if (start > end) {
		const unsigned int tmp = start;
		start = end;
		end = tmp;
	}

	// don't fuzz if the range is too small
	if (end - start <= MIN_BUFF_LEN / 2) {
		return false;
	}

	// how many loops through the fuzzer?
	const unsigned int iterations = 1 + rand() % 10;
	for (unsigned int i = 0; i < iterations; i++) {

		const unsigned int skip = rand() % 10 > 7 ? 1 + rand() % 10 : 1;
		const unsigned int whichMutation = rand() % 8;
		unsigned int j = 0;

		switch (whichMutation) {
			// set the range to a random byte
			case 0:
			{
				const char byte = rand() % 256;
				for (j = start; j < end; j += skip) {
					pBuf[j] = byte;
				}
			}
			break;

			// write random bytes to the range
			case 1:
			{
				for (j = start; j < end; j += skip) {
					pBuf[j] = rand() % 256;
				}
			}
			break;

			// set upper bit
			case 2:
				for (j = start; j < end; j += skip) {
					pBuf[j] |= 0x80;
				}
				break;

			// reset upper bit
			case 3:
				for (j = start; j < end; j += skip) {
					pBuf[j] &= 0x7F;
				}
				break;

			// set the first zero-byte found to non-zero
			case 4:
			{
				for (j = start; j < end; j += skip) {
					if (pBuf[j] == 0) {
						pBuf[j] = rand() % 256;
						break;
					}
				}
			}
			break;

			// insert interesting numbers
			case 5:
			{
				const int interestingNum[] = { 0,1,7,8,9,15,16,17,31,32,33,63,64,65,127,128,129,191,192,193,223,224,225,239,240,241,247,248,249,253,254,255 };
				for (j = start; j < end; j += skip) {
					pBuf[j] = (char)interestingNum[rand() % _countof(interestingNum)];
				}
			}
			break;

			// interesting characters
			case 6:
			{
				const char interestingChar[] = { ':', ';', '<', '>', '\\', '/', '.'};
				for (j = start; j < end; j += skip) {
					pBuf[j] = interestingChar[rand() % _countof(interestingChar)];
				}
			}
			break;

			// truncate
			case 7:
				*pLen = end;
				break;

			default:
				break;
		}
	}

	return true;
}

int main()
{
	size_t count = 0;
	constexpr size_t fuzzPercent = 10;

	while (1) {
		char buf[201];
		memset(buf, 'A', sizeof(buf));

		size_t len = sizeof(buf);
		const bool fOk = Fuzz(buf, &len, fuzzPercent);

		if (fOk) {
			char buf2[201];
			memcpy_s(buf2, sizeof(buf2), buf, len);
			buf2[len-1] = 0;

			printf("\n[iter: %zu] size=%zu\n\t%s ", 
				++count, 
				len, 
				buf2);
		}
	}
}
