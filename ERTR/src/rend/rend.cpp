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

vettore ray::point(double t){
	return o+t*d;
}

entity::entity(vettore _pos):pos(_pos){}
void entity::move(vettore m){
	pos=pos+m;
}
void entity::move_to(vettore m)
{
	pos = m;
}

camera::camera(vettore _pos, double _x , double _y):entity(_pos),x(_x),y(_y){}
double camera::width(){return x;}
double camera::hight(){return y;}
ray camera::cast(double px, double py){
	ray r;
	r.o = vettore(px, py, 0);
	r.d=(vettore(px,py,0)-pos).normalize();
	return r;
}

object::object(vettore _pos, double _refl, double _scat, vettore _color):entity(_pos),refl(_refl),scat(_scat), color(_color) {}
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
double object::get_ref(){
	return refl;
}
vettore object::get_color(){
	return color;
}

light::light(vettore _pos, double _i, vettore _color) : object(_pos, -1, 0, _color), i(_i), R(1){}
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
	return color*((r * n) * (i*soft));
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

sphere::sphere(vettore _pos, double _R, double _refl, vettore _color):object(_pos, _refl, 0, _color), R(_R){}
double sphere::intersect(ray r){
	double a=r.d*r.d;
	double b=2*(r.o-pos)*r.d;
	double c=(r.o-pos)*(r.o-pos)-pow(R,2);
	double d = pow(b,2) - 4 *a *c ;

	if(d<0)
		return 0;
	
	double t1 = (0-b - sqrt(d)) / (2 * a);
	double t2 = (0-b + sqrt(d)) / (2 * a);

	if (t1 * t2 < 0)
	return t2<0?t1:t2;

	if (t1 < 0 and t2 < 0)
	return 0;
	
	return t1<t2?t1:t2;	
	
}
vettore sphere::normal(vettore p){
	return (p-pos).normalize();
}

plane::plane(vettore _pos, vettore _N, double _refl, vettore _color) : object(_pos, _refl, 0, _color), N(_N) {
	N.normalize();
}
double plane::intersect(ray r)
{
	double num=(pos-r.o)*N;
	double den=r.d*N;
	double t=num/den;

	if (t < 0)
	return 0;

	return t;
}
vettore plane::normal(vettore p)
{
	return N.normalize();
}

