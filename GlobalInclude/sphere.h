#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.h"

class sphere : public hitable {//是sphere类，构造函数返回hitable*类型
public:
	sphere() = default;
	sphere(vec3 cen, float r, material* mat) :center(cen), radius(r), mat_ptr(mat){};//创造一个球体，包括球心和半径参数
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	//判断参数：给定一条射线
	virtual bool bounding_box(float t0, float t1, aabb& box) const;


	vec3 center;
	float radius;
	material *mat_ptr;
};


bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4 * a*c;
	//这里是不是写错了？不应该是b*b-4ac么
	//不错，已经进行了修改，就应该是b*b - 4*a*c



	//以下有一个优先返回原则：优先返回双解中离观察点（射线发射点）最近的击中点
	//注意！我们是传引用，在函数中就直接可以改变 rec结构体变量 的各类值
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / a / 2;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;//得到击中点的 t值 并储存入record
			rec.p = r.point_at_parameter(rec.t);//得到击中点的坐标 并储存人record
			rec.normal = (rec.p - center) / radius;//得到击中点的单位法向向量
			rec.mat_ptr = this->mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a / 2;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = this->mat_ptr;
			return true;
		}
	}
	return false;
}


bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}





class moving_sphere :public hitable {
public:
	moving_sphere() = default;
	moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material* mat) :
		center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(mat) {};

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const { return false; }


	vec3 center(float time) const;
	
	vec3 center0, center1;
	float time0, time1;
	float radius;
	material *mat_ptr;

};


vec3 moving_sphere::center(float time)const {
	return center0 + ((time - time0) / (time1 - time0)*(center1 - center0));
}


bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - 4 * a*c;




	//以下有一个优先返回原则：优先返回双解中离观察点（射线发射点）最近的击中点
	//注意！我们是传引用，在函数中就直接可以改变 rec结构体变量 的各类值
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / a / 2;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;//得到击中点的 t值 并储存入record
			rec.p = r.point_at_parameter(rec.t);//得到击中点的坐标 并储存人record
			rec.normal = (rec.p - center(r.time())) / radius;//得到击中点的单位法向向量
			rec.mat_ptr = this->mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a / 2;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = this->mat_ptr;
			return true;
		}
	}
	return false;
}

/*


aabb surronding_box(aabb box0, aabb box1)
{
vec3 small(
fmin(box0.min().x(), box1.min().x()),
fmin(box0.min().y(), box1.min().y()),
fmin(box0.min().z(), box1.min().z())
);

vec3 big(
fmax(box0.max().x(), box1.max().x()),
fmax(box0.max().y(), box1.max().y()),
fmax(box0.max().z(), box1.max().z())
);
return aabb(small, big);
}


*/

#endif