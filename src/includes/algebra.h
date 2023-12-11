#ifndef ALGEBRA_H
#define ALGEBRA_H

typedef struct {
	float x;
	float y;
} Vec2f;

Vec2f vec2f(float x, float y);
Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2f vec2f_mul(Vec2f a, Vec2f b);
Vec2f vec2f_div(Vec2f a, Vec2f b);

#endif // !ALGEBRA_H
