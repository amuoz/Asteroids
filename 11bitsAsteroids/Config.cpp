#include "Config.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

const string Config::FORWARD_VELOCITY = "FORWARD_VELOCITY";
const string Config::ANGULAR_VELOCITY = "ANGULAR_VELOCITY";
const string Config::THRUST = "THRUST";
const string Config::MASS = "MASS";
const string Config::FREQUENCY = "FREQUENCY";
const string Config::FREQUENCY_INCREASE = "FREQUENCY_INCREASE";
const string Config::BULLET_VELOCITY = "BULLET_VELOCITY";
const string Config::BULLET_FREQUENCY = "BULLET_FREQUENCY";
const string Config::EXPLOSION_DURATION = "EXPLOSION_DURATION";

Config::Config()
{

}

Config::~Config()
{

}

void Config::Load(const char * file)
{
	m_config.clear();

	// load from file
	unsigned int tileCode;
	std::string line;
	std::ifstream fstream(file);
	if (fstream)
	{
		// read each line from config file
		while (std::getline(fstream, line)) 
		{
			std::istringstream sstream(line);
			std::string key;
			if (std::getline(sstream, key, '='))
			{
				std::string value;
				if (std::getline(sstream, value))
				{
					StoreKeyValue(key, value);
				}
			}
		}
	}
}

float Config::GetValue(string key)
{
	return m_config.find(key)->second;
}

void Config::StoreKeyValue(string key, string value)
{
	std::string::size_type sz;     // alias of size_t
	float floatValue = std::stof(value, &sz);
	m_config.insert(std::pair<string, float>(key, floatValue));
}