scene::scene(camera _cam):cam(_cam){}
void scene::set_cam(camera &c){cam=c;}
void scene::add_obj(object &o){
	obj.push_back(&o);
}
void scene::add_lig(light &l){
	lig.push_back(&l);
	obj.push_back(&l);
}
/*
void scene::render(){
	struct winsize w;
	vector<char> scr;
	for(double n=1;n>0;n+=0.001){
		
		ioctl(0, TIOCGWINSZ, &w);

		vettore m(sin(n*7) * 35, sin(n*7) * 15, 5 + cos(n*7) * 35);
		(*lig[0]).move_to(m);

		vettore m2(cos(-n * 14) * 10, sin(-n*14)-1, 15 + sin(-n * 14) * 10);
		(*obj[1]).move_to(m2);

		double xd, xi, xf;
		xd = cam.width()/ double(w.ws_col);
		xi = cam.width() / 2;
		xf = -cam.width() / 2;

		double yd, yi, yf;
		yd = cam.hight()/ double(w.ws_row);  
		yi = cam.hight() / 2;
		yf = -cam.hight() / 2;

//		std::cout << "xd " << xd << " xi " << xi << " xf " << xf << endl;
//		std::cout << "yd " << yd << " yi " << yi << " yf " << yf << endl;

		printf("\x1b [23A");
		for(double i=yi; i>yf+yd; i-=yd){
			for (double j = xi; j > xf+xd; j -= xd)
			{
				ray r=cam.cast(j,i);
				int H=0;
				double T=(*obj[H]).intersect(r);
				for(int h=1; h<obj.size(); h++){
					double t=(*obj[h]).intersect(r);
					if(t>0){
						if(t<T || T==0){
							T=t;
							H=h;
						}
					}
				}
				if (T <= 0)
				{
					scr.push_back(' ');
				}
				else{

					bool ok=true;

					int c_r=0;
					while ((*obj[H]).get_ref() > 0 and c_r<5)
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
							scr.push_back(' ');
							ok = false;
							break;
						}
					}

					if((*obj[H]).get_ref()<0){
						ok=false;
						scr.push_back('@');
					}

					if(ok){
						vettore I;
						vettore P=r.point(T);
						for (int h = 0; h < lig.size(); h++)
						{
							ray s=(*lig[h]).cast(P);
							
							int H2=H;
							double T2 = (*obj[H2]).intersect(s);
							for (int k = 1; k < obj.size(); k++)
							{
								if (obj[k] != lig[h])
								{double t = (*obj[k]).intersect(s);
								if (t > 0)
								{
									if (t < T2 || T2==0)
									{
										T2 = t;
										H2 = k;
									}
								}}
							}
							if(H2==H){
								vettore N=(*obj[H]).normal(P);
								vettore S=(*lig[h]).shade(P,N);
								if(S>0){
									I=I+S;
								}
							}
						}
	//					putchar('@');
	//					std::cout << "@" << flush;
	//					std::cout << I << flush;

						//.,-~:;= !*#$ @ 
						if (I < 0)
						{
						//	std::cout << "0" << flush;
							scr.push_back('0');
						}
						else if(I==0){
						//	std::cout << " " << flush;
							scr.push_back(' ');
						}
						else if (0<I and I<=0.25)
						{
						//	std::cout << "." << flush;
							scr.push_back('.');
						}
						else if (0.25 < I and I <= 0.5)
						{
						//	std::cout << "," << flush;
							scr.push_back(',');
						}
						else if (0.5 < I and I <= 0.75)
						{
						//	std::cout << "-"<< flush;
							scr.push_back('-');
						}
						else if (0.75 < I and I <= 1)
						{
						//	std::cout << "~" << flush;
							scr.push_back('~');
						}
						else if (1 < I and I <= 1.25)
						{
						//	std::cout << ":" << flush;
							scr.push_back(':');
						}
						else if (1.25 < I and I <= 1.5)
						{
						//	std::cout << ";" << flush;
							scr.push_back(';');
						}
						else if (1.5 < I and I <= 1.75)
						{
						//	std::cout << "=" << flush;
							scr.push_back('=');
						}
						else if (1.75 < I and I <= 2)
						{
						//	std::cout << "!" << flush;
							scr.push_back('!');
						}
						else if (2 < I and I <= 2.25)
						{
						//	std::cout << "*" << flush;
							scr.push_back('*');
						}
						else if (2.25 < I and I <= 2.5)
						{
						//	std::cout << "#" << flush;
							scr.push_back('#');
						}
						else if (2.5 < I and I <= 2.75)
						{
						//	std::cout << "$" << flush;
							scr.push_back('$');
						}
						else if (2.75<I)
						{
						//	std::cout << "@" << flush;
							scr.push_back('@');
						}
						else{
						//	std::cout<< I<<flush;
							scr.push_back('0');
						}
					}
				}
			}
	//		putchar(10);
	//		std::cout<<endl;
			scr.push_back(10);
		}
		for(int i=0; i<scr.size(); i++)
			putchar(scr[i]);
		usleep(100);
		scr.clear();
	//	
			
	}
}
*/
void scene::rend_img(string file, double scale, double n)
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	vettore m(sin(n * 7) * 35, sin(n * 7) * 15, 5 + cos(n * 7) * 35);
	(*lig[0]).move_to(m);

	vettore m2(cos(-n * 14) * 10, sin(-n * 14) - 1, 15 + sin(-n * 14) * 10);
	(*obj[1]).move_to(m2);

	double xd, xi, xf;
	xd = double(1)/scale;
	xi = cam.width() / 2;
	xf = -cam.width() / 2;

	double yd, yi, yf;
	yd = double(1) / scale;
	yi = cam.hight() / 2;
	yf = -cam.hight() / 2;

	unsigned width = cam.width() * scale, height = cam.hight() * scale;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);

	int c_pix = 0;
	for (int i = 0; i < height ; i++)
	{
		for (double j = 0; j < width ; j++)
		{
			ray r = cam.cast(xi-xd*j, yi-yd*i);
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
				image[4 * c_pix + 0] = 0;
				image[4 * c_pix + 1] = 0;
				image[4 * c_pix + 2] = 0;
				image[4 * c_pix + 3] = 255;
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
						image[4 * c_pix + 0] = 0;
						image[4 * c_pix + 1] = 0;
						image[4 * c_pix + 2] = 0;
						image[4 * c_pix + 3] = 255;
						ok = false;
						break;
					}
				}

				if ((*obj[H]).get_ref() < 0)
				{
					ok = false;
					image[4 * c_pix + 0] = (*obj[H]).get_color().get_x() < 255 ? (*obj[H]).get_color().get_x() : 255;
					image[4 * c_pix + 1] = (*obj[H]).get_color().get_y() < 255 ? (*obj[H]).get_color().get_y() : 255;
					image[4 * c_pix + 2] = (*obj[H]).get_color().get_z() < 255 ? (*obj[H]).get_color().get_z() : 255;
					image[4 * c_pix + 3] = 255;
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
							S=S.per((*obj[H]).get_color()/255);
							if (S > 0)
							{
								I = I+S;
							}
						}
					}
/*					if (I < 0)
					{
						image[4 * c_pix + 0] = 0;
						image[4 * c_pix + 1] = 0;
						image[4 * c_pix + 2] = 0;
						image[4 * c_pix + 3] = 255;
					}
*///					else{
						image[4 * c_pix + 0] = I.get_x() < 255 ? I.get_x() : 255;
						image[4 * c_pix + 1] = I.get_y() < 255 ? I.get_y() : 255;
						image[4 * c_pix + 2] = I.get_z() < 255 ? I.get_z() : 255;
						image[4 * c_pix + 3] = 255;
//					}
				}
			}

			c_pix++;
		}
	}

	encodeOneStep(file.c_str(), image, width, height);
}