#include "includes/algebra.h"

Vec2f vec2f(float x, float y) {
	Vec2f res = {
		.x = x,
		.y = y
	};

	return res;
}

Vec2f vec2f_add(Vec2f a, Vec2f b) {
	Vec2f res =  {
		.x = a.x + b.x,
		.y = a.y + b.y,
	};

	return res;
}

Vec2f vec2f_sub(Vec2f a, Vec2f b) {
	Vec2f res =  {
		.x = a.x - b.x,
		.y = a.y - b.y,
	};

	return res;
}

Vec2f vec2f_mul(Vec2f a, Vec2f b) {
	Vec2f res =  {
		.x = a.x * b.x,
		.y = a.y * b.y,
	};

	return res;
}

Vec2f vec2f_div(Vec2f a, Vec2f b) {
	Vec2f res =  {
		.x = a.x / b.x,
		.y = a.y / b.y,
	};

	return res;
}
