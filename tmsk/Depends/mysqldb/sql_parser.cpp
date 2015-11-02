#include "sql_parser.h"

int sql_parser::parse_column_list(const char* inputString, char* columns[])
{
	this->reset();
	m_dataBuffer.clear();

	m_input = inputString;

	int columnCount = 0;
	
	while(1)
	{
		Token tok = this->_readName();
		if(tok.type != Token::tok_name)
			return -1;
		
		columns[columnCount++] = (char*)m_dataBuffer._end();
		m_dataBuffer.push_back(m_tokString.c_str(), m_tokString.size()+1);
		
		tok = this->_readComma();
		if(tok.type == Token::tok_eof)
			break;

		if(tok.type != Token::tok_comma)
			return -1;
	}
	
	return columnCount;
}

int sql_parser::parse_column_value_list(const char* inputString, char* columns[], char* columnVals[])
{
	this->reset();
	m_dataBuffer.clear();

	m_input = inputString;

	int columnCount = 0;

	while(1)
	{
		Token tok = this->_readName();
		if(tok.type != Token::tok_name)
			return columnCount ? columnCount : -1;

		columns[columnCount] = (char*)m_dataBuffer._end();
		m_dataBuffer.push_back(m_tokString.c_str(), m_tokString.size()+1);

		tok = this->_readEqual();
		if(tok.type == Token::tok_eof)
			return -1;

		tok = this->_readValue();

		if(tok.type == Token::tok_value)
		{
			columnVals[columnCount++] = (char*)m_dataBuffer._end();
			m_dataBuffer.push_back(m_tokString.c_str(), m_tokString.size()+1);
		}
		else if(tok.type == Token::tok_literal)
		{
			columnVals[columnCount++] = (char*)m_dataBuffer._end();
			m_dataBuffer.push_back(m_tokString.c_str(), m_tokString.size()+1);
		}
		else
		{
			return -1;
		}
	}

	return columnCount;
}

void sql_parser::reset()
{
	m_errCode = 0;
	m_errText.clear();
	m_nextPos = 0;
	m_input = NULL;
	m_parseStack.clear();
	m_tokString.clear();
	m_dataBuffer.clear();
}

void sql_parser::_skipBlank()
{
	while(1)
	{
		char ch = m_input[m_nextPos];
		if(!ch) return;
		if(ch == ' ') m_nextPos++;
		else if(ch == '\t') m_nextPos++;
		else if(ch == '\r') m_nextPos++;
		else if(ch == '\n') m_nextPos++;
		else break;
	}
}

