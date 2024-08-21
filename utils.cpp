typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long s64;
typedef unsigned long u64;

#define global_variable static;
#define internal static;

internal int
clamp(int min, int val, int max) {
	if (val > max) return max;
	if (val < min) return min;
	return val;
}