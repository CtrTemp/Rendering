#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record;
//当前文件在GlobalInclude/一级目录下
#include "../GlobalInclude/basic/vec3.h"
#include "../GlobalInclude/hitable.h"

#include "../GlobalInclude/random.h"
#include "../GlobalInclude/textures.h"





vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;//获得纯镜面反射光线（方向射线）
}

//折射光，返回是否可以发生折射（是否不发生全反射）
//并通过传引用，隐式的计算出折射光线
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);//单位入射光线
	float dt = dot(uv, n);//单位入射光线在法线（我们默认是从球心指向外部）上的投影就是入射角的cos值
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
	//全反射判断公式，可以自推，其中ni_over_nt是相对折射率，可知小于1时一定是不会发生全反射的
	//（因为是从折射率较小的物质射入折射率较大的物质一定不会发生全反射，这里或许可以优化加速）
	if (discriminant > 0)
	{
		refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
		return true;
	}
	else
		return false;
}
float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}


class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuated, ray& scattered) const = 0;
	virtual vec3 emitted(float u, float v, const vec3 &p) const { return vec3(0, 0, 0); }
};





/******************************** 哑光类/漫反射表面 *********************************/

class lambertian :public material {
public:
	lambertian(texture *a) :albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuated, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();//获得本次打击后得到的下一个目标点
		scattered = ray(rec.p, target - rec.p, r_in.time());//本次击中一个目标点后的下一个射线（获得散射光线）
		attenuated = albedo->value(0, 0, rec.p);
		return true;
	}

	//vec3 albedo;
	texture *albedo;
};

/******************************** 金属类/镜面 *********************************/
class mental :public material {
public:
	mental(const vec3& a, float f) : albedo(a) { if (f < 1)fuzz = f; else fuzz = 1; }

	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuated, ray& scattered) const {

		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);//获得反射方向向量
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());//获得散射方向向量
		//fuzz参数代表金属表面的“哑光”程度，即我们使用scattered散射参数来间接的代表reflect参数，
		//可见fuzz参数越大，scattered偏离reflect光线的程度，或者说不确定度就越大，也就表现出一种
		//光线“随机反射”的哑光、漫反射特性
		//注意这里scattered的计算方法与lambertian的不同，这里使用reflect为基，加上一个随机方向的向量得到散射方向
		//而之前lambertian的scattered的计算是使用normal为基进行计算
		//故在函数末端返回时，这个scattered与normal所成角度是有一定几率为钝角的，此时我们返回false
		attenuated = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
		//返回false说明散射光线与原法线成钝角，直观上理解：此时散射光线射入了object内部，即为光线被吸收
	}

	vec3 albedo;
	float fuzz;
};



/******************************** 玻璃类/透射表面 *********************************/
class dielectric :public material {
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3&attenuation, ray& scattered) const
	{
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;


		float reflect_prob;
		float cosine;


		if (dot(r_in.direction(), rec.normal) > 0)//如果是从晶体内部射向空气
		{
			outward_normal = -rec.normal;//我们将法线方向定义为由外部指向内部
			ni_over_nt = ref_idx;//ref_idx是大于1的相对折射率，ni_over_nt是入射端折射率除以出射端折射率
			cosine = ref_idx*dot(r_in.direction(), rec.normal / r_in.direction().length());
		}
		else//如果是从空气射向晶体球内部
		{
			outward_normal = rec.normal;//我们将法线方向定义为由内部指向外部
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal / r_in.direction().length());
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))//如果发生折射（无全反射）
		{
			reflect_prob = schlick(cosine, ref_idx);//应该是由菲涅尔公式近似计算出的反射光线强度
		}//其实是（转化成）反射光占总光线之比，在抗锯齿章节我们将一个像素点由多（100）条射线表示
		else
		{
			reflect_prob = 1.0;//如果全反射，则反射光占比为100%
		}

		if (drand48() < reflect_prob)
		{
			scattered = ray(rec.p, reflected);
		}//明白
		else
		{
			scattered = ray(rec.p, refracted);
		}//明白

		return true;

	}

	float ref_idx;
};

#endif


/*


class dielectric :public material {
public:
dielectric(float ri) : ref_idx(ri) {}
virtual bool scatter(const ray& r_in, const hit_record& rec, vec3&attenuation, ray& scattered) const
{
vec3 outward_normal;
vec3 reflected = reflect(r_in.direction(), rec.normal);
float ni_over_nt;
attenuation = vec3(1.0, 1.0, 0.0);
vec3 refracted;
if (dot(r_in.direction(), rec.normal) > 0)
{
outward_normal = -rec.normal;
ni_over_nt = ref_idx;
}
else
{
outward_normal = rec.normal;
ni_over_nt = 1.0 / ref_idx;
}
if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
{
scattered = ray(rec.p, refracted);
}
else
{
scattered = ray(rec.p, reflected);
return false;
}
return true;

}



float ref_idx;
};

*/