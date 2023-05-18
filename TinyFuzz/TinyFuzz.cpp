#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sal.h>

#define MIN_BUFF_LEN 8

bool Fuzz(
	_Inout_updates_bytes_(*pLen) char* pBuf, 
	_Inout_						 unsigned int *pLen) { 

	bool ok = true;

	// check for nulls
	if (pBuf == NULL || pLen == NULL) {
		return false;
	}
	
	// if data is too small to fuzz, return false
	if (*pLen < MIN_BUFF_LEN) {
		return false;
	}
	
	// don't fuzz everything
	if ((rand() % 100) > 25) {
		return false;
	}

	// get a random range to fuzz
	unsigned int start = rand() % *pLen;
	unsigned int end = rand() % *pLen;
	if (start > end) {
		unsigned int tmp = start;
		start = end;
		end = tmp;
	}

	// don't fuzz if the range is too small
	if (end - start <= MIN_BUFF_LEN/2) {
		return false;
	}

	// how many loops through the fuzzer?
	unsigned int iterations = 1 + rand() % 10;
	for (unsigned int i = 0; i < iterations; i++) {

		unsigned int skip = rand() % 10 > 7 ? 1 + rand() % 10 : 1;
		unsigned int whichMutation = rand() % 8;
		unsigned int j = 0;

		switch (whichMutation) {
			// set the range to a random byte
			case 0: 
				{
					char byte = rand() % 256;
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
					int interesting[] = { 0,1,7,8,9,15,16,17,31,32,33,63,64,65,127,128,129,191,192,193,223,224,225,239,240,241,247,248,249,253,254,255 };
					for (j = start; j < end; j += skip) {
						pBuf[j] = (char)interesting[rand() % (sizeof(interesting) / sizeof(int))];
					}
				}
				break;

			// interesting characters
			case 6: 
				{
					char interesting2[] = { ':', ';', '<', '>', '\\', '/' };
					for (j = start; j < end; j += skip) {
						pBuf[j] = interesting2[rand() % (sizeof(interesting2) / sizeof(char))];
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

	return ok;
}


int main()
{
	unsigned int count = 0;
	while (1) {
		char buf[201];
		memset(buf, 'A', sizeof(buf));

		unsigned int len = sizeof(buf);
		bool fOk = Fuzz(buf, &len);

		if (fOk) {
			char buf2[201];
			memcpy(buf2, buf, len);
			buf2[len-1] = 0;

			printf("\n[iter: %d] size=%d\n\t%s ", ++count, len, buf2);
		}
	}
}
