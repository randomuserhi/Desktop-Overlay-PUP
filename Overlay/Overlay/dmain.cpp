#include <iostream>
#include "renderer.h"
//#include <Eigen/Dense>

#include "Deep.h"

#include "dmain.h"

inline float frand()
{
	return ((double)rand() / (RAND_MAX));
}

Circle buffer[Balls];
float breath = 500;

void Init()
{
	srand(time(NULL));

	for (int i = 0; i < Balls; ++i)
	{
		buffer[i].x = frand() * w;
		buffer[i].y = frand() * h;
		buffer[i].vx = (frand() * 2.0f - 1.0f) * frand() * 1000.0f + 100.0f;
		buffer[i].vy = (frand() * 2.0f - 1.0f) * frand() * 1000.0f + 100.0f;

		buffer[i].r = frand();
		buffer[i].g = frand();
		buffer[i].b = frand();
		buffer[i].a = frand() * 0.2f + 0.8f;

		buffer[i].radius = frand() * 50.0f + 10.0f;
	}
}

float loop = 0;
void Update(float dt)
{
	breath = 200 + 50 * sin(loop);
	loop += dt;

	for (int i = 0; i < Balls; ++i)
	{
		buffer[i].x += buffer[i].vx * dt;
		buffer[i].y += buffer[i].vy * dt;

		if (buffer[i].x < 0.0f)
		{
			buffer[i].x = 0;
			buffer[i].vx *= -1.0f;
		}
		else if (buffer[i].x > w)
		{
			buffer[i].x = w;
			buffer[i].vx *= -1.0f;
		}

		if (buffer[i].y < 0.0f)
		{
			buffer[i].y = 0;
			buffer[i].vy *= -1.0f;
		}
		else if (buffer[i].y > h)
		{
			buffer[i].y = h;
			buffer[i].vy *= -1.0f;
		}
	}
}