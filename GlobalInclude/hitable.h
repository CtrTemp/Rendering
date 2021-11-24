#pragma once
#ifndef HITABLE_H
#define HITABLE_H
//当前文件在GlobalInclude/一级目录下
#include "../GlobalInclude/basic/ray.h"








inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb {
public:
	aabb() = default;
	aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

	vec3 min() const { return _min; }
	vec3 max() const { return _max; }

	bool hit(const ray& r, float tmin, float tmax) const;
	/*

	{
	for (int a = 0; a < 3; ++a)
	{
	float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a], (_max[a] - r.origin()[a]) / r.direction()[a]);
	float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a], (_max[a] - r.origin()[a]) / r.direction()[a]);
	tmin = ffmax(t0, tmin);
	tmax = ffmin(t1, tmax);
	if (tmax <= tmin)
	return false;
	}
	return true;
	}
	*/


	vec3 _min;
	vec3 _max;
};


//问题来了：如何通过调用区分这两个重载函数？！
inline bool aabb::hit(const ray& r, float tmin, float tmax) const {

	for (int a = 0; a < 3; ++a)
	{
		float invD = 1.0f / r.direction()[a];
		float t0 = (min()[a] - r.origin()[a])*invD;
		float t1 = (max()[a] - r.origin()[a])*invD;
		if (invD < 0.0f)
			std::swap(t0, t1);
		tmin = t0 > tmin ? t0 : tmin;
		tmin = t0 < tmax ? t0 : tmax;
		if (tmax <= tmin)
			return false;
	}

	return true;

}






class material;



//这个结构体用来记录那些与物体相交的射线的一些参数
struct hit_record {
	float t;//用于记录击中点时，此时射线方向向量的乘数
	vec3 p;//用于记录击中点坐标
	vec3 normal;//用于记录击中点处的法线（注意是单位向量）
	
	float u;
	float v;//最新引入，我还不清楚这个是干啥的

	material *mat_ptr;
};

class hitable {
public:
	hitable() = default;
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	//C++没见过这种表达啊
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};








#endif






