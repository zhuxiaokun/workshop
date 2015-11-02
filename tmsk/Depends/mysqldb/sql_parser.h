#pragma once
#include <System/Collections/KString.h>
#include <System/Collections/DynArray.h>
#include <System/Memory/KDataBuffer.h>

class sql_parser
{
public:
	struct Token
	{
		enum Type
		{
			tok_none,
			tok_equal,
			tok_comma,
			tok_name,
			tok_single_quotes,
			tok_double_quotes,
			tok_value,
			tok_literal,
			tok_eof,
		};
		
		Type type;
		int from_pos;
		int end_pos;

		Token():type(tok_none)
		{
		}
		Token(Type t):type(t)
		{
		}
		void reset()
		{
			type = tok_none;
			from_pos = end_pos = 0;
		}
	};
	typedef JG_C::DynArray<Token> ParseStack;

public:
	int m_errCode;
	JG_C::KString<256> m_errText;
	
	int m_nextPos;
	const char* m_input;
	
	ParseStack m_parseStack;
	JG_C::KDString<1024> m_tokString;
	JG_M::KDataBufferTmpl<1024*10> m_dataBuffer;

public:
	int parse_column_list(const char* inputString, char* columns[]);
	int parse_column_value_list(const char* inputString, char* columns[], char* columnVals[]);

public:
	void reset();
	void _skipBlank();
	Token _readName();
	Token _readValue();
	Token _readLiteral();
	Token _readNumberValue();
	Token _readEqual();
	Token _readComma();
};

DECLARE_SIMPLE_TYPE(sql_parser::Token);
