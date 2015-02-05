#ifndef _SOLAR_H
#define _SOLAR_H

#include "baseApp.h"

class SolarSystem : public Application
{

public:
	SolarSystem()
	{
		m_Num = 0;
	};
	
	bool startup()
	{
		m_Num = 1;
		return true;
	};
	
	bool update()
	{
		m_Num = 2;
		return true;
	}
	
	void draw() 
	{
		m_Num = 3;
	}
	
	void shutdown()
	{
		m_Num = -1;
	}

private:
	int m_Num;

};

#endif