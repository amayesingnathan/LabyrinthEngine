#pragma once

class System
{
public:
	void init(class Scene* scene);
	virtual void update() {};
	virtual void clean() {};

protected:
	class Scene* mScene;
};	