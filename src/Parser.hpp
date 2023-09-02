#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include <string>

#include "Scene.hpp"

/**
 * A tokenizer that analyzes the input stream, and splits it into tokens. 
 * 
 * Tokens are scanned into a list of tokens, which can be traversed through
 * as a queue, and these stored tokens can be scanned as many times as necessary
 * until the parser tells the tokenizer it can forget them.
 */
class Tokenizer
{
public:
	/**
	 * Constructs the tokenizer with the specified input stream
	 * 
	 * @param stream The input stream to tokenize
	 */
	Tokenizer(std::istream& stream);

	/**
	 * Returns whether or not there is another token in the input stream.
	 * 
	 * @return True if there is another token
	 */
	bool			hasNextToken();

	/**
	 * Returns the next string in the token stream. This does not advance the stream.
	 * To advance the stream, use forgetActiveTokens()
	 * 
	 * @return A string containing the next tokne
	 */
	std::string&	nextToken();
	
	/**
	 * Returns the next string in the token stream as a lowercase string
	 * 
	 * @return A lowercase string of the next token in the stream
	 */
	std::string&	nextTokenLower();

	/**
	 * Skips the remainder of the current line
	 */
	void			discardLine();

	/**
	 * Discards any of the tokens that are currently stored by the tokenizer. 
	 */
	void			forgetActiveTokens();

	/**
	 * Resets the current token pointer to the first token read, since
	 * the last call to forgetActiveTokens()
	 */
	void			resetIndex();

	/**
	 * Returns the next token in the stream as a double
	 * 
	 * @return The value of the token as a double
	 */
	double			nextDouble();

	/**
	 * Returns the next token in the stream as an integer
	 * 
	 * \return The value of the token as an integer
	 */
	int				nextInt();

protected:
	/**
	 * Returns whether or not the passed string is a comment
	 * 
	 * @param string The string to check
	 * @return True if the string is a comment
	 */
	bool		isComment(std::string& string);

	/**
	 * Returns the next string in the token stream
	 * 
	 * @return The string read into the tokenizer
	 */
	std::string nextString();

	/// The stream being scanned
	std::istream&				stream;

	/// A list of currently active tokens
	std::vector<std::string>	activeTokens;

	/// The current token in the list 
	unsigned int				tokenIndex;
};

/**
 * Parser for an input file
 */
class Parser
{
public:
	/**
	 * Construct the parser with the specified input stream
	 * 
	 * @param stream The input stream to parse
	 */
	Parser(std::istream& stream);

	/**
	 * Parse the file into an animation structure
	 * 
	 * @param animation The animation structure to place the resulting
	 *                  parse in
	 */
	void doParse(Animation& animation);

protected:
	/**
	 * Parses the render settings block
	 * 
	 * @param animation The animation structure to place the result in
	 */
	void parseRenderSettings(Animation& animation);

	/**
	 * Parses a single frame block within the input
	 * 
	 * @param frame The frame object to put the result in
	 */
	void parseFrame(Frame& frame);

	/**
	 * Parses a single object block within the input file
	 * 
	 * @param object The object to place the result in
	 * @param name The name associated with the object
	 */
	void parseObject(Object& object, std::string& name);

	/**
	 * Parses a single camera block within the input file
	 * 
	 * @param camera The camera object to place the result in
	 * @param name The name associated with the camera
	 */
	void parseCamera(Camera& camera, std::string& name);

	/**
	 * Parses a single light block within the input file
	 * 
	 * @param light The light object to place the result in
	 * @param name The name of the light object
	 */
	void parseLight(Light& light, std::string& name);

	/// The tokenizer that this parser uses
	Tokenizer				tokenizer;
};

#endif//PARSER_HPP