#include<iostream>
#include<cmath>

using namespace std;

double calculate(double i)
{
	double delta = 3.0;
	double diff;
	
	i = i - 50.0;
	i = i*2;
	
	if(i<5)
		i=5; 
	//for(double i=0;i<100;i++)
	{
		double result = pow (i,1.0/1.8);
		double result2 = pow(i+delta,1.0/1.8);
		
		diff = result2 - result; 
		diff /= 2; 
		diff *= 10;		
		cout<<" diff: "<<diff<<endl; 
	}
	return diff;
}


int main()
{
	double retval = 0;
	double initial_val = 50;
	for(int i=0;i<50;i++)
	{
		cout<<"retval: "<<initial_val;	
		retval = calculate(initial_val);
		initial_val += retval;
		 
	}
}
