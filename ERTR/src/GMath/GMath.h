#ifndef __GMATH_H__
#define __GMATH_H__

using namespace std;

class vettore;

#include <cmath>
#include <sstream>
#include <string>

template <typename T> 
int sgn(T val) {
	if(val<0)
		return -1;
	else
		return 1;
}

class vettore
{
	double x, y, z;

public:

	vettore(double=0, double=0, double=0);

	double mod() const;

	void sferic(double &r, double &t, double &p) const;

	double get_x()const ;
	double get_y()const ;
	double get_z()const ;
	
	vettore operator+(const vettore obj) const;
	void operator+=(const vettore obj);
	vettore operator-(const vettore obj) const;
	void operator-=(const vettore obj);
	vettore operator*(double n) const;
	void operator*=(double n);
	friend vettore operator*(double n, const vettore obj);
	vettore operator/(double n) const;
	void operator/=(double n);
	double operator*(const vettore obj) const;
	vettore operator%(const vettore obj) const;
	vettore operator*(const vettore v[3])const;

	vettore per(vettore obj);

	friend ostream &operator<<(ostream &os, const vettore &obj);
	friend istream &operator>>(istream &is, vettore &obj);

	bool operator<(const vettore obj) const;
	bool operator>(const vettore obj) const;
	bool operator==(const vettore obj) const;
	bool operator!=(const vettore obj) const;

	string print() const;
	string print(string type) const;

	friend double dist(const vettore a, const vettore b);
	friend vettore dir(const vettore a, const vettore b);
	friend vettore dir(const vettore a);

	vettore dir() const;
	vettore normalize();
	friend vettore normalize(vettore);
};

#endif