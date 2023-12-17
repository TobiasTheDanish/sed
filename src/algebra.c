#include "includes/algebra.h"

Vec2f vec2f(float x, float y) {
	Vec2f res = {
		.x = x,
		.y = y
	};

	return res;
}

bool vec2f_cmp(Vec2f a, Vec2f b) {
	return (a.x == b.x && a.y == b.y);
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

Vec2s vec2s(size_t x, size_t y) {
	Vec2s res = {
		.x = x,
		.y = y
	};

	return res;
}

bool vec2s_cmp(Vec2s a, Vec2s b) {
	return (a.x == b.x && a.y == b.y);
}

Vec2s vec2s_add(Vec2s a, Vec2s b) {
	Vec2s res =  {
		.x = a.x + b.x,
		.y = a.y + b.y,
	};

	return res;
}

Vec2s vec2s_sub(Vec2s a, Vec2s b) {
	Vec2s res =  {
		.x = a.x - b.x,
		.y = a.y - b.y,
	};

	return res;
}

Vec2s vec2s_mul(Vec2s a, Vec2s b) {
	Vec2s res =  {
		.x = a.x * b.x,
		.y = a.y * b.y,
	};

	return res;
}

Vec2s vec2s_div(Vec2s a, Vec2s b) {
	Vec2s res =  {
		.x = a.x / b.x,
		.y = a.y / b.y,
	};

	return res;
}
