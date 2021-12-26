#pragma once

class System
{
public:
	void init(class Scene* scene);
	virtual void update() {};

protected:
	class Scene* mScene;
};	