#pragma once
#ifndef BOX_H
#define BOX_H
//当前文件在GlobalInclude/Chapter/二级目录下
#include "../../GlobalInclude/hitable.h"
#include "../../GlobalInclude/hitableList.h"
#include "../../GlobalInclude/Chapter/diffuse_light.h"

class box :public hitable {
public:
	box() = default;
	box(const vec3 &p0, const vec3 &pl, material *ptr);

	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb &box)const;

	vec3 pmin, pmax;
	hitable *list_ptr;
};

box::box(const vec3 &p0, const vec3 &p1, material *ptr) {
	pmin = p0;
	pmax = p1;
	hitable **list = new hitable*[6];
	
	list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	
	list_ptr = new hitable_list(list, 6);
}


bool box::hit(const ray &r, float t_min, float t_max, hit_record &rec)const {
	return list_ptr->hit(r, t_min, t_max, rec);
}

bool box::bounding_box(float t0, float t1, aabb &box)const {
	box = aabb(pmin, pmax);
	return true;
}



class translate : public hitable {
public:
	translate() = default;
	translate(hitable *p, const vec3 &displacement) :ptr(p), offset(displacement) {}

	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb &box)const;

	hitable *ptr;
	vec3 offset;
};


bool translate::hit(const ray &r, float t_min, float t_max, hit_record &rec)const {
	ray move_r(r.origin() - offset, r.direction(), r.time());
	if (ptr->hit(move_r, t_min, t_max, rec))
	{
		rec.p += offset;
		return true;
	}
	else
		return false;
}

bool translate::bounding_box(float t0, float t1, aabb &box)const {
	if (ptr->bounding_box(t0, t1, box))
	{
		box = aabb(box.min() + offset, box.max() + offset);
		return true;
	}
	else
		return false;
}




class rotate_y : public hitable {
public:
	rotate_y() = default;
	rotate_y(hitable *p, float angle);

	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec)const;
	virtual bool bounding_box(float t0, float t1, aabb &box)const;

	hitable *ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	aabb bbox;

};


rotate_y::rotate_y(hitable *p, float angle) : ptr(p) {
	
	float radians = (M_PI / 180.)*angle;
	sin_theta = sin(radians);
	cos_theta = cos(radians);

	hasbox = ptr->bounding_box(0, 1, bbox);
	vec3 min(999999, 999999, 999999);
	vec3 max(-999999, -999999, -999999);

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				float x = i*bbox.max().x() + (1 - i)*bbox.min().x();
				float y = j*bbox.max().y() + (1 - j)*bbox.min().y();
				float z = k*bbox.max().z() + (1 - k)*bbox.min().z();

				float newx = cos_theta*x + sin_theta*z;
				float newz = -sin_theta*x + cos_theta*z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; ++c) {
					if (tester[c] > max[c])
						max[c] = tester[c];
					if (tester[c] < min[c])
						min[c] = tester[c];
				}
			}
		}
	}
	bbox = aabb(min, max);
}


bool rotate_y::hit(const ray &r, float t_min, float t_max, hit_record &rec)const {
	vec3 origin = r.origin();
	vec3 direction = r.direction();
	
	origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
	origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

	direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
	direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
	
	ray rotated_r(origin, direction, r.time());

	if (ptr->hit(rotated_r, t_min, t_max, rec))
	{
		vec3 p = rec.p;
		vec3 normal = rec.normal;

		p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
		p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

		normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
		normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

		rec.p = p;
		rec.normal = normal;
		return true;
	}
	else
		return false;

}


bool rotate_y::bounding_box(float t0, float t1, aabb &box)const {
	box = bbox;
	return hasbox;
}


#endif // !BOX_H
