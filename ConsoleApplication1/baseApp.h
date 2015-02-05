#ifndef _APPLICATION_H
#define _APPLICATION_H

class Application
{
public:

	virtual bool startup() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void shutdown() = 0;
};

#endif