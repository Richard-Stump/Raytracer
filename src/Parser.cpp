#include "Parser.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>

/**
 * Helper function to turn make a string lowercase
 * 
 * @param string The string to make lowercase
 * @return The string that was made lowercase
 */
static std::string& toLower(std::string& string)
{
	std::transform(
		string.begin(),
		string.end(),
		string.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);

	return string;
}

/**
 * Gets the index of a specified name in a list of names
 * 
 * \param names	The list of names
 * \param name	The name to get the index of
 * \return		The index, or -1 if there is none
 */
int getNameIndex(std::vector<std::string>& names, std::string& name)
{
	for (int i = 0; i < names.size(); i++)
		if (names[i] == name)
			return i;

	return -1;
}

//=============================================================
//						"TOKENIZER"
//=============================================================

Tokenizer::Tokenizer(std::istream& _stream) :
	stream(_stream),
	activeTokens(),
	tokenIndex{0}
{
	stream.exceptions(std::istream::eofbit);
}

bool Tokenizer::isComment(std::string& string)
{
	return (string.size() > 1) && (string.substr(0, 2) == "//");
}

std::string Tokenizer::nextString()
{
	std::string string;

	while (true) {
		stream >> string;

		if (isComment(string)) {
			stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} 
		else {
			if (string[0] == '\"' && string[string.size() - 1] != '\"') {
				char c;

				do {
					stream >> c;
					string += c;
				} while (c != '\"');
			}

			return string;
		}
	}
}

