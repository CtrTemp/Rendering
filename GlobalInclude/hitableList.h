#pragma once
#ifndef HITABLELIST_H
#define HITABLELIST_H

#include "hitable.h"
#include <memory>
#include <vector>




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



class hitable_list :public hitable {//是hitable_list类，构造函数返回hitable*类型？
public:
	hitable_list() = default;
	hitable_list(hitable **l, int n) { list = l; list_size = n; }

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;

	hitable **list;
	int list_size;
};

bool hitable_list::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = tmax;
	for (int i = 0; i < list_size; ++i)
	{
		if (list[i]->hit(r, tmin, closest_so_far, temp_rec))//这里是比较有意思的地方
		//在列表中遍历，tmax参数设为上一个list元素中的t值，并与本元素进行比较，
		//可见，只有当本元素中较小的t（也就是更近的击中点）才会被记录更新到temp_rec
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}


bool hitable_list::bounding_box(float t0, float t1, aabb &box) const {
	if (list_size < 1)
		return false;
	aabb temp_box;
	bool first_true = list[0]->bounding_box(t0, t1, temp_box);
	if (!first_true)
		return false;
	else
		box = temp_box;

	for (int i = 1; i < list_size; ++i)
	{
		if (list[0]->bounding_box(t0, t1, temp_box))
			box = surronding_box(box, temp_box);
		else
			return false;
	}
	return true;

}


class bvh_node :public hitable {
public:
	bvh_node() = default;
	bvh_node(hitable **l, int n, float time0, float time1);

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;


	hitable *left;
	hitable *right;
	aabb box;
};


bool bvh_node::bounding_box(float t0, float t1, aabb& b)const {
	b = box;
	return true;
}



int box_x_compare(const void *a, const void *b)
{
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().x() - box_right.min().x() < 0.0)
		return -1;
	else
		return 1;
}
int box_y_compare(const void *a, const void *b)
{
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().y() - box_right.min().y() < 0.0)
		return -1;
	else
		return 1;
}
int box_z_compare(const void *a, const void *b)
{
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	if (box_left.min().z() - box_right.min().z() < 0.0)
		return -1;
	else
		return 1;
}


bvh_node::bvh_node(hitable **l, int n, float time0, float time1) {
	int axis = int(3 * drand48());
	if (axis == 0)
		qsort(l, n, sizeof(hitable *), box_x_compare);
	else if (axis == 1)
		qsort(l, n, sizeof(hitable *), box_y_compare);
	else if (axis == 2)
		qsort(l, n, sizeof(hitable *), box_z_compare);

	if (n == 1)
	{
		left = right = l[0];
	}
	else if (n == 2)
	{
		left = l[0];
		right = l[1];
	}
	else
	{
		left = new bvh_node(l, n / 2, time0, time1);
		right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	box = surronding_box(box_left, box_right);

}













bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	if (box.hit(r, t_min, t_max))
	{
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, t_min, t_max, left_rec);
		bool hit_right = right->hit(r, t_min, t_max, right_rec);

		if (hit_left&&hit_right)
		{
			if (left_rec.t < right_rec.t)
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left)
		{
			rec = left_rec;
			return true;
		}
		else if (hit_right)
		{
			rec = right_rec;
			return true;
		}

		else
			return false;

	}
	else return false;
}





#endif
