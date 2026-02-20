#include "../engine.h"

/** Like Borland C's setw(), but for POSIX-style file handles */
void WriteWord(word value, int fd) {
	_write(fd, &value, sizeof(word));
}


/** Like Borland C's getw(), but for POSIX-style file handles */
word ReadWord(int fd) {
	word result;
	_read(fd, &result, sizeof(word));
	return result;
}

word ISQRT(dword n) {
	dword r = 0;
	dword bit = 1UL << 30;

	while (bit > n)
		bit >>= 2;

	while (bit != 0) {
		if (n >= r + bit) {
			n -= r + bit;
			r = (r >> 1) + bit;
		} else {
			r >>= 1;
		}
		bit >>= 2;
	}

	return (word) r;
}

dword LZH_Compress(byte *source, byte *dest, dword size) {
}

dword LZH_Decompress(byte *source, byte *dest, dword size) {
	byte *pointer;
	dword size_temp;
	bool ultra_compression_flag;
	int src_index, dst_index;


	src_index = 0;
	dst_index = 0;

	// Get ultra compression flag at offset 0
	ultra_compression_flag = source[src_index];
	src_index++;
}