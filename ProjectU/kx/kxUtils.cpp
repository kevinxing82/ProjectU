#include "kxUtils.h"
#include <string>

USING_KX;
int kxUtils::StripChars(char * string_in, char * string_out, char * strip_chars, int case_on)
{
	int num_extracts = 0,
		index_in = 0,
		index_out = 0,
		sindex,
		slength = strlen(strip_chars);

	if (!string_in || !string_out || strlen(string_in) == 0)
	{
		return 0;
	}

	if (!strip_chars || strlen(strip_chars) == 0)
	{
		strcpy_s(string_out, sizeof(string_out), string_in);
		return 0;
	}

	if (case_on == 1)
	{
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
			{
				if (string_in[index_in] == strip_chars[sindex])
				{
					index_in++;
					num_extracts++;
					break;
				}
			}
			if (sindex >= slength)
			{
				string_out[index_out++] = string_in[index_in++];
			}
		}
	}
	else
	{
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
			{
				if (toupper(string_in[index_in]) == toupper(strip_chars[sindex]))
				{
					index_in++;
					num_extracts++;
					break;
				}
			}
			if (sindex >= slength)
			{
				string_out[index_out++] = string_in[index_in++];
			}
		}
	}
	string_out[index_out] = 0;
	return (num_extracts);
}

int kxUtils::ReplaceChars(char * string_in, char * string_out, char * replace_chars, char rep_char, int case_on)
{
	int num_replacements = 0,
		index_in = 0,
		index_out = 0,
		sindex,
		slength = strlen(replace_chars);

	if (!string_in || !string_out || strlen(string_in) == 0)
	{
		return 0;
	}

	if (!replace_chars || strlen(replace_chars) == 0)
	{
		strcpy_s(string_out, sizeof(string_out), string_in);
		return 0;
	}

	if (case_on == 1)
	{
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
			{
				if (string_in[index_in] == replace_chars[sindex])
				{
					string_out[index_out++] = rep_char;
					index_in++;
					num_replacements++;
					break;
				}
			}

			if (sindex >= slength)
			{
				string_out[index_out++] = string_in[index_in++];
			}
		}
	}
	else
	{
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
			{
				if (toupper(string_in[index_in]) == toupper(replace_chars[sindex]))
				{
					string_out[index_out++] = rep_char;
					index_in++;
					num_replacements++;
					break;
				}
			}

			if (sindex >= slength)
			{
				string_out[index_out++] = string_in[index_in++];
			}
		}
	}

	string_out[index_out] = 0;
	return (num_replacements);
}

char *kxUtils::StringRtrim(char * string)
{
	int sindex = 0;
	int slength = strlen(string);
	if (!string || slength == 0)
	{
		return(string);
	}
	sindex = slength - 1;
	while (isspace(string[sindex]) && sindex >= 0)
	{
		string[sindex--] = 0;
	}
	return (string);
}

char *kxUtils::StringLtrim(char * string)
{
	int sindex = 0;
	int slength = strlen(string);
	if (!string || slength == 0)
	{
		return (string);
	}
	while (isspace(string[sindex]) && sindex < slength)
	{
		string[sindex++] = 0;
	}
	int  size = sizeof(char)*slength;
	memmove_s((void*)string, sizeof(char)*slength, (void*)&string[sindex], (slength - sindex));
	return (string);
}
