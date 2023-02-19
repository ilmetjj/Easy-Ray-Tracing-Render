#include <iostream>
#include <cmath>
#include "rend/rend.h"
#include "GMath/GMath.h"
#include <string>
#include <cstring>

using namespace std;

void move(camera &cam, vector<object *> &obj, vector<light *> &lig, double n)
{
	vettore m(sin(n * 7) * 35, sin(n * 7) * 15, 5 + cos(n * 7) * 35);
	lig[0]->move_to(m);

	vettore m2(cos(-n * 14) * 10, sin(-n * 14) - 1, 15 + sin(-n * 14) * 10);
	obj[1]->move_to(m2);

	vettore m3(30*sin(n*5),20*cos(n*5),-100);
	cam.move_to(m3);
	cam.point_to(obj[0]->get_pos());
}

int main(/*int argc, char** argv*/){

	camera c(38.4,21.6,100);

	scene A(c, move);

	sphere 
	s(4, vettore(0,0,0), 1, 0, 0, entity(vettore(0,0,15))),
	s2(2, vettore(189,50,110), 0, 1, 0, entity(vettore(5,2,15))),
	s3(1, vettore(25,148,187), 0, 1, 0, entity(vettore(7,4,17)));

	plane
	p(vettore(0,0,-1),vettore(0,0,0), 1, 0, 0, entity(vettore(0,0,100))),
	p2(vettore(0,1,0),vettore(109,87,123), 0, 1, 0, entity(vettore(0,-20,0)));

	l_point
	l(1,vettore(255,255,255),2.5,entity(vettore(10,10,15))),
	l1(1,vettore(255,0,0),1.5,entity(vettore(-50,50,0))),
	l2(1,vettore(0,255,0),1.5,entity(vettore(40,50,10))),
	l3(1,vettore(0,0,255),1.5,entity(vettore(-10,50,50)));

	A.add_obj(s);
	A.add_obj(s2);
	A.add_lig(l);
	A.add_obj(p);
	A.add_obj(s3);
	A.add_obj(p2);

	A.add_lig(l1);
	A.add_lig(l2);
	A.add_lig(l3);
	
	A.rend_term(200, 0.001, 3, 0);
	
	system("mkdir rendering2");
	
	for(float i=0; i<M_PI; i+=0.05){
		string file="rendering2/image"+std::to_string(i)+".png";
		A.rend_img(file, 200, i);
		cout<<i<<" "<<flush;
	}

	return 0;
}
