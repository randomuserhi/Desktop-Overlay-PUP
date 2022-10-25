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
		buffer[i].speed = sqrtf(buffer[i].vx * buffer[i].vx + buffer[i].vy + buffer[i].vy);

		buffer[i].r = frand();
		buffer[i].g = frand();
		buffer[i].b = frand();
		buffer[i].a = frand() * 0.2f + 0.8f;

		buffer[i].radius = frand() * 150.0f + 30.0f;
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

		// basic collision detection
		for (int j = 0; j < Balls; ++j)
		{
			if (i == j) continue;

			float deltaX = buffer[i].x - buffer[j].x;
			float deltaY = buffer[i].y - buffer[j].y;
			float sqrDist = deltaX * deltaX + deltaY * deltaY;
			float rad = buffer[i].radius + buffer[j].radius;
			if (sqrDist < rad * rad)
			{
				float dist = sqrtf(sqrDist);
				float overlap = 0.5f * (dist - rad);
				float displacementX = overlap * deltaX / dist;
				float displacementY = overlap * deltaY / dist;
			
				float temp = sqrtf(displacementX * displacementX + displacementY * displacementY);
				float normalX = displacementX / temp;
				float normalY = displacementY / temp;

				float kx = buffer[i].vx - buffer[j].vx;
				float ky = buffer[i].vx - buffer[j].vy;

				float p = 2.0f * (normalX * kx + normalY * ky) / (buffer[i].radius + buffer[j].radius);

				buffer[i].x -= displacementX;
				buffer[i].y -= displacementY;
				buffer[j].x += displacementX;
				buffer[j].y += displacementY;

				buffer[i].vx -= p * buffer[j].radius * normalX;
				buffer[i].vy -= p * buffer[j].radius * normalY;
				temp = sqrtf(buffer[i].vx * buffer[i].vx + buffer[i].vy * buffer[i].vy);
				buffer[i].vx = buffer[i].vx / temp * buffer[i].speed;
				buffer[i].vy = buffer[i].vy / temp * buffer[i].speed;

				buffer[j].vx += p * buffer[i].radius * normalX;
				buffer[j].vy += p * buffer[i].radius * normalY;
				temp = sqrtf(buffer[j].vx * buffer[j].vx + buffer[j].vy * buffer[j].vy);
				buffer[j].vx = buffer[j].vx / temp * buffer[j].speed;
				buffer[j].vy = buffer[j].vy / temp * buffer[j].speed;
			}
		}

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