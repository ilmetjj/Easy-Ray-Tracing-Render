#include "rend.h"

using namespace std;

void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height)
{
	// Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	// if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

vettore ray::point(double t)
{
	return o + t * d;
}

entity::entity(vettore _pos) : pos(_pos) {}
void entity::move(vettore m)
{
	pos = pos + m;
}
void entity::move_to(vettore m)
{
	pos = m;
}

camera::camera(vettore _pos, double _x, double _y) : entity(_pos), x(_x), y(_y) {}
double camera::width() { return x; }
double camera::height() { return y; }
ray camera::cast(double px, double py)
{
	ray r;
	r.o = vettore(px, py, 0);
	r.d = (vettore(px, py, 0) - pos).normalize();
	return r;
}

object::object(vettore _pos, double _refl, double _scat, vettore _color) : entity(_pos), refl(_refl), scat(_scat), color(_color) {}
ray object::reflect(ray r)
{
	double t = intersect(r);
	vettore P = r.point(t);
	vettore n = normal(P);

	t = (n * (r.o - P)) / (n * n);
	vettore Q = P + (n * t);
	ray s;
	s.o = P;
	s.d = (2 * Q - r.o - P);
	return s;
}
double object::get_ref()
{
	return refl;
}
vettore object::get_color()
{
	return color;
}

light::light(vettore _pos, double _i, vettore _color) : object(_pos, -1, 0, _color), i(_i), R(1) {}
ray light::cast(vettore p)
{
	ray r;
	r.o = pos;
	r.d = (p - pos).normalize();
	return r;
}
vettore light::shade(vettore p, vettore n)
{
	vettore r = (pos - p).normalize();
	return color * ((r * n) * (i * soft));
}
double light::intersect(ray r)
{
	double a = r.d * r.d;
	double b = 2 * (r.o - pos) * r.d;
	double c = (r.o - pos) * (r.o - pos) - pow(R, 2);
	double d = pow(b, 2) - 4 * a * c;

	if (d < 0)
		return 0;

	double t1 = (0 - b - sqrt(d)) / (2 * a);
	double t2 = (0 - b + sqrt(d)) / (2 * a);

	if (t1 * t2 < 0)
		return t2 < 0 ? t1 : t2;

	if (t1 < 0 and t2 < 0)
		return 0;

	return t1 < t2 ? t1 : t2;
}
vettore light::normal(vettore p)
{
	return (p - pos).normalize();
}

sphere::sphere(vettore _pos, double _R, double _refl, vettore _color) : object(_pos, _refl, 0, _color), R(_R) {}
double sphere::intersect(ray r)
{
	double a = r.d * r.d;
	double b = 2 * (r.o - pos) * r.d;
	double c = (r.o - pos) * (r.o - pos) - pow(R, 2);
	double d = pow(b, 2) - 4 * a * c;

	if (d < 0)
		return 0;

	double t1 = (0 - b - sqrt(d)) / (2 * a);
	double t2 = (0 - b + sqrt(d)) / (2 * a);

	if (t1 * t2 < 0)
		return t2 < 0 ? t1 : t2;

	if (t1 < 0 and t2 < 0)
		return 0;

	return t1 < t2 ? t1 : t2;
}
vettore sphere::normal(vettore p)
{
	return (p - pos).normalize();
}

plane::plane(vettore _pos, vettore _N, double _refl, vettore _color) : object(_pos, _refl, 0, _color), N(_N)
{
	N.normalize();
}
double plane::intersect(ray r)
{
	double num = (pos - r.o) * N;
	double den = r.d * N;
	double t = num / den;

	if (t < 0)
		return 0;

	return t;
}
vettore plane::normal(vettore p)
{
	return N.normalize();
}

scene::scene(camera _cam, void (*_move)(vector<object *> &, vector<light *> &, double)) : cam(_cam), move(_move) {}
void scene::set_cam(camera &c) { cam = c; }
void scene::add_obj(object &o)
{
	obj.push_back(&o);
}
void scene::add_lig(light &l)
{
	lig.push_back(&l);
	obj.push_back(&l);
}

vector<vettore> scene::render(int width, int height){

	vector<vettore> vec;
	vec.reserve(width*height);

	double xd, xi;
	xd = cam.width() / width;
	xi = cam.width() / 2;

	double yd, yi;
	yd = cam.height() / height;
	yi = cam.height() / 2;

	for (int i = 0; i < height; i++)
	{
		for (double j = 0; j < width; j++)
		{
			ray r = cam.cast(xi - xd * j, yi - yd * i);
			int H = 0;
			double T = (*obj[H]).intersect(r);
			for (int h = 1; h < obj.size(); h++)
			{
				double t = (*obj[h]).intersect(r);
				if (t > 0)
				{
					if (t < T || T == 0)
					{
						T = t;
						H = h;
					}
				}
			}
			if (T <= 0)
			{
				vec.push_back(0);
			}
			else
			{

				bool ok = true;

				int c_r = 0;
				while ((*obj[H]).get_ref() > 0 and c_r < 15)
				{
					c_r++;
					r = (*obj[H]).reflect(r);
					T = 0;
					int Hi = H;
					for (int h = 1; h < obj.size(); h++)
					{
						double t = (*obj[h]).intersect(r);
						if (t > 0 and h != Hi)
						{
							if (t < T || T == 0)
							{
								T = t;
								H = h;
							}
						}
					}
					if (T <= 0)
					{
						vec.push_back(0);
						ok = false;
						break;
					}
				}

				if ((*obj[H]).get_ref() < 0)
				{
					ok = false;
					vec.push_back((*obj[H]).get_color());
				}

				if (ok)
				{
					vettore I;
					vettore P = r.point(T);
					for (int h = 0; h < lig.size(); h++)
					{
						ray s = (*lig[h]).cast(P);

						int H2 = H;
						double T2 = (*obj[H2]).intersect(s);
						for (int k = 1; k < obj.size(); k++)
						{
							if (obj[k] != lig[h])
							{
								double t = (*obj[k]).intersect(s);
								if (t > 0)
								{
									if (t < T2 || T2 == 0)
									{
										T2 = t;
										H2 = k;
									}
								}
							}
						}
						if (H2 == H)
						{
							vettore N = (*obj[H]).normal(P);
							vettore S = (*lig[h]).shade(P, N);
							S = S.per((*obj[H]).get_color() / 255);
							if (S > 0)
							{
								I = I + S;
							}
						}
					}
					vec.push_back(I);
				}
			}
		}
	}

	return vec;
}

void scene::rend_img(string file, double scale, double n)
{
	move(obj, lig, n);

	unsigned width = cam.width() * scale, height = cam.height() * scale;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);

	vector<vettore> vec;
	vec=render(width, height);

	for(int i=0; i<vec.size(); i++){
		image[4 * i + 0] = vec[i].get_x() < 255 ? vec[i].get_x() : 255;
		image[4 * i + 1] = vec[i].get_y() < 255 ? vec[i].get_y() : 255;
		image[4 * i + 2] = vec[i].get_z() < 255 ? vec[i].get_z() : 255;
		image[4 * i + 3] = 255;
	}

	encodeOneStep(file.c_str(), image, width, height);
}

