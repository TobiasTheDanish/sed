#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <stdbool.h>
#include <stddef.h>
typedef struct {
	float x;
	float y;
} Vec2f;

typedef struct {
	size_t x;
	size_t y;
} Vec2s;

Vec2f vec2f(float x, float y);
bool vec2f_cmp(Vec2f a, Vec2f b);
Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2f vec2f_mul(Vec2f a, Vec2f b);
Vec2f vec2f_div(Vec2f a, Vec2f b);

Vec2s vec2s(size_t x, size_t y);
bool vec2s_cmp(Vec2s a, Vec2s b);
Vec2s vec2s_add(Vec2s a, Vec2s b);
Vec2s vec2s_sub(Vec2s a, Vec2s b);
Vec2s vec2s_mul(Vec2s a, Vec2s b);
Vec2s vec2s_div(Vec2s a, Vec2s b);

#endif // !ALGEBRA_H
