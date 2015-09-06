#include<iostream>
#include<vector>
#include<cstdlib>
using namespace std;

int main()
{
	double outer_radius = 13.0;
	double inner_radius = 7.0;

	double rad = 10.0;
	double d = 5.0;

	cout<<"# x y"<<endl; 
	for(int i=0;i<1000;i++)
	{
		for(int j=0;j<4;)
		{
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			r *= 13.0;
			r1 *= 13.0;

			double distance = (r*r) + (r1*r1);
			if( distance <= 169 && distance >= 49 )
			{
				switch(j)
				{
				case 0:
					//cout<<r<<" "<<r1<<endl;
					cout<<r<<" "<<r1<<" "<<"1"<<endl;
					break;
				case 1: 
					cout<<-r<<" "<<r1<<" "<<"1"<<endl;
					break;
				case 2:
					cout<<-r+rad<<" "<<-r1+d<<" 2"<<endl;
					break;
				case 3:
					cout<<r+rad<<" "<<-r1+d<<" 2"<<endl; 
					break;
				}
				j++;
			}
		}
		
	}
}
