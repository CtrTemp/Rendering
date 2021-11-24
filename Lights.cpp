//#include "GlobalInclude/ray.h"
//#include "GlobalInclude/vec3.h"
//当前.cpp文件应该和GlobalInclude文件夹在同一个目录下
#include "GlobalInclude/camera.h"
#include "GlobalInclude/hitable.h"
#include "GlobalInclude/hitableList.h"
#include "GlobalInclude/material.h"
#include "GlobalInclude/textures.h"
#include "GlobalInclude/random.h"
#include "GlobalInclude/sphere.h"

#include <string>

#include <iostream>
#include <fstream>
#include <random>
#include <sys/time.h>  


#include "GlobalInclude/Chapter/diffuse_light.h"
#include "GlobalInclude/Chapter/Perlin.h"

#define TOTAL_IMG	1

//关于文件包含关系的总结：
/*
	对于当前文件所在的目录，可以直接include当期目录下的所有头文件，无论当前文件是头文件还是源文件
对于当前文件下属目录下的需要加目录名再引用下属目录下的头文件；如果想包含当前目录上级目录中的文件，则需要、
使用../来引导到上级目录进行引用。

	另外，所有引用文件均可使用绝对路径来指定位置。

	默认情况下，c_cpp_properties.json文件会将根目录引导到当前工程一级目录下。

	之后吃完饭整理工程，实验上述是否成立！

*/





vec3 color(const ray& r, hitable *world, int depth)
{

	hit_record rec;

	if (world->hit(r, 0.001, 999999, rec))//FLT_MAX
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return emitted + attenuation*color(scattered, world, depth + 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		return vec3(0, 0, 0);
	}

}


hitable *sample_light() {
	texture *pertext = new noise_texture(5);
	hitable **list = new hitable *[5];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));//Ground

	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));//漫反射表面
	list[2] = new sphere(vec3(6, 2, 0), 2, new dielectric(1.5));//透明表面
	list[3] = new sphere(vec3(0, 2, 6), 2, new mental(vec3(0.8, 0.8, 0.8), 0.5*drand48()));//金属镜面

	list[4] = new sphere(vec3(0, 10, 0), 2, new diffuse_light(new constant_texture(vec3(10, 10, 10))));//吊灯光源
	//list[5] = new xy_rect(3, 5, 1, 3, 3, new diffuse_light(new constant_texture(vec3(20, 0, 0))));
	//list[5] = new sphere(vec3(0, 7, 0), 2.2, new dielectric(1.5));

	

	return new hitable_list(list, 5);
}



hitable *cornell_box() {
	hitable **list = new hitable*[6];
	int i = 0;

	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

	
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);

	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	return new hitable_list(list, i);

}




vec3 lookfrom_gen(int index, float total)
{

	//vec3 look_temp(-20,15,20);
	//if(index<10)
	//	return look_temp-vec3(0,0,index);
	//if(index<20)
	//	return look_temp+vec3(0,index-10,-10);
	//else
	//	return look_temp+vec3(index-20,10,-10);
	

	return vec3 (20*cos(2*M_PI*(float)index/total), 15+5*sin(4*M_PI*(float)index/total), 20*sin(2*M_PI*(float)index/total));

} 





int main(void)
{
	int nx = 200;//图像高度
	int ny = 100;//图像宽度
	int ns = 100;//单一像素点投射射线数

	int unit_percent = ny/100;


	struct timeval timeStart, timeEnd, timeSystemStart; 
    double runTime=0, systemRunTime; 
    gettimeofday(&timeSystemStart, NULL );
	//Linux下计时器

	hitable *world = sample_light();


	//vec3 lookfrom(278, 278, -800);
	//vec3 lookat(278, 278, 0);
	


	for(int img_index=0; img_index<TOTAL_IMG; img_index++)
	{

		vec3 lookfrom = lookfrom_gen(img_index, 60);
			
			
		vec3 lookat(2, 2.5, -3);
		float dist_to_focus = 10.0;
		float aperture = 0.0;//MotionBlur
		float vfov = 40.0;
		camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);


		std::ofstream OutputImage;
		std::string Path = "./Pic/ctrTestt"+std::to_string(img_index)+".ppm";
		OutputImage.open(Path);
		OutputImage << "P3\n" << nx << " " << ny << "\n255\n";


		std::cout << "iNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN" << std::endl;
		
		
		gettimeofday(&timeStart, NULL );
		//开始计时
		


		for (int j = ny - 1; j >= 0; --j)
		{
			for (int i = 0; i < nx; ++i)
			{
				vec3 col(0, 0, 0);
				for (int s = 0; s < ns; ++s)
				{


					float u = float(i + rand() % 101 / float(101)) / float(nx);
					float v = float(j + rand() % 101 / float(101)) / float(ny);


					ray r = cam.get_ray(u, v);

					vec3 p = r.point_at_parameter(2.0);

					col += color(r, world, 0);

				}


				col /= float(ns);

				col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

				col = color_unit_normalization(col, 1);//色值

				int ir = int(255.99*col[0]);
				int ig = int(255.99*col[1]);
				int ib = int(255.99*col[2]);

				
				

				OutputImage << ir << " " << ig << " " << ib << "\n";


			}
			//std::cout<<j%unit_percent<<std::endl;
			/*
			if(((ny-j)%unit_percent)==0)
			{
				int percentage = (ny-j)/unit_percent;
				gettimeofday( &timeEnd, NULL ); 
				//停止计时
				runTime = (timeEnd.tv_sec - timeStart.tv_sec ) + (double)(timeEnd.tv_usec -timeStart.tv_usec)/1000000; 
				
			
				std::cout<<"Image Processing"<<percentage<<"% : "<< runTime << "second cost from now on" << std::endl;
			}
			*/
		}

		gettimeofday( &timeEnd, NULL ); 
		//停止计时
		runTime = (timeEnd.tv_sec - timeStart.tv_sec ) + (double)(timeEnd.tv_usec -timeStart.tv_usec)/1000000; 
		std::cout<<"Number"<<img_index<<": The total time of the pirmary loop is: " << runTime << "s" << std::endl;

	}

	std::cout<<"ALL DONE"<<std::endl;
	return 0;
}




