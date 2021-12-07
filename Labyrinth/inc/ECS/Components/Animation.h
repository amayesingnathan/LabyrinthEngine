#pragma once

struct Animation
{

	int frames;
	int speed;

	Animation() : frames(1), speed(0) {}

	Animation(int f, int s) : frames(f), speed(s) {}
};