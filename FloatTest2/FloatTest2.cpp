// FloatTest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <algorithm>

enum bit {
	zero,
	one
};

struct FloatParts {
	bit sign;
	std::vector<bit> exponent;
	std::vector<bit> fraction;
};

static std::vector<bit> to_bits(float f);
static bit to_bit(int index, float value);
static FloatParts to_float_parts(float value);
static int bits_to_int(int start, int length, std::vector<bit> bits);
static int bits_to_int(std::vector<bit> bits);
static void print_float_bits(std::vector<bit> bits);
static float float_from_parts(FloatParts parts);
static int bit_at(int frac, int i);
static std::vector<bit> bit_subarray(std::vector<bit>& src, int i, int len);

int _tmain(int argc, _TCHAR* argv[]){
	auto val = 0.7;
	auto bits = to_bits(val);
	auto parts = to_float_parts(val);

	printf("val = %f\n", val);
	printf("sign = %d\n", parts.sign);
	printf("exp = %d\n", bits_to_int(parts.exponent));
	printf("frac = %d\n", bits_to_int(parts.fraction));
	printf("calculated = %f\n", float_from_parts(parts));

	print_float_bits(bits);

	return 0;
}

std::vector<bit> to_bits(float f){
	std::vector<bit> bits; 
	for(int i = 0; i < 32; i++){
		bits.push_back(to_bit(i, f));
	}
	std::reverse(bits.begin(), bits.end());
	return bits;
}

bit to_bit(int index, float value){
	int ival = *((int *)(void *)&value);
	int bval = (ival >> index) & 0x01;
	if(bval) return one;
	return zero;
}

FloatParts to_float_parts(float value){
	FloatParts parts;
	auto bits = to_bits(value);
	parts.sign = bits[0];
	parts.exponent = bit_subarray(bits, 1, 8);
	parts.fraction = bit_subarray(bits, 9, 23);
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
	for(int i = 0; i < 32; i++){
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
	int sign = parts.sign > 0 ? -1 : 1;
	int e = bits_to_int(parts.exponent) - 127;
	double exp = pow((double)2.0, (double)e);
	double frac = 0;
	int fraci = bits_to_int(parts.fraction);
	frac += 1;
	for(int i = 0; i < 23; i++){
		int x = bit_at(fraci, i + 9);
		frac += x * pow(2.0, -i);
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