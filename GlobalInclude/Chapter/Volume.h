#pragma once
#ifndef __VOLUME_H
#define __VOLUME_H
//当前文件在GlobalInclude/Chapter/二级目录下
#include "../../GlobalInclude/hitable.h"
#include "../../GlobalInclude/hitableList.h"
#include "../../GlobalInclude/Chapter/diffuse_light.h"

class isotropic : public material {
public:
	isotropic() = default;
	isotropic(texture *a) :albedo(a) {}

	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attunuation, ray &scatter) const;

	texture *albedo;
};

bool isotropic::scatter(const ray &r_in, const hit_record &rec, vec3 &attunuation, ray &scattered) const {
	scattered = ray(rec.p, random_in_unit_sphere());
	attunuation = albedo->value(rec.u, rec.v, rec.p);
	return true;
}




class constant_medium :public hitable {
public:
	constant_medium() = default;
	constant_medium(hitable *b, float d, texture *a):boundary(b),density(d)
	{
		phase_function = new isotropic(a);
	}

	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb &box)const;


	hitable *boundary;
	float density;
	material *phase_function;
};


bool constant_medium::hit(const ray &r, float t_min, float t_max, hit_record &rec)const {
	bool db = (drand48() < 0.00001);
	db = false;//质疑这里
	hit_record rec1, rec2;
	if (boundary->hit(r, -999999, 999999, rec1))
	{
		if (boundary->hit(r, rec1.t + 0.0001, 999999, rec2))
		{
			if (db)
				std::cerr << "\n0 t1" << rec1.t << " " << rec2.t << std::endl;
			if (rec1.t < t_min)
				rec1.t = t_min;
			if (rec2.t > t_max)
				rec2.t = t_max;
			if (rec1.t >= rec2.t)
				return false;
			if (rec1.t < 0)
				rec1.t = 0;

			float distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
			float hit_distance = -(1 / density)*log(drand48());
			if (hit_distance < distance_inside_boundary)
			{
				if (db)
					std::cerr << "hit_distance = " << hit_distance << std::endl;

				rec.t = rec1.t + hit_distance / r.direction().length();

				if (db)
					std::cerr << "rec.t = " << rec.t << std::endl;

				rec.p = r.point_at_parameter(rec.t);

				if (db)
					std::cerr << "rec.p = " << rec.p << std::endl;

				rec.normal = vec3(1, 0, 0);
				rec.mat_ptr = phase_function;
				
				return true;
			}
			
		}
	}
	return false;
}

bool constant_medium::bounding_box(float t0, float t1, aabb &box)const {
	return boundary->bounding_box(t0, t1, box);
}



#endif









