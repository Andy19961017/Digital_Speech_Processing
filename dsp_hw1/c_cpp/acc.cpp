#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
	fstream x;
	x.open(argv[1], ios::in);
	fstream y;
	y.open(argv[2], ios::in);
	unsigned wrong=0;
	for (int i = 0; i < 2500; ++i){
		string xs;
		getline(x,xs);
		string ys;
		getline(y,ys);
		if (xs[7]!=ys[7]) wrong++;
	}
	cout << "Accuracy: " << 1-(wrong/2500.0) << endl;
}