sql_parser::Token sql_parser::_readName()
{
	m_tokString.clear();
	this->_skipBlank();

	Token tok;
	tok.type = Token::tok_name;
	tok.from_pos = m_nextPos;

	char ch = m_input[m_nextPos];
	if(!ch)
	{
		return Token(Token::tok_eof);
	}
	else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		m_tokString.append(ch); m_nextPos++;
		while(1)
		{
			ch = m_input[m_nextPos];
			if(!ch)
			{
				tok.end_pos = m_nextPos;
				return tok;
			}
			else if ((ch == '_') || (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
			{
				m_tokString.append(ch);
				m_nextPos++;
			}
			else
			{
				tok.end_pos = m_nextPos;
				return tok;
			}
		}
	}
	else
	{
		return Token();
	}
}

sql_parser::Token sql_parser::_readValue()
{
	m_parseStack.clear();
	m_tokString.clear();

	this->_skipBlank();

	Token tok;

	char ch = m_input[m_nextPos];
	switch(ch)
	{
	case '\0':
		return Token(Token::tok_eof);
	case '\'':
		{
			Token tok(Token::tok_single_quotes);
			tok.from_pos = m_nextPos++;
			tok.end_pos = m_nextPos;
			m_parseStack.push_back(tok);
			return this->_readLiteral();
		}
	case '\"':
		{
			Token tok(Token::tok_double_quotes);
			tok.from_pos = m_nextPos++;
			tok.end_pos = m_nextPos;
			m_parseStack.push_back(tok);
			return this->_readLiteral();
		}
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return this->_readNumberValue();
	default:
		return Token();
	}

	return Token();
}

sql_parser::Token sql_parser::_readLiteral()
{
	while(1)
	{
		char ch = m_input[m_nextPos];
		switch(ch)
		{
		case '\0': return Token();
		case '\'':
			{
				ASSERT(!m_parseStack.empty());
				Token& tok = m_parseStack.back();
				if(tok.type == Token::tok_single_quotes)
				{
					m_nextPos++;
					Token ret(Token::tok_literal);
					ret.from_pos = tok.from_pos;
					ret.end_pos = m_nextPos;
					return ret;
				}
				else
				{
					m_nextPos++;
					m_tokString.append(ch);
					break;
				}
			}
		case '\"':
			{
				Token& tok = m_parseStack.back();
				if(tok.type == Token::tok_double_quotes)
				{
					m_nextPos++;
					Token ret(Token::tok_literal);
					ret.from_pos = tok.from_pos;
					ret.end_pos = m_nextPos;
					return ret;
				}
				else
				{
					m_nextPos++;
					m_tokString.append(ch);
					break;
				}
			}
		case '\\':
			{
				m_nextPos++;
				ch = m_input[m_nextPos];
				switch(ch)
				{
				case '\0': return Token();
				case 'r': m_nextPos++; m_tokString.append('\r'); break;
				case 'n': m_nextPos++; m_tokString.append('\n'); break;
				case 't': m_nextPos++; m_tokString.append('\t'); break;
				case 'b': m_nextPos++; m_tokString.append('\b'); break;
				default:  m_nextPos++; m_tokString.append( ch ); break;
				}
				break;
			}
		default:
			{
				m_nextPos++; m_tokString.append( ch );
				break;
			}
		}
	}

	return Token();
}

sql_parser::Token sql_parser::_readNumberValue()
{
	m_tokString.clear();
	this->_skipBlank();

	BOOL dot = FALSE;
	Token tok(Token::tok_value);
	tok.from_pos = m_nextPos;

	char ch = m_input[m_nextPos];
	switch(ch)
	{
	case '\0': return Token();
	case '.':
		{
			if(dot) return Token();
			dot = TRUE;
			m_nextPos++;
			m_tokString.append(ch);
			break;
		}
	case '0':case '1':case '2':case '3':case '4':
	case '5':case '6':case '7':case '8':case '9':
		{
			m_nextPos++;
			m_tokString.append(ch);
			break;
		}
	default:
		{
			return Token();
		}
	}

	while(1)
	{
		ch = m_input[m_nextPos];
		switch(ch)
		{
		case '.':
			{
				if(dot) return Token();
				dot = TRUE;
				m_nextPos++;
				m_tokString.append(ch);
				break;
			}
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			{
				m_nextPos++;
				m_tokString.append(ch);
				break;
			}
		default:
			{
				tok.end_pos = m_nextPos;
				return tok;
			}
		}
	}

	return Token();
}

sql_parser::Token sql_parser::_readEqual()
{
	this->_skipBlank();

	Token tok(Token::tok_equal);
	tok.from_pos = m_nextPos;

	char ch = m_input[m_nextPos];
	if(!ch) return Token(Token::tok_eof);
	if(ch == '=')
	{
		tok.end_pos = ++m_nextPos;
		return tok;
	}

	return Token();
}

sql_parser::Token sql_parser::_readComma()
{
	this->_skipBlank();

	Token tok(Token::tok_comma);
	tok.from_pos = m_nextPos;

	char ch = m_input[m_nextPos];
	if(!ch) return Token(Token::tok_eof);
	if(ch == ',')
	{
		tok.end_pos = ++m_nextPos;
		return tok;
	}

	return Token();
}
