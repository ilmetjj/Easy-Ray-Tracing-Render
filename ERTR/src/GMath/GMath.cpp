#include "GMath.h"

using namespace std;



/*vettore*/

vettore::vettore(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

double vettore::mod() const{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

void vettore::sferic(double &r, double &t, double &p) const{
	r = mod();
	if (x > 0)
	{
		t = atan(y / x);
	}
	else if (x < 0)
	{
		if (y >= 0)
		{
			t = atan(y / x) + M_PI;
		}
		else
		{
			t = atan(y / x) - M_PI;
		}
	}
	else
	{
		if (y > 0)
		{
			t = M_PI / 2;
		}
		else if (y < 0)
		{
			t = -M_PI / 2;
		}
		else
		{
			t = 0;
		}
	}
	if (z == 0)
	{
		p = 0;
	}
	else
	{
		p = acos(z / r);
	}
}


double vettore::get_x() const{
	return x;
}
double vettore::get_y() const{
	return y;
}
double vettore::get_z() const{
	return z;
}

vettore vettore::operator+(const vettore obj) const{
	return vettore(x + obj.x, y + obj.y, z + obj.z);
}
void vettore::operator+=(const vettore obj){
	x=x + obj.x;
	y=y + obj.y;
	z=z + obj.z;
}
vettore vettore::operator-(const vettore obj) const{
	return vettore(x - obj.x, y - obj.y, z - obj.z);
}
void vettore::operator-=(const vettore obj){
	x=x - obj.x;
	y=y - obj.y;
	z=z - obj.z;
}
vettore vettore::operator*(double n) const{
	return vettore(n * x, n * y, n * z);
}
void vettore::operator*=(double n){
	x=n * x;
	y=n * y;
	z=n * z;
}
vettore operator*(double n, const vettore obj){
	return vettore(n * obj.x, n * obj.y, n * obj.z);
}
vettore vettore::operator/(double n) const{
	return vettore(x / n, y / n, z / n);
}
void vettore::operator/=(double n){
	x=x / n;
	y=y / n;
	z=z / n;
}
double vettore::operator*(const vettore obj) const{
	return x * obj.x + y * obj.y + z * obj.z;
}
vettore vettore::operator%(const vettore obj) const{
	return vettore((y * obj.z - z * obj.y), (z * obj.x - x * obj.z), (x * obj.y - y * obj.x));
}
vettore vettore::operator*(const vettore v[3]) const {
	return vettore(x * v[0].x + y * v[0].y + z * v[0].z, x *v[1].x + y * v[1].y + z * v[1].z, x *v[2].x + y * v[2].y + z * v[2].z);
}

vettore vettore::per(vettore obj){
	return vettore(x*obj.x,y*obj.y,z*obj.z);
}

ostream &operator<<(ostream &os, const vettore &obj){
	os << obj.mod();
	return os;
}
istream &operator>>(istream &is, vettore &obj){
	double _x, _y, _z;
	is >> _x >> _y >> _z;
	obj.x = _x;
	obj.y = _y;
	obj.z = _z;
	return is;
}

bool vettore::operator<(const vettore obj) const{
	if (x != obj.x)
	{
		return x < obj.x;
	}
	else
	{
		if (y != obj.y)
		{
			return y < obj.y;
		}
		else
		{
			return z < obj.z;
		}
	}
}
bool vettore::operator>(const vettore obj) const{
	if (x != obj.x)
	{
		return x > obj.x;
	}
	else
	{
		if (y != obj.y)
		{
			return y > obj.y;
		}
		else
		{
			return z > obj.z;
		}
	}
}
bool vettore::operator==(const vettore obj) const{
	return (x == obj.x && y == obj.y && z == obj.z);
}
bool vettore::operator!=(const vettore obj) const
{
	return (x != obj.x or y != obj.y or z != obj.z);
}

string vettore::print() const{
	stringstream a;
	a << mod() << " (" << x << ", " << y << ", " << z << ") ";
	return a.str();
}
string vettore::print(string type) const{
	double r, t, p;
	sferic(r, t, p);
	stringstream a;
	if (type == "t" || type == "totale")
	{
		a << " (" << r << ", " << t * 180 / M_PI << "°, " << p * 180 / M_PI << "°)/(" << x << ", " << y << ", " << z << ") ";
	}
	else if (type == "s" || type == "standard")
	{
		a << r << " (" << x << ", " << y << ", " << z << ") ";
	}
	else if (type == "r" || type == "sferica")
	{
		a << " (" << r << ", " << t * 180 / M_PI << "°, " << p * 180 / M_PI << "°) ";
	}
	else if (type == "c" || type == "cartesiana")
	{
		a << " (" << x << ", " << y << ", " << z << ") ";
	}
	else if (type == "c_c" || type == "clean_cartesiana")
	{
		a << " " << x << ", " << y << ", " << z << " ";
	}
	return a.str();
}

double dist(const vettore a, const vettore b){
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

vettore dir(const vettore a, const vettore b){
	vettore r = b - a;
	return r / r.mod();
}

vettore dir(const vettore a){
	return a / a.mod();
}

vettore vettore::dir() const{
	return *this / mod();
}

vettore vettore::normalize(){
	*this = *this/mod();
	return *this;
}

vettore normalize(vettore v){
	v=v/v.mod();
	return v;
}