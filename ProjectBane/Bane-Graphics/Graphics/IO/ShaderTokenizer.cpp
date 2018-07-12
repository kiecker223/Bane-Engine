#include "ShaderTokenizer.h"



/*
	So our tokenizer has a grammar
		Type Typename SemanticCharacter SEMANTIC_NAME
	
	struct Thingy
	{
		Type member SemanticCharacter SEMANTIC_NAME
	};

	ReturnType FunctionName(ParameterType parameter SemanticCharacter SEMANTIC_NAME) SemanticCharacter SEMANTIC_NAME
	{
	}
*/

// void ShaderIO::TokenStream::InitializeStream(const std::string& InRawData)
// {
// 	std::vector<std::string> ReadTokens;
// 	for (uint TokenIdx = 0; TokenIdx < ReadTokens.size(); TokenIdx++)
// 	{
// 		std::string RawData = ReadTokens[TokenIdx];
// 
// 		// so here various things are tested
// 	}
// }
// 
// std::vector<std::string> ShaderIO::TokenStream::Tokenize(std::string InData)
// {
// 	return std::vector<std::string>();
// }