void scene::rend_term(int slp_t, double dn, double nf, double ni){
	struct winsize w;
	double width, height;
	for (float n = ni; n < nf; n+=dn)
	{
		usleep(slp_t);

		move(obj, lig, n);

		ioctl(0, TIOCGWINSZ, &w);

		width=w.ws_col;
		height=w.ws_row;

		vector<vettore> vec;
		vec = render(width, height);

		int c=0;
		for (int i = 0; i < height; i++){
			for(int j=0; j<width; j++){

				float I;
				I = vec[i * width + j].mod();
//				I = (vec[i * width + j].get_x() + vec[i * width + j].get_y() + vec[i * width + j].get_z())/3;
				I*=term_filt;

				if (I < 0)
				{
					putchar('X');
				}
				else if (I == 0)
				{
					putchar(' ');
				}
				else if (0 < I and I <= 0.25)
				{
					putchar('.');
				}
				else if (0.25 < I and I <= 0.5)
				{
					putchar(',');
				}
				else if (0.5 < I and I <= 0.75)
				{
					putchar('-');
				}
				else if (0.75 < I and I <= 1)
				{
					putchar('~');
				}
				else if (1 < I and I <= 1.25)
				{
					putchar(':');
				}
				else if (1.25 < I and I <= 1.5)
				{
					putchar(';');
				}
				else if (1.5 < I and I <= 1.75)
				{
					putchar('=');
				}
				else if (1.75 < I and I <= 2)
				{
					putchar('!');
				}
				else if (2 < I and I <= 2.25)
				{
					putchar('*');
				}
				else if (2.25 < I and I <= 2.5)
				{
					putchar('#');
				}
				else if (2.5 < I and I <= 2.75)
				{
					putchar('$');
				}
				else if (2.75 < I)
				{
					putchar('@');
				}
				else
				{	
					putchar('X');
				}

				c++;
			}
			putchar(10);
		}
		printf("\x1b [23A");
	}
}