
#pragma once
#define N 100
typedef struct {
	BYTE word[N][N];
	int w;
	int h;
}ACODE;

typedef struct {
	char dst;
	float score;
	int w;
	int h;
	BYTE src[500];
}LCODE;

static LCODE sample[16] = {
	{ '0', 0, 11, 17, "0001111100000111011100011100011100111000111011100000111111000001111110000011111100000111111000001111110000011111100000111111000001111110000011101110001110011100011100011101110000011111000" },
	{ '1', 0,  9, 17, "000011000000111000111111000000111000000111000000111000000111000000111000000111000000111000000111000000111000000111000000111000000111000000111000111111111" },
	{ '2', 0, 11, 17, "0001111110001100001110110000001111100000011111100000111111000001110000000011000000001110000000111000000001100000000110000000110000000011000001101100000011010000001111111111111111111111110" },
	{ '3', 0, 11, 17, "0011111100001100011100111000011101110000111011100001110000000011100000001110000001111000000000111000000000111000000000111000000001111110000011111100000111111000011100110001111000111111000" },
	{ '4', 0, 12, 18, "000000011000000000111000000000111000000001011000000001011000000010011000000110011000000100011000001100011000011000011000010000011000110000011000001111111110000000011000000000011000000000011000000000011100000011111111" },
	{ '5', 0, 11, 17, "0011111111100111111111001100000000010000000000100000000011000000000110111110001110001110011000011100000000011100000000111000000001111110000011111100000111110000011100110001111000111111000" },
	{ '6', 0, 12, 17, "000011111100000110001110001100001110011000000000011000000000111000000000111011111000111110011110111100001110111000000111111000000111111000000111111000000111011100000111011100001110001110001110000111111000" },
	{ '7', 0, 11, 17, "0111111111111111111110111000001101100000010011000001100000000110000000001100000000110000000001100000000110000000001100000000011000000001110000000011100000000111000000001110000000011100000" },
	{ '8', 0, 11, 17, "0011111100001110001110111000001111110000011111100000111111100001110111100111000111111100001111111000111001111011100001110111000001111110000011111100000111111000001110111000111000011111000" },
	{ '9', 0, 11, 17, "0001111100001110001100011100011101110000011011100000111111000001111110000011111100000111111100011110111001111100111110111000000001110000000111000000001110011100011000111001110000111110000" },
	{ 'A', 0, 13, 18, "000000110000000000111000000000011100000000011110000000001111100000000101110000000010111000000011001100000001100111000000110011100000011001110000011111111000001100001110000110000111000011000011100011100001110001110000011101111100111111" },
	{ 'B', 0, 12, 17, "111111111000011100011100011100001110011100001110011100001110011100001110011100011100011111110000011100011100011100001110011100000111011100000111011100000111011100000111011100000111011100001110111111111000" },
	{ 'C', 0, 12, 17, "000011111110000110001110001110000111011100000011011100000011111000000000111000000000111000000000111000000000111000000000111000000000111000000000111000000011011100000011011100000110001110001100000011111000" },
	{ 'D', 0, 12, 17, "111111110000011100111100011100001110011100001110011100000111011100000111011100000111011100000111011100000111011100000111011100000111011100000111011100000110011100001110011100001110011100111100111111110000" },
	{ 'E', 0, 12, 17, "111111111110011100001110011100000111011100000011011100011000011100011000011100011000011111111000011100011000011100011000011100011000011100000000011100000000011100000011011100000011011100000110111111111110" },
	{ 'F', 0, 12, 17, "111111111110011100001110011100000011011100000011011100011000011100011000011100011000011111111000011100011000011100011000011100011000011100000000011100000000011100000000011100000000011100000000111110000000" },
};
int authcode(const char *authcodepath, char *text);