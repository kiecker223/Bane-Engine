#pragma once

#include "Common.h"
#include "Common/StringHelpers.h"


// Gross
// namespace ShaderIO
// {
// 	typedef enum ETOKEN_TYPE
// 	{
// 		TOKEN_TYPE_TYPE_DECLARATION,
// 		TOKEN_TYPE_VARIABLE_DECLARATION,
// 		TOKEN_TYPE_SEMANTIC_TOKEN,
// 		TOKEN_TYPE_SEMANTIC_DECLARATION,
// 		TOKEN_TYPE_FUNCTION_DECLARATION,
// 		TOKEN_TYPE_STRUCT_DECLARATION, 
// 		TOKEN_TYPE_BEGIN_SCOPE,
// 		TOKEN_TYPE_END_SCOPE,
// 		TOKEN_TYPE_FUNCTION_PARAMETER_DECLARATION_BEGIN,
// 		TOKEN_TYPE_FUNCTION_PARAMETER_DELCARATION_END
// 	};
// 
// 	class Token
// 	{
// 	public:
// 		Token() = delete;
// 		Token(std::string Str, ETOKEN_TYPE TokenType) :
// 			m_Str(Str),
// 			m_TokenType(TokenType)
// 		{
// 		}
// 
// 		inline std::string GetStr() const
// 		{
// 			return m_Str;
// 		}
// 
// 		inline ETOKEN_TYPE GetTokenType() const
// 		{
// 			return m_TokenType;
// 		}
// 
// 	private:
// 		std::string m_Str;
// 		ETOKEN_TYPE m_TokenType;
// 	};
// 
// 
// 	class TokenStream
// 	{
// 	public:
// 		
// 		void InitializeStream(const std::string& InRawData);
// 		std::vector<std::string> Tokenize(std::string InData);
// 
// 		std::vector<Token> Tokens;
// 
// 	};
// 
// }
// 
