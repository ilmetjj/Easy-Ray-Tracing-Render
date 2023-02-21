#ifndef _REND_
#define _REND_

#include<cmath>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<fstream>
#include<iostream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include"../GMath/GMath.h"
#include "../lodepng/lodepng.h"

using namespace std;

#define soft 0.5F
#define term_filt 0.01f
#define sun_appr 0.9f

void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height);

//structures
struct ray;
//	struct triangle;

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

/*
struct triangle{
	vettore a, b, c;
	vettore n;
	bool parallel();
	bool do_intersect();
	vettore intersect();
};
*/

class entity{
protected:
	vettore pos;
	vettore dx, dy, dz;
public:
	entity(vettore _pos = vettore(0, 0, 0), vettore _dx=vettore(1,0,0), vettore _dy=vettore(0,1,0), vettore _dz=vettore(0,0,1));
	
	void move(vettore m);
	void move_to(vettore m);
	void rotate_abs(vettore r);
	void rotate_to(vettore _dx, vettore _dy, vettore _dz);
	void point_to(vettore p, vettore up=vettore(0,1,0));

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

public:
	object(vettore _color=vettore(255,255,255), double _refl=0, double _opac=0, double _emit=0, entity e=entity());

	virtual double intersect(ray r)=0;
	virtual vettore normal(ray r)=0;
	
	ray reflect(ray r);
//	ray trapass(ray r);
	ray cast(ray r);

	double get_refl();
	double get_opac();
	double get_emit();
	vettore get_color();
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

/*
class mesh : public object
{
protected:
	vector<triangle> v_tr;
public:
	mesh(vector<triangle> v_tr, );
	void add_triangle(triangle);

	ray cast(vettore p);
	double intersect(ray r);
	vettore shade(vettore p, vettore n);
};
*/

class scene
{
protected:
	camera cam;
	vector<light*> lig;
	vector<object*> obj;

	void (*move)(camera &, vector<object *> &, vector<light *> &, double);

	vector<vettore> render(int x, int y);
	vector<vettore> rend_p(int x, int y, int n_sample, int bounce);

	vettore radiance(ray r, int n_sample, int bounce);

public:
	scene(camera _cam, void (*_move)(camera &, vector<object *> &, vector<light *> &, double));
	void set_cam(camera &c);
	void add_obj(object &o);
	void add_lig(light &l);
	
	void rend_img(string file="render.png", double scale=100, double n=1);
	void rend_term(int slp_t=100, double dn=0.01, double nf=10, double ni=0);

	void rend_img_p(string file="render.png", double scale=100, double n=1,  int n_sample=100, int bounce=3);
};

#endif
