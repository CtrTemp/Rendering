#pragma once
#include <math.h>
#include <stdlib.h>
#include <iostream>

/*
	一个比较显而易见的问题就是：我们赋予了这个三维向量太多的含义：
我们可以用它来表示一个向量；
我们可以用它来表示空间中的一个点的坐标；
还可以用它老表示颜色值的取值

这样难免会造成在定义时理解上的混淆！

这是你之后要特别注意的一个地方，也是之后对代码进行你个人优化的一个首要方向
*/

class vec3 {

public:
	//vec3() {}//大括号在这里就相当于一个分号，哈哈这里就是默认构造函数
	vec3() = default;
	vec3(float e0, float e1, float e2)
	{
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
	}

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	inline const vec3& operator+() const { return *this; }
	inline const vec3  operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	inline float  operator[](int i) const { return e[i]; }
	inline float& operator[](int i) { return e[i]; }//为什么要定义两个这个？！和上面那个有什么区分

	inline vec3& operator+=(const vec3 &v);
	inline vec3& operator-=(const vec3 &v);
	inline vec3& operator*=(const vec3 &v);
	inline vec3& operator/=(const vec3 &v);
	inline vec3& operator*=(const float t);
	inline vec3& operator/=(const float t);

	inline float length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}

	inline float squared_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline void make_unit_vector();

	float e[3];//这个类只有这一个数据值
	//这个类的其他部分都是关于这个数据的构造以及操作
};




inline std::istream& operator >> (std::istream &is, vec3 &t)
{
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator << (std::ostream &os, const vec3 &t)
{
	os << t.e[0] << t.e[1] << t.e[2];
	return os;
}

inline void vec3::make_unit_vector()
{
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}


inline vec3 operator*(float t, const vec3 &v)
{
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, float t)
{
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator/(const vec3 &v, float t)
{
	return vec3(v.e[0]/t, v.e[1]/t, v.e[2]/t);
}


inline float dot(const vec3 &v1, const vec3 &v2)
{
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2)
{
	return vec3(
		 (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
		 (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0])
	);
}



inline vec3& vec3::operator+=(const vec3 &v)
{
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

inline vec3& vec3::operator-=(const vec3 &v)
{
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(const vec3 &v)
{
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}

inline vec3& vec3::operator/=(const vec3 &v)
{
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(float t)
{
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline vec3& vec3::operator/=(float t)
{
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
	return *this;
}


inline vec3 unit_vector(vec3 v)
{
	return v / v.length();
}


/*
	色值归一化：
	取大于255的最大的通道色值，将其除以zone作为比例因子
	将三个通道的通道值按比例因子缩小
	得到三个均小于zone的通道值


*/
inline vec3 color_unit_normalization(vec3 v, float zone)
{
	float para = 0;//比例因子
	vec3 return_vec;
	if (v[0] > zone)
		para = zone / v[0];//得到一个小于1的值
	else if (v[1] > zone)
		para = zone / v[1];
	else if (v[2] > zone)
		para = zone / v[2];

	else
		return v;

	for (int i = 0; i < 3; ++i)
		return_vec[i] = v[i] * para;

	return return_vec;
}
