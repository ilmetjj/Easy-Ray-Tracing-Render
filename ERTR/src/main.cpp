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
//	c.move_to(vettore(0,100,-100));
//	c.point_to(vettore(0,-15,15));

	scene A(c, move);

	sphere 
	s(4, vettore(100,200,0), 1, 0, 0, entity(vettore(0,0,15))),
	s2(2, vettore(189,50,110), 0, 1, 0, entity(vettore(5,2,15))),
	s3(1, vettore(25,148,187), 0, 1, 0, entity(vettore(7,4,17)));

	plane
	p(vettore(0,0,-1),vettore(0,0,0), 1, 0, 0, entity(vettore(0,0,100))),
	p2(vettore(0,1,0),vettore(109,87,123), 0, 1, 0, entity(vettore(0,-20,0)));

	l_point
	l(5,vettore(255,255,255),15,entity(vettore(10,10,15))),
	l1(5,vettore(255,0,0),5,entity(vettore(-50,50,0))),
	l2(5,vettore(0,255,0),5,entity(vettore(40,50,10))),
	l3(5,vettore(0,0,255),5,entity(vettore(-10,50,50)));

	A.add_obj(s);
	A.add_obj(s2);
	A.add_lig(l);
	A.add_obj(p);
	A.add_obj(s3);
	A.add_obj(p2);

	A.add_lig(l1);
	A.add_lig(l2);
	A.add_lig(l3);
/*
	sphere 
	s(4, vettore(255,100,255), 0, 1, 0, entity(vettore(0,-10,15)));

	plane
	p(vettore(0,0,-1),vettore(100,100,255), 0, 1, 0, entity(vettore(0,0,100))),
	p2(vettore(0,1,0),vettore(100,255,100), 0, 1, 0, entity(vettore(0,-15,0)));

	sun
	u2(vettore(0,1,0),vettore(230,255,127),3);

	l_point 
	u(5,vettore(100,100,100),5,entity(vettore(0,0,15)));

	A.add_obj(s);
	A.add_obj(p);
	A.add_obj(p2);

	A.add_lig(u);
	A.add_lig(u2);
*/
//	A.rend_term(200, 0.001, 3, 0);

	sun
	u2(vettore(0,1,0),vettore(230,255,127),1);
	
	A.add_lig(u2);
	
	system("mkdir rendering2");

//	int i=0;	
	for(float i=31; i<50; i+=1){
		string file="rendering2/image"+std::to_string(i)+".png", file_p="rendering2/image_p_"+std::to_string(i)+".png";
		A.rend_img(file, 10, i);
		A.rend_img_p(file_p, 10, i, 200, 2);
		cout<<i<<" "<<flush;
	}

	return 0;
}
