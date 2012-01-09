// FloatTest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <algorithm>

#define EIGHT_BIT

#ifdef THIRTY_TWO_BIT
#define TOTAL_BITS 32
#define EXPONENT_BITS 8
#define SIGNIFICAND_BITS 23
#define EXPONENT_BIAS 127
#endif

#ifdef EIGHT_BIT
#define TOTAL_BITS 8
#define EXPONENT_BITS 3
#define SIGNIFICAND_BITS 4
#define EXPONENT_BIAS 4
#endif

enum bit {
	zero,
	one
};

typedef char byte;

struct FloatParts {
	bit sign;
	std::vector<bit> exponent;
	std::vector<bit> fraction;
};

static std::vector<bit> float_to_bits(float f);
static std::vector<bit> byte_to_bits(byte b);

static bit float_to_bit(int index, float value);
static bit byte_to_bit(int index, byte value);
static FloatParts to_float_parts(std::vector<bit> bits);
static int bits_to_int(int start, int length, std::vector<bit> bits);
static int bits_to_int(std::vector<bit> bits);
static void print_float_bits(std::vector<bit> bits);
static float float_from_parts(FloatParts parts);
static int bit_at(int frac, int i);
static std::vector<bit> bit_subarray(std::vector<bit>& src, int i, int len);

static void enumerate();
static void test_float();

int _tmain(int argc, _TCHAR* argv[]){
	enumerate();
	return 0;
}

void enumerate(){
	for(int i = 0; i < 255; i++){
		FloatParts parts = to_float_parts(byte_to_bits((byte)i));
		printf("%d, ", (int)i);
		printf("%d, ", parts.sign);
		printf("%d, ", bits_to_int(parts.exponent));
		printf("%d, ", bits_to_int(parts.fraction));
		printf("%f\n", float_from_parts(parts));
	}
}

void test_float(){
	auto val = 0.0;
	auto bits = float_to_bits(val);
	auto parts = to_float_parts(float_to_bits(val));

	printf("val = %f\n", val);
	printf("sign = %d\n", parts.sign);
	printf("exp = %d\n", bits_to_int(parts.exponent));
	printf("frac = %d\n", bits_to_int(parts.fraction));
	printf("calculated = %f\n", float_from_parts(parts));

	print_float_bits(bits);
}

std::vector<bit> float_to_bits(float f){
	std::vector<bit> bits; 
	for(int i = 0; i < TOTAL_BITS; i++){
		bits.push_back(float_to_bit(i, f));
	}
	std::reverse(bits.begin(), bits.end());
	return bits;
}

std::vector<bit> byte_to_bits(byte b){
	std::vector<bit> bits;
	for(int i = 0; i < 8; i++){
		bits.push_back(byte_to_bit(i, b));
	}
	std::reverse(bits.begin(), bits.end());
	return bits;
}

bit float_to_bit(int index, float value){
	int ival = *((int *)(void *)&value);
	int bval = (ival >> index) & 0x01;
	if(bval) return one;
	return zero;
}

bit byte_to_bit(int index, byte value){
	int ival = *((int *)(void *)&value);
	int bval = (ival >> index) & 0x01;
	if(bval) return one;
	return zero;
}


FloatParts to_float_parts(std::vector<bit> bits){
	FloatParts parts;
	parts.sign = bits[0];
	parts.exponent = bit_subarray(bits, 1, EXPONENT_BITS);
	parts.fraction = bit_subarray(bits, EXPONENT_BITS+1, SIGNIFICAND_BITS);
	return parts;
}

int bits_to_int(int start, int length, std::vector<bit> bits){
	std::vector<bit> relevant_bits;
	for(int i = 0; i < length; i++){
		relevant_bits.push_back(bits.at(i + start));
	}
	return bits_to_int(relevant_bits);
}

int bits_to_int(std::vector<bit> bits){
	int value = 0;
	for(int i = 0; i < bits.size(); i++){
		bit b = bits.at(i);
		value <<= 1;
		if(b == one){
			value |= 0x01;
		} else {
			value &= ~(0x01);
		}
	}
	return value;
}

void print_float_bits(std::vector<bit> bits){
	for(int i = 0; i < TOTAL_BITS; i++){
		bit b = bits.at(i);
		putchar(b == zero ? '0' : '1');
		if(i == 0){
			putchar(' ');
		}
		if(i == 8){
			putchar(' ');
		}
	}
}

float float_from_parts(FloatParts parts){
	bool isZero = true;
	int sign = parts.sign > 0 ? -1 : 1;
	int e = bits_to_int(parts.exponent) - EXPONENT_BIAS;
	if(e != 0){
		isZero = false;
	}
	double exp = pow((double)2.0, (double)e);
	double frac = 0;
	frac += 1;
	for(int i = 0; i < SIGNIFICAND_BITS; i++){
		if(parts.fraction.at(i) == one){
			frac += pow(2.0, -(i + 1));
			isZero = false;
		}
	}
	if(isZero){
		return 0;
	}
	return sign * frac * exp;
}

static int bit_at(int frac, int i){
	int x = (frac << (i - 1)) & 0x80000000;
	if(x != 0){
		return 0x01;
	} else {
		return 0x00;
	}
}

std::vector<bit> bit_subarray(std::vector<bit>& src, int start, int len){
	std::vector<bit> bits;
	for(int i = 0; i < len; i++){
		bits.push_back(src.at(i + start));
	}
	return bits;
}