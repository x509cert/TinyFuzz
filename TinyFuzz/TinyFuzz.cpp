#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <sal.h>
#include <memory>

/////////////////////////////////////////////////////////////
// Written by: Michael Howard (mikehow@microsoft.com)
// Azure Data Platform, Microsoft Corporation
/////////////////////////////////////////////////////////////
// I have tried to keep this code as simple as possible.
// It is not meant to be a good fuzzer, just a simple one.
// I also tried to keep C++ dependencies low.
/////////////////////////////////////////////////////////////
// Usage is:
//	Create the fuzzing object:
//		auto f = new Fuzzer(); 
//  You can pass a fuzz % value as an optional arg, this is
//  the % of data to fuzz. For example, if it's 10 then 
//  only 10% of calls to the Fuzz() method mutate the data.
//  Next, call:
//		f->Fuzz(buff, bufflen);
//  Which will mutate the data, or not! The return value is
//  true means data was fuzzed or false, means not fuzzed.
//  false does not necessarily mean there was an error, it
//  just means the data was not fuzzed.
//  On true, buff is the fuzzed buffer and bufflen is the 
//  buffer length which might not equal bufflen on the way in.
// 
// That's it!
/////////////////////////////////////////////////////////////

class Fuzzer {
	private:
		const static size_t MIN_BUFF_LEN = 8;
		const static size_t MIN_FUZZ_PERCENT = 5;
		size_t _fuzzPercent = MIN_FUZZ_PERCENT;

	public:
		Fuzzer(_In_range_(MIN_FUZZ_PERCENT, 100) size_t fuzzPercent = MIN_FUZZ_PERCENT) noexcept {
			_fuzzPercent = (fuzzPercent > 100 || fuzzPercent < MIN_FUZZ_PERCENT) 
				? MIN_FUZZ_PERCENT 
				: fuzzPercent;
		}

		bool Fuzz(_Inout_updates_bytes_(*pLen)	char* pBuf,
				  _Inout_						size_t* pLen) noexcept {

<<<<<<< HEAD
			// don't fuzz everything
			if ((rand() % 100) > MIN_FUZZ_PERCENT) {
				return false;
=======
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
	if ((rand() % 100) > fuzzPercent) {
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
>>>>>>> e017eab7ae687917fe2946179e92d3144487bd8e
			}

			// check for nulls
			if (pBuf == nullptr || pLen == nullptr) {
				return false;
			}

			// if data is too small to fuzz, return false
			if (*pLen < MIN_BUFF_LEN) {
				return false;
			}

			// get a random range to fuzz
			size_t start = rand() % *pLen;
			size_t end = rand() % *pLen;
			if (start > end) {
				const size_t tmp = start;
				start = end;
				end = tmp;
			}

			// don't fuzz if the range is too small
			if (end - start <= MIN_BUFF_LEN / 2) {
				return false;
			}

			// how many loops through the fuzzer?
			// most of the, 10%, keep it at one iteration
			const unsigned int iterations = rand() % 10 == 7 
				? 1 
				: 1 + rand() % 10;

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
						const char interestingChar[] = { ':', ';', '<', '>', '\\', '/', '.' };
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
};


int main()
{
	constexpr size_t fuzzPercent = 10;
	
	auto fuzz = std::make_unique<Fuzzer>(fuzzPercent);
	if (!fuzz) {
		return -1;
	}

	for (size_t iter = 0; iter < 1000000; iter++) {
		char buf[201];
		memset(buf, 'A', sizeof(buf));

		size_t len = sizeof(buf);
		const bool fOk = fuzz->Fuzz(buf, &len);

		if (fOk) {
			char buf2[201];
			memcpy_s(buf2, sizeof(buf2), buf, len);
			buf2[len-1] = 0;

			printf("\n[iter: %zu] size=%zu\n\t%s ", 
				iter, 
				len, 
				buf2);
		}
	}
}
