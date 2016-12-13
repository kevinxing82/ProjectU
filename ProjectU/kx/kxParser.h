#pragma once
#include <iostream>

namespace KevinX
{
#define PARSER_BUFFER_SIZE        256
#define PARSER_MAX_COMMENT 16

#define PATTERN_MAX_ARGS        16
#define PATTERN_BUFFER_SIZE      80

	class kxParser
	{
	public:
		kxParser();
		~kxParser();

		int Reset();
		int Open(char* filename);
		int Close();

		char* Getline(int mode);

		int SetComment(char* string);
		int PatternMatch(char* string, char* pattern, ...);

	public:
		FILE *fsteam;
		char buffer[PARSER_BUFFER_SIZE];
		int length;
		int num_lines;
		char comment[PARSER_MAX_COMMENT];

		char pstring[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];
		int num_pstrings;

		float pfloats[PATTERN_MAX_ARGS];
		int num_pfloats;
	};
}