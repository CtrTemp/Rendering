#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record;
//��ǰ�ļ���GlobalInclude/һ��Ŀ¼��
#include "../GlobalInclude/basic/vec3.h"
#include "../GlobalInclude/hitable.h"

#include "../GlobalInclude/random.h"
#include "../GlobalInclude/textures.h"





vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;//��ô����淴����ߣ��������ߣ�
}

//����⣬�����Ƿ���Է������䣨�Ƿ񲻷���ȫ���䣩
//��ͨ�������ã���ʽ�ļ�����������
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);//��λ�������
	float dt = dot(uv, n);//��λ��������ڷ��ߣ�����Ĭ���Ǵ�����ָ���ⲿ���ϵ�ͶӰ��������ǵ�cosֵ
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
	//ȫ�����жϹ�ʽ���������ƣ�����ni_over_nt����������ʣ���֪С��1ʱһ���ǲ��ᷢ��ȫ�����
	//����Ϊ�Ǵ������ʽ�С���������������ʽϴ������һ�����ᷢ��ȫ���䣬�����������Ż����٣�
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





/******************************** �ƹ���/��������� *********************************/

class lambertian :public material {
public:
	lambertian(texture *a) :albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuated, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();//��ñ��δ����õ�����һ��Ŀ���
		scattered = ray(rec.p, target - rec.p, r_in.time());//���λ���һ��Ŀ�������һ�����ߣ����ɢ����ߣ�
		attenuated = albedo->value(0, 0, rec.p);
		return true;
	}

	//vec3 albedo;
	texture *albedo;
};

/******************************** ������/���� *********************************/
class mental :public material {
public:
	mental(const vec3& a, float f) : albedo(a) { if (f < 1)fuzz = f; else fuzz = 1; }

	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuated, ray& scattered) const {

		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);//��÷��䷽������
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());//���ɢ�䷽������
		//fuzz���������������ġ��ƹ⡱�̶ȣ�������ʹ��scatteredɢ���������ӵĴ���reflect������
		//�ɼ�fuzz����Խ��scatteredƫ��reflect���ߵĳ̶ȣ�����˵��ȷ���Ⱦ�Խ��Ҳ�ͱ��ֳ�һ��
		//���ߡ�������䡱���ƹ⡢����������
		//ע������scattered�ļ��㷽����lambertian�Ĳ�ͬ������ʹ��reflectΪ��������һ���������������õ�ɢ�䷽��
		//��֮ǰlambertian��scattered�ļ�����ʹ��normalΪ�����м���
		//���ں���ĩ�˷���ʱ�����scattered��normal���ɽǶ�����һ������Ϊ�۽ǵģ���ʱ���Ƿ���false
		attenuated = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
		//����false˵��ɢ�������ԭ���߳ɶ۽ǣ�ֱ������⣺��ʱɢ�����������object�ڲ�����Ϊ���߱�����
	}

	vec3 albedo;
	float fuzz;
};



/******************************** ������/͸����� *********************************/
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


		if (dot(r_in.direction(), rec.normal) > 0)//����ǴӾ����ڲ��������
		{
			outward_normal = -rec.normal;//���ǽ����߷�����Ϊ���ⲿָ���ڲ�
			ni_over_nt = ref_idx;//ref_idx�Ǵ���1����������ʣ�ni_over_nt������������ʳ��Գ����������
			cosine = ref_idx*dot(r_in.direction(), rec.normal / r_in.direction().length());
		}
		else//����Ǵӿ������������ڲ�
		{
			outward_normal = rec.normal;//���ǽ����߷�����Ϊ���ڲ�ָ���ⲿ
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal / r_in.direction().length());
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))//����������䣨��ȫ���䣩
		{
			reflect_prob = schlick(cosine, ref_idx);//Ӧ�����ɷ�������ʽ���Ƽ�����ķ������ǿ��
		}//��ʵ�ǣ�ת���ɣ������ռ�ܹ���֮�ȣ��ڿ�����½����ǽ�һ�����ص��ɶࣨ100�������߱�ʾ
		else
		{
			reflect_prob = 1.0;//���ȫ���䣬�����ռ��Ϊ100%
		}

		if (drand48() < reflect_prob)
		{
			scattered = ray(rec.p, reflected);
		}//����
		else
		{
			scattered = ray(rec.p, refracted);
		}//����

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