#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include <string>

#include "Scene.hpp"

/**
 * A tokenizer that analyzes the input stream, and splits it into tokens. 
 */
class Tokenizer
{
public:
	Tokenizer(std::istream& stream);

	/**
	 * Returns whether or not there is another token in the input stream.
	 * 
	 * \return True if there is another token
	 */
	bool			hasNextToken();

	/**
	 * Returns the next string in the token stream. This does not advance the stream.
	 * To advance the stream, use forgetActiveTokens()
	 * 
	 * \return A string containing the next tokne
	 */
	std::string&	nextToken();
	
	/**
	 * Returns the next string in the token stream as a lowercase string
	 * 
	 * \return A lowercase string of the next token in the stream
	 */
	std::string&	nextTokenLower();

	/**
	 * Skips the remainder of the current line
	 */
	void			discardLine();

	/**
	 * Discards any of the tokens that are currently stored by the tokenizer. 
	 * 
	 */
	void			forgetActiveTokens();

	/**
	 * Resets the index
	 * 
	 */
	void			resetIndex();

	double			nextDouble();
	int				nextInt();

protected:
	bool		isComment(std::string& string);
	std::string nextString();

	std::istream&				stream;
	std::vector<std::string>	activeTokens;
	unsigned int				tokenIndex;
};

class Parser
{
public:
	Parser(std::istream& stream);

	void doParse(Animation& animation);

protected:
	void parseRenderSettings(Animation& animation);
	void parseFrame(Frame& frame);
	void parseObject(Object& object, std::string& name);
	void parseCamera(Camera& camera, std::string& name);
	void parseLight(Light& light, std::string& name);

	Tokenizer				tokenizer;
};

#endif//PARSER_HPP