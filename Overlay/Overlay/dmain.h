#pragma once

struct Circle
{
	float x;
	float y;
	float vx;
	float vy;
	float r;
	float g;
	float b;
	float a;
	float radius;
	float speed;
};

#define Balls 20
extern Circle buffer[];

extern float breath;

void Init();
void Update(float dt);