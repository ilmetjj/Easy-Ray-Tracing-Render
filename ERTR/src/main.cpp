#include <iostream>
#include <cmath>
#include "rend/rend.h"
#include "GMath/GMath.h"
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char** argv){

	camera c(vettore(0,0,-100), 38.4,21.6);

	scene A(c);

	double n=3.25;

	sphere s(vettore(0, 0, 15), 4, 1), s2(vettore(5, 2, 15), 2, 0, vettore(189,50,110)), s3(vettore(7, 4, 17), 1, 0, vettore(25,148,187));
	light l(vettore(10, 10, 15), 2, vettore(255,255,255));
	plane p(vettore(0,0,100),vettore(0,0,-1),1), p2(vettore(0,-20,0), vettore(0,1,0));

	A.add_obj(s);
	A.add_obj(s2);
	A.add_lig(l);
	A.add_obj(p);
	A.add_obj(s3);
	A.add_obj(p2);

	light l1(vettore(-50, 50, 0), 1, vettore(255, 0, 0)), l2(vettore(40, 50, 10), 1, vettore(0, 255, 0)), l3(vettore(-10, 50, 50), 1, vettore(0, 0, 255));
	A.add_lig(l1);
	A.add_lig(l2);
	A.add_lig(l3);

//	A.render();
	system("mkdir rendering2");
	for(float i=0; i<M_PI; i+=0.05){
		string file="rendering2/image"+std::to_string(i)+".png";
		A.rend_img(file, 50, i);
	}

	return 0;
}
