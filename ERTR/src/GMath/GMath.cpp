#include "GMath.h"

using namespace std;

/*vettore*/

vettore::vettore(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

double vettore::mod() const
{
	double m;
	m = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	return m;
}

void vettore::sferic(double &r, double &t, double &p) const
{
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

vettore vettore::operator+(const vettore obj) const
{
	return vettore(x + obj.x, y + obj.y, z + obj.z);
}
vettore vettore::operator-(const vettore obj) const
{
	return vettore(x - obj.x, y - obj.y, z - obj.z);
}
vettore vettore::operator*(double n) const
{
	return vettore(n * x, n * y, n * z);
}
vettore operator*(double n, const vettore obj)
{
	return vettore(n * obj.x, n * obj.y, n * obj.z);
}
vettore vettore::operator/(double n) const
{
	return vettore(x / n, y / n, z / n);
}
double vettore::operator*(const vettore obj) const
{
	return x * obj.x + y * obj.y + z * obj.z;
}
vettore vettore::operator%(const vettore obj) const
{
	return vettore((y * obj.z - z * obj.y), (z * obj.x - x * obj.z), (x * obj.y - y * obj.x));
}

vettore vettore::per(vettore obj){
	return vettore(x*obj.x,y*obj.y,z*obj.z);
}

ostream &operator<<(ostream &os, const vettore &obj)
{
	os << obj.mod();
	return os;
}
istream &operator>>(istream &is, vettore &obj)
{
	double _x, _y, _z;
	is >> _x >> _y >> _z;
	obj.x = _x;
	obj.y = _y;
	obj.z = _z;
	return is;
}

bool vettore::operator<(const vettore obj) const
{
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
bool vettore::operator>(const vettore obj) const
{
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
bool vettore::operator==(const vettore obj) const
{
	return (x == obj.x && y == obj.y && z == obj.z);
}

string vettore::print() const
{
	stringstream a;
	a << mod() << " (" << x << ", " << y << ", " << z << ") ";
	return a.str();
}
string vettore::print(string type) const
{
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

double dist(const vettore a, const vettore b)
{
	double r;
	r = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	return r;
}

vettore dir(const vettore a, const vettore b)
{
	vettore r;
	r = b - a;
	return r / r.mod();
}

vettore dir(const vettore a)
{
	return a / a.mod();
}

vettore vettore::dir() const
{
	return *this / mod();
}

vettore vettore::normalize(){
	*this = dir();
	return *this;
}
