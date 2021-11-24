#pragma once
#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray
{
public:
	ray() = default;
	ray(const vec3&a, const vec3& b, float ti = 0.0) { A = a; B = b; _time = ti; }

	vec3 origin() const { return A; }//������ʼ��
	vec3 direction() const { return B; }//���߷���������Ӧ���ǵ�λ������
	float time() const { return _time; }
	vec3 point_at_parameter(float t) const { return A + t*B; }
	//�������߷�����������t���õ�����ĩ��ָ���

	vec3 A;
	vec3 B;
	float _time;
};


#endif