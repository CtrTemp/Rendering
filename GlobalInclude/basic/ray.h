#pragma once
#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray
{
public:
	ray() = default;
	ray(const vec3&a, const vec3& b, float ti = 0.0) { A = a; B = b; _time = ti; }

	vec3 origin() const { return A; }//射线起始点
	vec3 direction() const { return B; }//射线方向向量（应该是单位向量）
	float time() const { return _time; }
	vec3 point_at_parameter(float t) const { return A + t*B; }
	//给定射线方向向量倍数t，得到射线末端指向点

	vec3 A;
	vec3 B;
	float _time;
};


#endif