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
#define term_filt 0.05f

void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height);

//structures
struct ray;

//basic entity
class entity;

//type of entities
class camera;
class light;
class object;

//objects
class sphere;
class plane;

//scene
class scene;


struct ray{
	vettore o;
	vettore d;
	vettore point(double t);
};

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
	void point_to(vettore p);

	vettore get_pos();
};

class camera : public entity{
private:
	double lx, ly, df;
public:
	camera(vettore _pos=vettore(0,0,-1), double _lx=8, double _ly=5, double _df=100);
	double width();
	double height();
	ray cast(double px, double py);
};

class object : public entity
{
protected:
	double refl, scat;
	vettore color;

public:
	object(vettore _pos=vettore(0,0,1), double refl=0, double scat=0, vettore _color=vettore(255,255,255));
	virtual double intersect(ray r)=0;
	virtual vettore normal(vettore p)=0;
	ray reflect(ray r);
	double get_ref();
	vettore get_color();
};

class light : public object
{
private:
	double i, R;

public:
	light(vettore _pos = vettore(1, 1, -1), double _i = 1, vettore _color=vettore(255,255,255));
	ray cast(vettore p);
	vettore shade(vettore p, vettore n);
	double intersect(ray r);
	vettore normal(vettore p);
};

class sphere : public object{
private:
	double R;
public:
	sphere(vettore _pos=vettore(0,0,1), double _R=1, double _refl=0, vettore _color=vettore(255,255,255));
	double intersect(ray r);
	vettore normal(vettore p);
};
class plane : public object
{
private:
	vettore N;

public:
	plane(vettore _pos = vettore(0, 0, 1), vettore N = vettore(0,1,0), double _refl=0, vettore _color=vettore(255,255,255));
	double intersect(ray r);
	vettore normal(vettore p);
};

class scene
{
private:
	camera cam;
	vector<light*> lig;
	vector<object*> obj;

	void (*move)(camera &, vector<object *> &, vector<light *> &, double);

	vector<vettore> render(int x, int y);

public:
	scene(camera _cam, void (*_move)(camera &, vector<object *> &, vector<light *> &, double));
	void set_cam(camera &c);
	void add_obj(object &o);
	void add_lig(light &l);
	
	void rend_img(string file="render.png", double scale=100, double n=1);
	void rend_term(int slp_t=100, double dn=0.01, double nf=10, double ni=0);
};

#endif