void Tokenizer::discardLine()
{
	stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool Tokenizer::hasNextToken()
{
	return tokenIndex < activeTokens.size() || !stream.eof();
}

std::string& Tokenizer::nextToken()
{
	if (tokenIndex >= activeTokens.size()) {
		activeTokens.push_back(nextString());
	}

	return activeTokens[tokenIndex++];
}

std::string& Tokenizer::nextTokenLower()
{
	return toLower(nextToken());
}

void Tokenizer::forgetActiveTokens()
{
	activeTokens.clear();
	tokenIndex = 0;
}

void Tokenizer::resetIndex()
{
	tokenIndex = 0;
}

double Tokenizer::nextDouble()
{
	return std::stod(nextToken());
}

int Tokenizer::nextInt()
{
	return std::stoi(nextToken());
}

//=============================================================
//							PARSER
//=============================================================

class NullObject : public Object
{
public:

	void parseProperty(std::string& name, Tokenizer& tokenizer)
	{
		tokenizer.discardLine();
	}
};

Parser::Parser(std::istream& stream) :
	tokenizer(stream)
{
}

void Parser::parseRenderSettings(Animation& animation) {
	std::cout << "Parsing render settings" << std::endl;
	
	tokenizer.nextToken();

	std::string token;
	while ((token = tokenizer.nextTokenLower()) != "}") {
		if (token == "resolution") {
			animation.width = tokenizer.nextInt();
			animation.height = tokenizer.nextInt();
		}
		else if (token == "maxdepth") {
			animation.maxDepth = tokenizer.nextInt();
		}
		else if (token == "samples") {
			animation.samples = glm::floor(glm::sqrt(tokenizer.nextDouble()));
		}
		else if (token == "loop") {
			animation.loop = true;
		}
		else if (token == "fps") {
			animation.fps = tokenizer.nextInt();
		}
	}

	tokenizer.forgetActiveTokens();
	std::cout << "Done parsing render settings" << std::endl;
}

void Parser::parseObject(Object& object, std::string& name)
{
	std::cout << "Parsing Object " << name << std::endl;

	tokenizer.nextToken();

	std::string token;
	while ((token = tokenizer.nextTokenLower()) != "}") {
		tokenizer.forgetActiveTokens();
		object.parseProperty(token, tokenizer);
	}

	tokenizer.forgetActiveTokens();
	std::cout << "Done parsing object" << std::endl;
}

void Parser::parseCamera(Camera& camera, std::string& name)
{
	std::cout << "Parsing camera " << name << std::endl;

	tokenizer.nextToken();

	std::string token;
	while ((token = tokenizer.nextTokenLower()) != "}") {
		tokenizer.forgetActiveTokens();
		camera.parseProperty(token, tokenizer);
	}

	tokenizer.forgetActiveTokens();
	std::cout << "Done parsing camera" << std::endl;
}

void Parser::parseLight(Light& light, std::string& name)
{
	std::cout << "Parsing light " << name << std::endl;

	tokenizer.nextToken();

	std::string token;
	while ((token = tokenizer.nextTokenLower()) != "}") {
		tokenizer.forgetActiveTokens();
		light.parseProperty(token, tokenizer);
	}

	tokenizer.forgetActiveTokens();
	std::cout << "Done parsing light" << std::endl;
}


// There is certainly a better way to do this.
// I am pretty ashamed of myself because I am too lazy to do it properly
#define PARSE_HACK(TYPE)											\
	int objectIndex = getNameIndex(frame.objectNames, name);		\
	std::shared_ptr<Object> object = std::make_shared<TYPE>();		\
	if (objectIndex == -1) {										\
		parseObject(*object, name);									\
		frame.objects.push_back(object);							\
		frame.objectNames.push_back(name);							\
	}																\
	else {															\
		*object = *frame.objects[objectIndex];						\
		parseObject(*object, name);									\
		frame.objects[objectIndex] = object;						\
	}

void Parser::parseFrame(Frame& frame)
{
	std::cout << "Parsing frame" << std::endl;

	frame.timeOffset = 0.0f;
	
	if (tokenizer.nextTokenLower() == "keyframe") {
		frame.timeOffset = std::stod(tokenizer.nextToken());
	}

	tokenizer.nextToken();
	tokenizer.forgetActiveTokens();

	std::string token;
	while ((token = tokenizer.nextTokenLower()) != "}") {
		tokenizer.forgetActiveTokens();

		std::string name;
		if (token != "background") {
			name = tokenizer.nextToken();
		}

		if (token == "background") {
			frame.background = {
				tokenizer.nextDouble(),
				tokenizer.nextDouble(),
				tokenizer.nextDouble(),
			};
		}
		else if (token == "sphere") {
			PARSE_HACK(Sphere);
		}
		else if (token == "plane") {
				PARSE_HACK(Plane);
		}
		else if (token == "triangle") {
			PARSE_HACK(Triangle);
		}
		else if (token == "light") {
			int lightIndex = getNameIndex(frame.lightNames, name);		
			std::shared_ptr<Light> light = std::make_shared<Light>();		
			if (lightIndex == -1) {										
				parseLight(*light, name);									
				frame.lights.push_back(light);							
				frame.lightNames.push_back(name);							
			}																
			else {															
				*light = *frame.lights[lightIndex];						
				parseLight(*light, name);									
				frame.lights[lightIndex] = light;						
			}
		}
		else if (token == "camera") {
			parseCamera(frame.camera, frame.cameraName);
		}
		else {
			std::cout << "Unknown object type of \"" << token << "\"" << std::endl;
			NullObject nullObject;
			parseObject(nullObject, name);
		}
	}

	tokenizer.forgetActiveTokens();
	std::cout << "Done Parsing Frame" << std::endl;
}

void Parser::doParse(Animation& animation)
{
	std::cout << "Beginning Parse" << std::endl;

	// This try loop is to wait for an EOF exception while parsing. 
	// There's probably a more semantically-correct way to do this, but this works
	try {
		while (tokenizer.hasNextToken()) {
			std::string& token = tokenizer.nextTokenLower();

			if (token == "rendersettings") {
				parseRenderSettings(animation);
			}
			else if (token == "keyframe") {
				if (animation.keyFrames.size() == 0)
					animation.keyFrames.push_back(Frame());
				else
					animation.keyFrames.push_back(animation.keyFrames.back());

				tokenizer.resetIndex();
				parseFrame(animation.keyFrames.back());
			}
			else {
				std::cerr << "Unkown Token!!!" << std::endl;
			}

			tokenizer.forgetActiveTokens();
		}
	}
	catch (std::istream::failure e) {
		// If the tokenizer still has information left, then we know
		// that the parser wasn't finished and that EOF came early
		if(tokenizer.hasNextToken())
			std::cerr << "Early EOF!" << std::endl;
		else
			std::cout << "Done Parsing." << std::endl;
	}

}