#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

using namespace std;

class Config
{
public:

	static const string FORWARD_VELOCITY;
	static const string ANGULAR_VELOCITY;
	static const string THRUST;
	static const string MASS;
	static const string FREQUENCY;
	static const string FREQUENCY_INCREASE;
	static const string BULLET_VELOCITY;
	static const string BULLET_FREQUENCY;
	static const string EXPLOSION_DURATION;
	static const string RAPID_FIRE;
	static const string DIFFICULTY_INCREASE;

	Config();
	~Config();

	// loads config from file
	void Load(const char *file);

	float GetValue(string key);

private:

	void StoreKeyValue(string key, string value);

	map<string, float> m_config;
};

#endif