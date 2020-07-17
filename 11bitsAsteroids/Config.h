#ifndef CONFIG_H
#define CONFIG_H

class Config
{
public:

	Config();
	~Config();

	// configuration
	float m_forwardVelocity;
	float m_angularVelocity;
	float m_thrust;
	float m_mass;
	float m_freq;
	float m_freqIncrease;
};

#endif