#ifndef _REND_
#define _REND_

#include<cmath>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<fstream>
#include<iostream>
#include<sys/ioctl.h>
#include<stdio.h>
#include<unistd.h>
#include<random>
#include<chrono>

#include"../GMath/GMath.h"
#include "../lodepng/lodepng.h"

using namespace std;

#define soft 1.0
#define term_filt 0.01
#define sun_appr 0.999
#define min_smp 1
#define s_b_p 3
#define t_min 1e-7
#define div 1e-5

void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height);
void decodeOneStep(const char *filename, std::vector<unsigned char> &image);

//structures
struct ray;
struct triangle;

//basic entity
class entity;

//type of entities
class camera;
class light;
class object;

//lights
class l_point;
class sun;

//objects
class sphere;
class plane;
//	class mesh;

//scene
class scene;


struct ray{
	vettore o;
	vettore d;
	ray(vettore _o=vettore(0,0,0), vettore _d=vettore(1,1,1));
	vettore point(double t);
};


struct triangle{
	vettore a, b, c;
	vettore n;
//	bool parallel();
//	bool do_intersect();
	double intersect(ray r);
};


class entity{
protected:
	vettore pos;
	vettore dx, dy, dz;
public:
	entity(vettore _pos = vettore(0, 0, 0), vettore _dx=vettore(1,0,0), vettore _dy=vettore(0,1,0), vettore _dz=vettore(0,0,1));
	
	virtual void move(vettore m);
	virtual void move_to(vettore m);
	virtual void rotate_abs(vettore r);
	virtual void rotate_to(vettore _dx, vettore _dy, vettore _dz);
	virtual void point_to(vettore p, vettore up=vettore(0,1,0));

	vettore get_pos();
	vettore get_dx();
	vettore get_dy();
	vettore get_dz();
};

class camera : public entity{
protected:
	double lx, ly, df;
public:
	camera(double _lx=8, double _ly=5, double _df=100, entity e=entity(vettore(0,0,-100), vettore(1,0,0), vettore(0,1,0), vettore(0,0,1)));

	double width();
	double height();
	double focal();

	ray cast(double px, double py);
};

class light : public entity
{
protected:
	vettore color;
	double I;

public:
	light(vettore _color=vettore(255,255,255), double _I=100, entity e=entity());
	
	virtual ray cast(vettore p)=0;
	virtual double intersect(ray r)=0;
	virtual vettore shade(vettore p, vettore n)=0;

	double get_I();
	vettore get_color();
};

class l_point : public light
{
protected:
	double R;
public:
	l_point(double _R=1, vettore _color=vettore(255,255,255), double _I=100, entity e=entity());

	ray cast(vettore p);
	double intersect(ray r);
	vettore shade(vettore p, vettore n);
};

class sun : public light
{
protected:
	vettore angle;
public:
	sun(vettore _angle=vettore(-1,-1,-1), vettore _color=vettore(255,255,255), double _I=100, entity e=entity());

	ray cast(vettore p);
	double intersect(ray r);
	vettore shade(vettore p, vettore n);
};

class object : public entity
{
protected:
	double refl, opac, emit;
	vettore color;
	double n;
public:
	object(vettore _color=vettore(255,255,255), double _refl=0, double _opac=0, double _emit=0, entity e=entity());

	virtual double intersect(ray r)=0;
	virtual vettore normal(ray r)=0;
	
	ray reflect(ray r);
	ray trapass(ray r, double n2);
	ray cast(ray r, std::mt19937_64& eng);

	double get_refl();
	double get_opac();
	double get_emit();
	vettore get_color();
	double get_n();
};

class sphere : public object{
protected:
	double R;
public:
	sphere(double _R=1, vettore _color=vettore(255,255,255), double _refl=0, double _opac=0, double _emit=0, entity e=entity());

	double intersect(ray r);
	vettore normal(ray r);
};
class plane : public object
{
protected:
	vettore N;

public:
	plane(vettore _N=vettore(0,0,1), vettore _color=vettore(255,255,255), double _refl=0, double _opac=0, double _emit=0, entity e=entity());
	
	double intersect(ray r);
	vettore normal(ray r);
};


class mesh : public object
{
protected:
	int n_tr;
	vettore max, min;
	vector<triangle> box;
	vector<triangle> v_tr;
	int H_lst;
public:
	mesh(string file, double scale = 1, vettore _color = vettore(255, 255, 255), double _refl = 0, double _opac = 0, double _emit = 0, entity e = entity());

	double intersect(ray r);
	vettore normal(ray r);

	void move(vettore m);
	void move_to(vettore m);
	void rotate_abs(vettore r);
//	void rotate_to(vettore _dx, vettore _dy, vettore _dz);
};


class scene
{
protected:
	camera cam;
	vector<light*> lig;
	vector<object*> obj;

	void (*move)(camera &, vector<object *> &, vector<light *> &, double);

	vector<vettore> render(int x, int y);
	vector<vettore> rend_p(int x, int y, int n_sample, int bounce);

	vettore radiance(ray r, int n_sample, int bounce, int ref=0, int H_prev=-1);

	int strt_bnc;
	std::mt19937_64 eng;
	double n_glob;
public:
	scene(camera _cam, void (*_move)(camera &, vector<object *> &, vector<light *> &, double));
	void set_cam(camera &c);
	void add_obj(object &o);
	void add_lig(light &l);
	
	void rend_img(string file="render.png", double scale=100, double n=1);
	void rend_term(int slp_t=100, double dn=0.01, double nf=10, double ni=0);

	void rend_img_p(string file="render.png", double scale=100, double n=1,  int n_sample=100, int bounce=3);
	void upgr_img_p(string file="render.png", double scale=100, double n=1,  int n_sample=100, int bounce=3, int n_iter=1);
};

#endif
