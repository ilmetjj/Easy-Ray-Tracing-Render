#include "rend.h"

using namespace std;

/*Image function*/

void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height){
	// Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	// if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

void decodeOneStep(const char *filename, std::vector<unsigned char> &image) {
//	std::vector<unsigned char> image; // the raw pixels
	unsigned width, height;

	// decode
	unsigned error = lodepng::decode(image, width, height, filename);

	// if there's an error, display it
	if (error)
		std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

	// the pixels are now in the vector "image", 4 bytes per pixel, ordered
	// RGBARGBA..., use it as texture, draw it, ...
}

/*Ray*/
ray::ray(vettore _o, vettore _d):o(_o), d(_d){
	d.normalize();
}
vettore ray::point(double t){
	return o + t * d;
}

/*Entity*/

entity::entity(vettore _pos , vettore _dx, vettore _dy, vettore _dz):pos(_pos), dx(_dx), dy(_dy), dz(_dz) {
	dx.normalize();
	dz=normalize(dx%dy);
	dy=normalize(dz%dx);
}
void entity::move(vettore m){
	pos = pos + m;
}
void entity::move_to(vettore m){
	pos = m;
}
void entity::rotate_abs(vettore r){
	if(r.get_x()!=0){
		vettore rx[3]={vettore(1,0,0),vettore(0,cos(r.get_x()),-sin(r.get_x())),vettore(0,sin(r.get_x()),cos(r.get_x()))};
		dx=dx*rx;
		dy=dy*rx;
		dz=dz*rx;
	}
	if (r.get_y() != 0)
	{
		vettore ry[3] = {vettore(cos(r.get_y()), 0, sin(r.get_y())), vettore(0, 1, 0), vettore(-sin(r.get_y()), 0, cos(r.get_y()))};
		dx = dx * ry;
		dy = dy * ry;
		dz = dz * ry;
	}
	if (r.get_z() != 0)
	{
		vettore rz[3] = {vettore(cos(r.get_z()), -sin(r.get_z()), 0), vettore(sin(r.get_z()), cos(r.get_z()), 0), vettore(0, 0, 1)};
		dx = dx * rz;
		dy = dy * rz;
		dz = dz * rz;
	}

	dx.normalize();
	dz = normalize(dx % dy);
	dy = normalize(dz % dx);
}
void entity::rotate_to(vettore _dx, vettore _dy, vettore _dz){
	dx=_dx; dy=_dy; dz=_dz;
}
void entity::point_to(vettore p, vettore up){
	dz = normalize(p - pos);
	dx = normalize(up % dz);
	dy = normalize(dz % dx);
}

vettore entity::get_pos(){return pos;}
vettore entity::get_dx(){return dx;}
vettore entity::get_dy(){return dy;}
vettore entity::get_dz(){return dz;}

/*Camera*/

camera::camera(double _lx, double _ly, double _df, entity e): entity(e), lx(_lx), ly(_ly), df(_df){}
double camera::width(){return lx;}
double camera::height(){return ly;}
double camera::focal(){return df;}

ray camera::cast(double px, double py){
	ray r;

	r.o= pos + (dz*df) + (dx*px) + (dy*py);
	r.d=normalize(r.o-pos);
	
	return r;
}



/*Light*/

light::light(vettore _color, double _I, entity e):entity(e), color(_color), I(_I){}

double light::get_I(){return I;}
vettore light::get_color(){return color;}

/*L_point*/

l_point::l_point(double _R, vettore _color, double _I, entity e):light(_color, _I, e), R(_R){}

ray l_point::cast(vettore p){
	ray r;
	r.o=pos;
	r.d=normalize(p - pos);
	return r;
}
double l_point::intersect(ray r){
/*	double a = r.d * r.d;
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
	*/
//	r.d.normalize();
	double u, e;
	vettore h=pos-r.o;
	u=h*r.d;
	e=pow(u,2)-(h*h)+pow(R,2);
	if(e<0){
		return 0;
	}
	else if(e==0){
		return u>0?u:0;
	}
	else{
		double t1=u-sqrt(e), t2=u+sqrt(e);
		if(t1==0 or t2==0){
			return 0;
		}
		if(t1<0 and t2<0){
			return 0;
		}
		else if(t1>0 and t2>0){
			return t1<t2?t1:t2;
		}
		else{
			return t1>0?t1:t2;
		}
	}

}
vettore l_point::shade(vettore p, vettore n){
	vettore r = normalize(pos - p);
	double t=pow(dist(p,pos),2);
	return color * ((r * n) * (I * soft))/t;
}

/*Sun*/

sun::sun(vettore _angle, vettore _color, double _I, entity e):light(_color, _I, e), angle(_angle){
	angle.normalize();
}

ray sun::cast(vettore p){
	ray r;
	r.o=(p+angle*1e4);
	r.d=angle.dir();
	return r;
}
double sun::intersect(ray r){
	if(r.d.dir()*angle>sun_appr){
		return 1e4;
	}
	else
		return 0;
}
vettore sun::shade(vettore p, vettore n){
	return color * ((angle * n) * (I * soft));
}
/*Object*/

object::object(vettore _color, double _refl, double _opac, double _emit, entity e): entity(e), color(_color), refl(_refl), opac(_opac), emit(_emit), n(1.5){}
ray object::reflect(ray r){/*
	ray s;
//	r.d.normalize();
	vettore P = r.point(intersect(r));
	s.o = P;
	vettore n = normal(r);
	double t;
	t = (n * (r.o - P)) / (n * n);
	vettore Q = P + (n * t);
	s.d = normalize(2 * Q - r.o - P);
	return s;*/

	ray s;
	vettore P = r.point(intersect(r));
	s.o = P;
	vettore N = normal(r);
	s.d= r.d-(2*r.d*N)*N;
	return s;
}
ray object::cast(ray r, std::mt19937_64& eng){
	ray s;
	s.o = r.point(intersect(r));

	vettore N=normal(r);

	std::uniform_real_distribution<double> d_unit(0,1);

//	double R = sqrt(d_unit(eng)), th = 2 * M_PI * d_unit(eng);
//	double dx=R*cos(th), dy=R*sin(th);
//	double h_sq = 1 - dx *dx - dy *dy ;
//	double h = sqrt(h_sq>0?h_sq:0);

	double R = sqrt(d_unit(eng)), th = 2 * M_PI * d_unit(eng);
	double dx=R*cos(th), dy=R*sin(th);
	double dz=sqrt(1-dx*dx-dy*dy);

	vettore vx=dir((N!=vettore(1,0,0) and N!=vettore(-1,0,0))?N%vettore(1,0,0):N%vettore(0,1,0));
	vettore vy=dir(N%vx);

	
	
	s.d=(vx * dx + vy * dy + N * dz);	
	return s;
/*	double x = 0, y = 0, z = 0;
	vettore N, v(0, 0, 0);
	N = normal(r).dir();

	std::uniform_real_distribution<double> distr(-1, 1);
	std::uniform_real_distribution<double> d_cos(0, M_PI_2);
	std::uniform_real_distribution<double> d_unit(0, 1);

	do
	{
		x = distr(eng);
		y = distr(eng);
		z = distr(eng);
		v = vettore(x, y, z);
	} while (v.mod() > 1 or v.dir() * N.dir() <= 0); // d_unit(eng) );//cos(d_cos(eng)));

	return ray(r.point(intersect(r)), normalize(v));*//*

	vettore N=normal(r);

	std::uniform_real_distribution<double> d_unit(0,1);

	double e1=d_unit(eng), e2=d_unit(eng);
	double dx=cos(2*M_PI*e2)*sqrt(1-e1*e1), dy=sin(2*M_PI*e2)*sqrt(1-e1*e1), dz=e1;

	vettore vx=(N!=vettore(1,0,0) and N!=vettore(-1,0,0))?N%vettore(1,0,0):N%vettore(0,1,0);
	vettore vy=N%vx;

	return ray(r.point(intersect(r)), (vx * dx + vy * dy + N * dz));*/

}

ray object::trapass(ray r, double n_ext){
	ray s;
	vettore P=r.point(intersect(r)), N=normal(r);
	s.o=P;
/*
	vettore dx, dy, dz;
	double lx, lz;
	double n1, n2;
	if(N*r.d<0){
		dz=N*(-1);
		n1=n_ext;
		n2=n;
	}
	else{
		dz = N;
		n1=n;
		n2=n_ext;
	}
	dy = dir(dz % r.d);
	dx = dir(dy % dz);

	double sin1, sin2;
	sin1=sqrt(1-pow(r.d*N,2));
	sin2=sin1*n1/n2;

	lx=sin2;
	lz=sqrt(1-lx*lx);

	s.d=dx*lx+dz*lz;*/

	double n1, n2;
	if (N * r.d < 0) {
		N=N*(-1);
		n1 = n_ext;
		n2 = n;
	}
	else {
		n1 = n;
		n2 = n_ext;
	}
	double cos1,cos2;
	cos1=r.d*N; cos2=sqrt(1-pow(n1/n2,2)*(1-pow(cos1,2)));
	s.d=(n1/n2)*r.d+(cos2-n1/n2*cos1)*N;

/*
	cout<<" r.o= "<<r.o.print()<<flush;
	cout<<" r.d= "<<r.d.print()<<flush;
	cout<<" n1= "<<n1<<" n2= "<<n2<<flush;
	cout << " s.o= " << s.o.print() << flush;
	cout << " s.d= " << s.d.print() << flush;
	cout << " dx= " << dx.print() << flush;
	cout << " dy= " << dy.print() << flush;
	cout << " dz= " << dz.print() << flush;
	cout << " lx= " << lx << flush;
	cout << " lz= " << lz << flush;
	cout << " sin1= " << sin1 << flush;
	cout << " sin2= " << sin2 << flush;
*/
	return s;
}

double object::get_refl(){return refl;}
double object::get_opac(){return opac;}
double object::get_emit(){return emit;}
vettore object::get_color(){return color;}
double object::get_n(){return n;}

/*Sphere*/

sphere::sphere(double _R, vettore _color, double _refl, double _opac, double _emit, entity e):object(_color, _refl, _opac, _emit, e), R(_R){}
double sphere::intersect(ray r){
//	r.d.normalize();
	double u, e;
	vettore h=pos-r.o;
	u=h*r.d;
	e=pow(u,2)-(h*h)+pow(R,2);
	if(e<0){
		return 0;
	}
	else if(e==0){
		return u>0?u:0;
	}
	else{
		double t1=u-sqrt(e), t2=u+sqrt(e);
		if(t1==0 or t2==0){
			return 0;
		}
		if(t1<0 and t2<0){
			return 0;
		}
		else if(t1>0 and t2>0){
			return t1<t2?t1:t2;
		}
		else{
			return t1>0?t1:t2;
		}
	}
}
vettore sphere::normal(ray r){
//	r.d.normalize();
	vettore p=r.point(intersect(r));
	return normalize(p - pos);
}

/*Plane*/

plane::plane(vettore _N, vettore _color, double _refl, double _opac, double _emit, entity e):object(_color, _refl, _opac, _emit, e), N(_N){
	N.normalize();
}
double plane::intersect(ray r){
//	r.d.normalize();

	double num = (pos - r.o) * N;
	double den = r.d * N;
	if(den==0)
		return 0;
	double t = num / den;

	if (t <= 0)
		return 0;

	return t;
}
vettore plane::normal(ray r){
	return /*normalize*/(N);
}

/*Scene*/

scene::scene(camera _cam, void (*_move)(camera &, vector<object *> &, vector<light *> &, double)) : cam(_cam), move(_move) {
	srand(time(NULL));
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	eng=mt19937_64(seed);
	n_glob=1;
}
void scene::set_cam(camera &c) {cam = c;}
void scene::add_obj(object &o){obj.push_back(&o);}
void scene::add_lig(light &l){lig.push_back(&l);}

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
			double T = obj[H]->intersect(r);
			for (int h = 0; h < obj.size(); h++)
			{
				double t = obj[h]->intersect(r);
				if (t > 0)
				{
					if (t < T || T <= 0)
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
				while (obj[H]->get_refl() > 0 and c_r < 15)
				{
					c_r++;
					r = obj[H]->reflect(r);
					T = 0;
					int Hi = H;
					for (int h = 0; h < obj.size(); h++)
					{
						double t = obj[h]->intersect(r);
						if (t > 0 and h != Hi)
						{
							if (t < T || T <= 0)
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

				if (obj[H]->get_refl() < 0)
				{
					ok = false;
					vec.push_back(obj[H]->get_color());
				}

				if (ok)
				{
					vettore I;
					vettore P = r.point(T);
					for (int h = 0; h < lig.size(); h++)
					{
						ray s = lig[h]->cast(P);

						int H2 = H;
						double T2 = obj[H2]->intersect(s);
						for (int k = 0; k < obj.size(); k++)
						{
							double t = obj[k]->intersect(s);
							if (t > 0)
							{
								if (t < T2 || T2 <= 0)
								{
									T2 = t;
									H2 = k;
								}
							}
						}
						if (H2 == H)
						{
							vettore N = obj[H]->normal(r);
							vettore S = lig[h]->shade(P, N);
							S = S.per(obj[H]->get_color() / 255);
							if (S.get_x()>=0 and S.get_y()>=0 and S.get_z()>=0)
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

vector<vettore> scene::rend_p(int width, int height, int n_sample, int bounce){
	vector<vettore> vec;
	vec.reserve(width*height);

	double xd, xi;
	xd = cam.width() / width;
	xi = cam.width() / 2;

	double yd, yi;
	yd = cam.height() / height;
	yi = cam.height() / 2;

	strt_bnc=bounce;

	for (int i = 0; i < height; i++)
	{
		cout<<i<<":	"<<flush;
		for (int j = 0; j < width; j++)
		{
//			cout<<j<<" "<<flush;
			vettore color;
			ray r = cam.cast(xi - xd * j, yi - yd * i);
			color=radiance(r,n_sample,bounce);
			vec.push_back(color);
		}
		cout<<endl;
	}
	return vec;
}

vettore scene::radiance(ray r, int n_sample, int bounce, int ref, int H_prev){
	vettore color=vettore(0,0,0);

	bool hit_l=false;
	int H = 0;
	double T = 0;
	for (int h = 0; h < lig.size(); h++)
	{
		double t = lig[h]->intersect(r);
		if (t > t_min)
		{
			if (t < T || T <= 0)
			{
				T = t;
				H = h;
				hit_l=true;
			}
		}
	}
	for (int h = 0; h < obj.size(); h++)
	{
		double t = obj[h]->intersect(r);
		if (t > t_min)
		{
			if (t < T || T <= 0)
			{
				T = t;
				H = h;
				hit_l=false;
			}
		}
	}
	if(hit_l){
		color= lig[H]->get_color()*lig[H]->get_I();
	}
	else if(T>0 and bounce>0){
		ray s;
		vettore get_col;

		if(obj[H]->get_refl()>0){
			s=obj[H]->reflect(r);
			get_col=radiance(s,n_sample, bounce-1,ref+1,H);
			get_col = get_col.per(obj[H]->get_color() / 255);
			color=color+get_col;
		}
		else if (obj[H]->get_opac() == 0 and obj[H]->get_refl() == 0 and obj[H]->get_emit()==1){
			
			double n1=1, n2=1, R, T, R0, cos_th, sin_th;
			cos_th = obj[H]->normal(r) * r.d;
			sin_th=sqrt(1-cos_th*cos_th);
			bool intern=false;
			if(cos_th<0){
				n1=n_glob;
				n2=obj[H]->get_n();
				cos_th=0-cos_th;
			}
			else{
				n2 = n_glob;
				n1 = obj[H]->get_n();
				intern=true;
			}
			R0=pow((n1-n2)/(n1+n2),2);
			R=R0+(1-R0)*pow(1-cos_th,5);
			T=1-R;

			if(R<=0 or T<=0)
			cout<<" R0= "<<R0<<" R= "<<R<<" T "<<T<<flush;

//			if(R)
			if(!intern){
				s=obj[H]->reflect(r);
				get_col = R * radiance(s, n_sample, bounce - 1, ref + 1, H);
				get_col = get_col.per(obj[H]->get_color() / 255);
				color=color+get_col;
			}
			if(n1<n2 or sin_th<n2/n1){
				s=obj[H]->trapass(r, n_glob);
				get_col = T * radiance(s, n_sample, bounce - 1, ref + 1, H);
				get_col = get_col.per(obj[H]->get_color() / 255);
				color = color + get_col;
			}
		}
		else if(obj[H]->get_opac()==1){
			vettore sum=vettore(0,0,0);
			
			int n_smp=n_sample*pow(double(bounce+ref)/strt_bnc,strt_bnc);
			n_smp= n_smp>min_smp?n_smp:min_smp;
			for(int i=0; i<n_smp; i++){
				get_col=vettore(0,0,0);
				
				s=obj[H]->cast(r, eng);
				get_col=radiance(s,n_sample,bounce-1,ref,H);// *(s.d*obj[H]->normal(r));
				get_col = get_col.per(obj[H]->get_color() / 255);
				sum=sum+get_col;
			}
			sum=sum*(1/double(n_smp));
			
			color=color+sum;
		}
	}
	else if(T>0 and bounce>0 and H==H_prev){
		cout<<" T: "<<T<<flush;
	}

	return color;

}

void scene::rend_img(string file, double scale, double n)
{
	move(cam, obj, lig, n);

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
	for (double n = ni; n < nf; n+=dn)
	{
		usleep(slp_t);

		move(cam, obj, lig, n);

		ioctl(0, TIOCGWINSZ, &w);

		width=w.ws_col;
		height=w.ws_row;

		vector<vettore> vec;
		vec = render(width, height);

		int c=0;
		for (int i = 0; i < height; i++){
			for(int j=0; j<width; j++){

				double I;
				I = vec[i * width + j].mod();
//				I = (vec[i * width + j].get_x() + vec[i * width + j].get_y() + vec[i * width + j].get_z())/3;
				I*=term_filt;

				if (I < 0)
				{
					std::putchar('X');
				}
				else if (I == 0)
				{
					std::putchar(' ');
				}
				else if (0 < I and I <= 0.25)
				{
					std::putchar('.');
				}
				else if (0.25 < I and I <= 0.5)
				{
					std::putchar(',');
				}
				else if (0.5 < I and I <= 0.75)
				{
					std::putchar('-');
				}
				else if (0.75 < I and I <= 1)
				{
					std::putchar('~');
				}
				else if (1 < I and I <= 1.25)
				{
					std::putchar(':');
				}
				else if (1.25 < I and I <= 1.5)
				{
					std::putchar(';');
				}
				else if (1.5 < I and I <= 1.75)
				{
					std::putchar('=');
				}
				else if (1.75 < I and I <= 2)
				{
					std::putchar('!');
				}
				else if (2 < I and I <= 2.25)
				{
					std::putchar('*');
				}
				else if (2.25 < I and I <= 2.5)
				{
					std::putchar('#');
				}
				else if (2.5 < I and I <= 2.75)
				{
					std::putchar('$');
				}
				else if (2.75 < I)
				{
					std::putchar('@');
				}
				else
				{	
					std::putchar('X');
				}

				c++;
			}
			std::putchar(10);
		}
		printf("\x1b [23A");
	}
}

void scene::rend_img_p(string file, double scale, double n,  int n_sample, int bounce)
{
	move(cam, obj, lig, n);

	unsigned width = cam.width() * scale, height = cam.height() * scale;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);

	vector<vettore> vec;
	vec=rend_p(width, height, n_sample, bounce);
	image.resize(width * height * 4);

	cout<<"ended"<<endl;;
	for(int i=0; i<vec.size(); i++){
		image[4 * i + 0] = vec[i].get_x() < 255 ? vec[i].get_x() : 255;
		image[4 * i + 1] = vec[i].get_y() < 255 ? vec[i].get_y() : 255;
		image[4 * i + 2] = vec[i].get_z() < 255 ? vec[i].get_z() : 255;
		image[4 * i + 3] = 255;
	}

	encodeOneStep(file.c_str(), image, width, height);
}

void scene::upgr_img_p(string file, double scale, double n, int n_sample, int bounce, int iter) {
	move(cam, obj, lig, n);

	unsigned width = cam.width() * scale, height = cam.height() * scale;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);
	
//	decodeOneStep(file.c_str(), image);

	vector<vettore> vec, temp;
	vec = rend_p(width, height, n_sample, bounce);
	cout << "ended 0 " << endl;
	for (int i = 0; i < vec.size(); i++) {
		image[4 * i + 0] = vec[i].get_x() < 255 ? vec[i].get_x() : 255;
		image[4 * i + 1] = vec[i].get_y() < 255 ? vec[i].get_y() : 255;
		image[4 * i + 2] = vec[i].get_z() < 255 ? vec[i].get_z() : 255;
		image[4 * i + 3] = 255;
	}
	encodeOneStep(file.c_str(), image, width, height);
	cout << "ended 0 " << endl;

	for(int j=1; j<iter; j++){
		temp = rend_p(width, height, n_sample, bounce);
		cout << "ended "<< j << endl;
		for (int i = 0; i < vec.size(); i++) {
			vec[i]=(vec[i]*j+temp[i])/(j+1);
			image[4 * i + 0] = (vec[i].get_x() < 255 ? vec[i].get_x() : 255);
			image[4 * i + 1] = (vec[i].get_y() < 255 ? vec[i].get_y() : 255);
			image[4 * i + 2] = (vec[i].get_z() < 255 ? vec[i].get_z() : 255);
			image[4 * i + 3] = 255;
		}
		temp.clear();
		encodeOneStep(file.c_str(), image, width, height);
	}
}