#include<iostream>
#include<string>
#include<vector>
#include<utility>
#include<random>
#include<cmath>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<map>
#include<algorithm>

using namespace std;


default_random_engine generator;
normal_distribution<double> distribution;
double a = 1.7159;
double b ; 

double threshold_percent = 0.45;

class Neuron{
	public:
	Neuron(int in_intralayer_index , int in_level_number , int in_layer_index);
	pair<int,int> neuron_id; //layer_number , intralayer_index
	//int layer_number;        //neuron layer number 
	int input_number;        //no of input nodes
        int output_number;       //no of output nodes
	int layer_index;         //layer index this neuron is a part of
	int next_layer_index;
	double current_output;
	int intralayer_index;
	double confidence;
	bool fired_flag;
	//vector<double> input_weight;
	vector<double> input_value;
	vector<double> input_value_node;
	vector<int> input_node;
	vector<double> output_weight;
	vector<int> output_node;
	int threshold;
	int initialize_weights();
	int calculate_output();
	double get_input_sum();
	int clear_input();
	int inhibitory_decrease(int min_confidence_neuron);
        int stimulatory_increase(int min_confidence_neuron);	
	int update_weight();
	double calculate_delta_change(double weight);
};

class Neural_layer
{
	public:
	Neural_layer(int in_level_number ,int first_neuron_no , int in_neuron_count , int in_layer_index );
	int make_connections();
	int neuron_count; //number of neuron in layer
	int first_neuron_no;
	int level_number; //layer of neuron
	int layer_index;
	int next_layer_index;
	vector<Neuron> inlayer_neuron;
	double avg_layer_confidence; 
	int calculate_output(int index);
	int clear_input();
	int update_weight();
	int new_stimulus_calculation();
};



class Network{
	public: 
	int number_of_level;
	int number_of_layers;
	int iter_layer_connection; 
	int total_points; 
	int change_required;  
	vector<pair<double,double>> coordinates;
	vector<int> coordinate_result; //2 types of coordinates 1 for first type and 2 for second type 
	vector<Neural_layer> neural_layer_vector;
	vector<vector<vector<int>>> fired_store; 
	Network();
	int start_stimulus();
	int start_stimulus_input(int i);
	int clear_input();
	int read_coordinates();
	int convert_number_into_neural_input(int number,vector<int> & neural_input);
	int big_controller(int input_index);
	int inhibition(int neuron_inhibit , int cur_layer , map<pair<int,int>,double> &inhibition_data);
        int inhibition_change_weight(map<pair<int,int>,double>inhibition_data,map<double,int>m_confidence);
	int stimulation(vector<int>v_stimulation_neuron , map<double,int> m_confidence);
        int get_prev_layer_neuron(int current_layer , int current_input_value_node , int &prev_layer , int &prev_layer_neuron);
	int get_inhibition_stimulation(int input_index ,vector<int> &v_inhibition_neuron,vector<int> &v_stimulation_neuron );
	int update_weight(int);
};

Network network;




Neuron::Neuron(int in_intralayer_index , int in_level_number , int in_layer_index )
:neuron_id(in_level_number ,in_intralayer_index),
layer_index(in_layer_index),
intralayer_index(in_intralayer_index),
confidence(0),
fired_flag(false)
{
	cout<<neuron_id.first<<","<<neuron_id.second<<endl; 
	//initialize_weights();	
}

double Neuron::get_input_sum()
{
	double ret_val = 0;
	int i_max = input_value.size();
	for(int i=0;i<i_max;i++)
	{
		ret_val += input_value[i];	
	}
	cout<<"Neuron layer: "<<layer_index<<" neuron no: "<<intralayer_index<<" input_val: "<<ret_val<<endl; 
	return ret_val; 

}
int Neuron::clear_input()
{
	input_value.clear();
	input_value_node.clear();	
	fired_flag = false;
	confidence = 0;
}

int Neuron::calculate_output()
{
	double v; 
	for(int i=0;i<input_value.size();i++)
	{
		v+=input_value[i];			
	}
	current_output = a*tanh(b*v);
	cout<<"Current output: "<<current_output<<endl; 
}

double Neuron::calculate_delta_change(double i)
{
	double delta = 3.0;
	double diff;
	
	i = i - 50.0;
	i = i*2;
	
	if(i < 0)
		i *= -1.0;


	if( i > 99)
		return 0.000000001;

	
	if(i<5)
		i=5; 
	//for(double i=0;i<100;i++)
	{
		double result = pow (i,1.0/1.8);
		double result2 = pow(i+delta,1.0/1.8);
		
		diff = result2 - result; 
		diff /= 2; //not done in single line 
		diff *= 10;		
	//	cout<<" diff: "<<diff<<endl; 
	}
	return diff;
}

int Neuron::update_weight()
{
	if(!fired_flag)
		return 0; 
	//FIX IT//	
/* this could offset the balance as all node might fire and we will increase all weights but hopefull it will follow its average and it will be fine */

	vector<int> decrease_wt_index;
	vector<double> weight_change;
	double total_increase = 0;
	for(int i=0;i<output_node.size();i++)
	{
		weight_change.push_back(calculate_delta_change(output_weight[i]));
		//cout<<"output_node: "<<output_node[i]<<" layer: "<<layer_index<<" min_confidence_neuron: "<<min_confidence_neuron<<endl;
		if(network.neural_layer_vector[next_layer_index].inlayer_neuron[output_node[i]].fired_flag )
		{
			cout<<" layer: "<<layer_index<<" intralayer: "<<intralayer_index<<" i:"<<output_node[i]<<" output_wt "<<output_weight[i]<<" + diff: "<<weight_change[i]<<endl;
			output_weight[i] = output_weight[i] + weight_change[i]/4.0;
			total_increase += output_weight[i];
		}
		else
		{
			cout<<" layer: "<<layer_index<<" intralayer: "<<intralayer_index<<" i:"<<output_node[i]<<" output_wt "<<output_weight[i]<<" - diff: "<<weight_change[i]<<endl;
			output_weight[i] = output_weight[i] - weight_change[i]/4.0;
			total_increase += output_weight[i];
		}
		
	}

	for(int i=0;i<output_node.size();i++)
	{
		output_weight[i] = (100/total_increase)*output_weight[i];
	}
	//decrease the remaining weights // did not add it here might need changing 
	
}



int Neuron::stimulatory_increase(int min_confidence_neuron)
{
	//increase the weigth of min_confidence_neuron link , decrease the weight of all other neuron

	cout<<"Increase for layer: "<<layer_index<<" neuron: "<<min_confidence_neuron<<endl; 	
	vector<double> weight_change;
	int increase_index;
	double sum_for_ratio = 0;  
	for(int i=0;i<output_node.size();i++)
	{
		weight_change.push_back(calculate_delta_change(output_weight[i]));
		//cout<<"output_node: "<<output_node[i]<<" layer: "<<layer_index<<" min_confidence_neuron: "<<min_confidence_neuron<<endl;
		cout<<"i:"<<i<<" output_wt "<<output_weight[i]<<" diff: "<<weight_change[i]<<endl;
		if( min_confidence_neuron == output_node[i])
		{
			increase_index = i;
			cout<<"increase_index "<<increase_index<<endl; 
		}
		else
		{
			sum_for_ratio += weight_change[i];
			//cout<<"sum: "<<sum_for_ratio<<endl;
		}
		
	}
	
	//increase the increase index by 2x
	//cout<<"prev weight: "<<	output_weight[increase_index];
	output_weight[increase_index] = output_weight[increase_index] + 2.0*weight_change[increase_index];
	//output_weight[increase_index] = output_weight[increase_index] + weight_change[increase_index];
	//cout<<" new weight: "<<	output_weight[increase_index]<<endl;


	//decrease the rest //working here 
	for(int i=0;i<output_node.size();i++)
	{
		if(i!=increase_index)
		{
			//cout<<"wt change: "<<weight_change[i]<<" prev weight: "<<	output_weight[i];
			output_weight[i] = output_weight[i] - (weight_change[i]/sum_for_ratio)*2.0*weight_change[increase_index];
			//output_weight[i] = output_weight[i] - (weight_change[i]/sum_for_ratio)*weight_change[increase_index];
			//cout<<" new weight: "<<	output_weight[i]<<endl;
		}
	}	
	
	//cout<<"Exit\n";		
	

}


int Neuron::inhibitory_decrease(int min_confidence_neuron) //TODO decide on how to increase or decrease weight
{
	//decrease the weigth of min_confidence_neuron link , increase the weight of all other neuron
	
	vector<double> weight_change;
	int decrease_index;
	double sum_for_ratio = 0;  
	for(int i=0;i<output_node.size();i++)
	{
		weight_change.push_back(calculate_delta_change(output_weight[i]));
		//cout<<"output_node: "<<output_node[i]<<" layer: "<<layer_index<<" min_confidence_neuron: "<<min_confidence_neuron<<endl;
		cout<<"i:"<<i<<" output_wt "<<output_weight[i]<<" diff: "<<weight_change[i]<<endl;
		if( min_confidence_neuron == output_node[i])
		{
			decrease_index = i;
			cout<<"decrease_index "<<decrease_index<<endl; 
		}
		else
		{
			sum_for_ratio += weight_change[i];
			//cout<<"sum: "<<sum_for_ratio<<endl;
		}
		
	}
	
	//decrease the decrease index by 2x
	//cout<<"prev weight: "<<	output_weight[decrease_index];
	output_weight[decrease_index] = output_weight[decrease_index] - 2.0*weight_change[decrease_index];
	//output_weight[decrease_index] = output_weight[decrease_index] - weight_change[decrease_index];
	//cout<<" new weight: "<<	output_weight[decrease_index]<<endl;


	//increase the rest //working here 
	for(int i=0;i<output_node.size();i++)
	{
		if(i!=decrease_index)
		{
			//cout<<"wt change: "<<weight_change[i]<<" prev weight: "<<	output_weight[i];
			output_weight[i] = output_weight[i] + (weight_change[i]/sum_for_ratio)*2.0*weight_change[decrease_index];
			//output_weight[i] = output_weight[i] + (weight_change[i]/sum_for_ratio)*weight_change[decrease_index];
			//cout<<" new weight: "<<	output_weight[i]<<endl;
		}
	}	
	
	//cout<<"Exit\n";		
	
}


Neural_layer::Neural_layer(int in_level_number ,int in_first_neuron_no ,int in_neuron_count,int in_layer_index)
:neuron_count(in_neuron_count),
 first_neuron_no(in_first_neuron_no),
 level_number(in_level_number),
 layer_index(in_layer_index)
{
	//Neural layer will decide which neurons to connect to. //first pass we only initialize the neurons , second pass we will connect the neurons to each other. 

	for(int i = 0; i < neuron_count ; i++)
	{
		cout<<"Constructor level: "<<level_number<<" intralayer_index: "<<i<<endl;
		Neuron neuron(i+first_neuron_no , level_number , layer_index);
		inlayer_neuron.push_back(neuron);	
	} 
}


int Neural_layer::clear_input()
{
	for(int i=0;i<inlayer_neuron.size();i++)
	{
		inlayer_neuron[i].clear_input();
	}
}

int Neural_layer::update_weight()
{
	for(int i=0;i<inlayer_neuron.size();i++)
	{
		inlayer_neuron[i].update_weight();	
	}	
}


int Neural_layer::new_stimulus_calculation()
{
	int lcon_inlayer_index = 9999;
	double lowest_confidence = 99999; 
	for( int i=0;i<inlayer_neuron.size();i++)
	{
		//find the neuron with lowest confidence
		if(inlayer_neuron[i].confidence < lowest_confidence && !inlayer_neuron[i].fired_flag)
		{
			lcon_inlayer_index = i;
			lowest_confidence = inlayer_neuron[i].confidence;// we have stored the lowest confidence	
		}
	}

	if(lcon_inlayer_index == 9999) //error scenario need to be fixed later
	{
		cout<<"SUNNY: ERROR couldn't find non firing neuron in lowest confidence loop \n";
	}


	for( int i=0;i<inlayer_neuron[lcon_inlayer_index].input_node.size();i++)
	{	
		//only trigger for those neuron that have not previously fired
		if(  find(inlayer_neuron[lcon_inlayer_index].input_value_node.begin(), inlayer_neuron[lcon_inlayer_index].input_value_node.end(), inlayer_neuron[lcon_inlayer_index].input_node[i] ) == inlayer_neuron[lcon_inlayer_index].input_value_node.end())
		{
		//	cout<<"SUNNY: ERROR another error \n";

		}
		else
		{
			//increase the value
			int prev_layer; 
			int prev_layer_neuron;
		
			network.get_prev_layer_neuron(layer_index , inlayer_neuron[lcon_inlayer_index].input_node[i],prev_layer,prev_layer_neuron);
			network.neural_layer_vector[prev_layer].inlayer_neuron[prev_layer_neuron].stimulatory_increase(lcon_inlayer_index);	
	
		}
		//find previous layer of the lowest confidence neuron

	}
		

}



Network::Network()
{
	number_of_level = 6;
	number_of_layers = 8;
	
	//first loop to initialize neurons //second loop to connect them	 
	for(int level_number=0;level_number<number_of_level;level_number++)
	{
		if( level_number == 0)
		{
			//input layer
			cout<<"Initialising level: "<<level_number<<"index 0"<<endl;
			Neural_layer neural_layer(level_number,0 , 16 , 0); //level 0 , layer 0 , no of neurons 16
			neural_layer_vector.push_back(neural_layer);

			cout<<"Initialising level: "<<level_number<<"index 1"<<endl;
			Neural_layer neural_layer1(level_number,16 , 16 , 1); //level 0 , layer 1 , no of neurons 16
			neural_layer_vector.push_back(neural_layer1);
		}
		else if(level_number ==1)
		{
			cout<<"Initialising level: "<<level_number<<"index 2"<<endl;
			Neural_layer neural_layer(level_number,0 , 16 , 2); //level 1 , layer 2 , no of neurons 16
			neural_layer_vector.push_back(neural_layer);

			cout<<"Initialising level: "<<level_number<<"index 3"<<endl;
			Neural_layer neural_layer1(level_number,16 , 16 ,3); //level 1 , layer 3 , no of neurons 16
			neural_layer_vector.push_back(neural_layer1);
		}
		else if(level_number==2)
		{
			cout<<"Initialising level: "<<level_number<<"index 4"<<endl;
			Neural_layer neural_layer(level_number,0 , 16 , 4); //level 2 , layer 4 , no of neurons 16
			neural_layer_vector.push_back(neural_layer);
		}
		else if(level_number==3)
		{
			cout<<"Initialising level: "<<level_number<<"index 5"<<endl;
			Neural_layer neural_layer(level_number,0 , 8 , 5); //level 3 , layer 5 , no of neurons 8
			neural_layer_vector.push_back(neural_layer);
		}
		else if(level_number==4)
		{
			cout<<"Initialising level: "<<level_number<<"index 5"<<endl;
			Neural_layer neural_layer(level_number,0 , 4 , 6); //level 4 , layer 6 , no of neurons 4
			neural_layer_vector.push_back(neural_layer);
		}
		else if(level_number == 5)
		{
			//output layer
			cout<<"Initialising level: "<<level_number<<"index 6"<<endl;
			Neural_layer neural_layer(level_number,0 , 2 , 7); //level 5 , layer 7 , no of neurons 2
			neural_layer_vector.push_back(neural_layer);
		}
		else
		{
			cout<<"Error\n";
		}
	}

	//second loop to connect them and set weights	
 
	//connection between layers 0 and 2   //threshold calculation for each level for layer one threshold is  
	for(int j=0;j<8;j++)
	{
		neural_layer_vector[0].next_layer_index = 2;
		for(int i=0;i<5;i++)
		{
			//first 7 neural connections
			neural_layer_vector[0].inlayer_neuron[j].output_node.push_back(j+i);
                        neural_layer_vector[0].inlayer_neuron[j].output_weight.push_back(20);
                        neural_layer_vector[0].inlayer_neuron[j].next_layer_index = 2;
                        neural_layer_vector[2].inlayer_neuron[j+i].input_node.push_back(j);
			neural_layer_vector[0].inlayer_neuron[j].threshold = threshold_percent * 100; 			
			cout<<"Connecting layer 0-2 with: "<<j<<","<<j+i<<endl; 

		}
		for(int i=0;i<5;i++)
		{
			//last 7 neural connections
			int revj = 15-j;
			neural_layer_vector[0].inlayer_neuron[revj].output_node.push_back(revj-i);
                        neural_layer_vector[0].inlayer_neuron[revj].output_weight.push_back(20);
                        neural_layer_vector[0].inlayer_neuron[revj].next_layer_index = 2;
                        neural_layer_vector[2].inlayer_neuron[revj-i].input_node.push_back(revj);
                        neural_layer_vector[0].inlayer_neuron[revj].threshold = threshold_percent * 100;
			cout<<"Connecting layer 0-2 with: "<<revj<<","<<revj-i<<endl; 
		}
	}

	//connection between layer 1 and 3  , neuron in layer 1 are from 14 to 28  , neuron in layer 3 are from 14 to 28
	for(int j=0;j<8;j++)
	{
		neural_layer_vector[1].next_layer_index = 3;
		for(int i=0;i<5;i++)
		{
			//first 7 neural connections
			cout<<"Connecting layer 1-3 with: "<<j<<","<<j+i+14<<endl; 
			neural_layer_vector[1].inlayer_neuron[j].output_node.push_back(j+i);
                        neural_layer_vector[1].inlayer_neuron[j].output_weight.push_back(20);
                        neural_layer_vector[1].inlayer_neuron[j].next_layer_index = 3;
                        neural_layer_vector[3].inlayer_neuron[j+i].input_node.push_back(j);
                        neural_layer_vector[1].inlayer_neuron[j].threshold = threshold_percent * 100 ;
			cout<<"Connecting layer 1-3 with: "<<j<<","<<j+i<<endl; 
		}
		for(int i=0;i<5;i++)
		{
			//last 7 neural connections
			int revj = 15-j;
			neural_layer_vector[1].inlayer_neuron[revj].output_node.push_back(revj-i);
                        neural_layer_vector[1].inlayer_neuron[revj].output_weight.push_back(20);
                        neural_layer_vector[1].inlayer_neuron[revj].next_layer_index = 3;
                        neural_layer_vector[3].inlayer_neuron[revj-i].input_node.push_back(revj);
                        neural_layer_vector[1].inlayer_neuron[revj].threshold = threshold_percent * 100 ;
			cout<<"Connecting layer 1-3 with: "<<revj<<","<<14+revj-i<<endl; 
		}
	}

	for(int layer_no = 2 ; layer_no <= 3; layer_no++)
	{
		neural_layer_vector[layer_no].next_layer_index = 4;
		//connection between layer 2 and 4   //connection between layers 3 and 4
		for(int j = 0 ; j<16;j++)
		{
			for(int i =0 ;i<5;i++)
			{	
				int random_no = rand()%16;
				int found = 0;
			        cout<<"connecting "<<layer_no <<" neuron: "<<j<<" the random number: "<<random_no<<endl;
				cout<<"Heres the random number: "<<random_no<<endl;
				for(int find_index = 0 ; find_index < neural_layer_vector[layer_no].inlayer_neuron[j].output_node.size() ; find_index++)
				{
					if( neural_layer_vector[layer_no].inlayer_neuron[j].output_node[find_index] == random_no)
					{
						neural_layer_vector[layer_no].inlayer_neuron[j].output_weight[find_index] += 20;
						found = 1;
						cout<<"Repeat for: "<<random_no<<endl;  
					}
				}	

				if( !found )
				{
					neural_layer_vector[layer_no].inlayer_neuron[j].output_node.push_back(random_no);
					neural_layer_vector[layer_no].inlayer_neuron[j].output_weight.push_back(20);
                                        neural_layer_vector[layer_no].inlayer_neuron[j].next_layer_index = 4;
					if(layer_no ==2)
					{
                                        	neural_layer_vector[4].inlayer_neuron[random_no].input_node.push_back(j);
					}
					else
					{
                                        	neural_layer_vector[4].inlayer_neuron[random_no].input_node.push_back(16+j);
					}
                                        neural_layer_vector[layer_no].inlayer_neuron[j].threshold = threshold_percent * 100;
				}
			}
			cout<<"Done for 1 neuron in layer: "<<layer_no<<endl ;
		}
	}
	//connection addition for layer 4 and 5

	for(int j=0;j<16;j++)
	{
		neural_layer_vector[4].next_layer_index = 5;
		for(int i = 0 ; i<4; i++) //only four connections here
		{
			int random_no = rand()%8;
			int found = 0;
			cout<<"connecting 4-5 neuron: "<<j<<" the random number: "<<random_no<<endl;
			for(int find_index = 0 ; find_index < neural_layer_vector[4].inlayer_neuron[j].output_node.size() ; find_index++)
			{
				if( neural_layer_vector[4].inlayer_neuron[j].output_node[find_index] == random_no)
				{
					neural_layer_vector[4].inlayer_neuron[j].output_weight[find_index] += 20;
					found = 1;
					cout<<"Repeat for: "<<random_no<<endl;  
				}
			}	

			if(!found )
			{
				neural_layer_vector[4].inlayer_neuron[j].output_node.push_back(random_no);
				neural_layer_vector[4].inlayer_neuron[j].output_weight.push_back(25);
				neural_layer_vector[4].inlayer_neuron[j].next_layer_index = 5;
                                neural_layer_vector[5].inlayer_neuron[random_no].input_node.push_back(j);
				neural_layer_vector[4].inlayer_neuron[j].threshold = threshold_percent * 200 ; //(16*5*20)/8
			}
		}
	}

	//connection addition for layer 5 and 6

	for(int j=0;j<8;j++)
	{
		neural_layer_vector[5].next_layer_index = 6;
		for(int i=0;i<3; i++)
		{
			int random_no = rand()%4;
			int found = 0;
			//cout<<"Heres the random number: "<<random_no<<endl;
			cout<<"connecting 5-6 neuron: "<<j<<" the random number: "<<random_no<<endl;
			for(int find_index = 0 ; find_index < neural_layer_vector[5].inlayer_neuron[j].output_node.size() ; find_index++)
			{
				if( neural_layer_vector[5].inlayer_neuron[j].output_node[find_index] == random_no)
				{
					neural_layer_vector[5].inlayer_neuron[j].output_weight[find_index] += 20;
					found = 1;
					cout<<"Repeat for: "<<random_no<<endl;  
				}
			}	

			if(!found )
			{

				double weight = 100.0/3.0;
				neural_layer_vector[5].inlayer_neuron[j].output_node.push_back(random_no);
				neural_layer_vector[5].inlayer_neuron[j].output_weight.push_back(weight);
				neural_layer_vector[5].inlayer_neuron[j].next_layer_index = 6;
                                neural_layer_vector[6].inlayer_neuron[random_no].input_node.push_back(j);
				neural_layer_vector[5].inlayer_neuron[j].threshold = threshold_percent * 200 ; //(16*4*25)/8
			}
		}
	}

	//connection addition for layer 6 and 7 (output)
	for(int j=0;j<4;j++)
	{
		neural_layer_vector[6].next_layer_index = 7;
		for(int i=0;i<2;i++)
		{
			neural_layer_vector[6].inlayer_neuron[j].output_node.push_back(i);
			neural_layer_vector[6].inlayer_neuron[j].output_weight.push_back(50);
			neural_layer_vector[6].inlayer_neuron[j].next_layer_index = 7;
			neural_layer_vector[6].inlayer_neuron[j].threshold = threshold_percent * 200; ;//(8*3*33.33)/4
		}	
	}
	
	//set threshold value of final layer
	for(int j=0;j<2;j++)
	{
		neural_layer_vector[7].inlayer_neuron[j].threshold = threshold_percent * 200; //(4*2*50)/2	
	}
}




int Network::read_coordinates()
{
	ifstream myfile ("points.dat");
	string line; 
	if(myfile.is_open())
	{
		while( getline(myfile,line))
		{
			double x , y; 
			int result;
			//cout<<"Sun1"<<line<<endl;
			stringstream ssline(line);
			ssline>>x>>y>>result;
			//cout<<x<<", "<<y<<endl;
			pair<double,double> input_coordinate(x,y);
			coordinates.push_back(input_coordinate); //change this  
			coordinates.push_back(input_coordinate); 
			coordinates.push_back(input_coordinate); 
	                coordinate_result.push_back(result); //2 types of coordinates 1 for first type and 2 for second type 
	                coordinate_result.push_back(result); //2 types of coordinates 1 for first type and 2 for second type 
	                coordinate_result.push_back(result); //2 types of coordinates 1 for first type and 2 for second type 
			 
		}
		myfile.close();
	}
	else
	{
		cout<<"Unable to openfile"<<endl; 
	}	
}


int Network::start_stimulus_input(int i)
{
	cout<<"Entering function start_stimulus_input"<<endl; 
	//transformation done here
	cout<<coordinates[i].first<<" "<<coordinates[i].second<<endl;
	coordinates[i].first += 15;
	coordinates[i].second += 10;

	int x = coordinates[i].first*1000.0;
	int y = coordinates[i].second*1000.0;

	cout<<x<<" "<<y<<endl; 
	vector<int> neural_input_x;
	vector<int> neural_input_y;
	convert_number_into_neural_input(x , neural_input_x); 
	convert_number_into_neural_input(y , neural_input_y); 

	int j = 0;
	for(vector<int>::iterator x_itr = neural_input_x.begin() ; x_itr != neural_input_x.end() ;j++, x_itr++)
	{
		cout<<*x_itr;
		//setinput in input node x will be input in layer 0
		if(*x_itr)
		{
			neural_layer_vector[0].inlayer_neuron[j].input_value.push_back(100);		
		}
		else
		{
			neural_layer_vector[0].inlayer_neuron[j].input_value.push_back(0);		
		}
	}
	cout<<endl;

	j=0;
	for(vector<int>::iterator y_itr = neural_input_y.begin() ; y_itr != neural_input_y.end() ; j++,y_itr++)
	{
		cout<<*y_itr;
		//setinput in input node y will be input in layer 1 
		if(*y_itr)
		{
			neural_layer_vector[1].inlayer_neuron[j].input_value.push_back(100);		
		}
		else
		{
			neural_layer_vector[1].inlayer_neuron[j].input_value.push_back(0);		
		}

	}
	cout<<endl;
}	


int  Network::convert_number_into_neural_input(int number , vector<int> & neural_input)
{
	//cout<<number<<endl; 
	int mask = 1;
	for(int i=0;i<16;i++)
	{
		int temp = number & mask;
		mask = mask<<1; 
		if(temp)
		{
			//cout<<"1"; 
			neural_input.push_back(1);
		}
		else 
		{
			//cout<<"0";
			neural_input.push_back(0);
		}	
	}
	//cout<<endl; 
}


int Network::start_stimulus()
{
   for(int k=0;k<1;k++)
   {
	for(int i=0; i<coordinates.size() ; i++)
	{
		start_stimulus_input( i);
		if( k==0)
		{
		vector<vector<int>> new_fired_store;
		fired_store.push_back(new_fired_store);
		}
		//start executing for each layer //here we are considering 100 as activation point
		for(int layer_no=0;layer_no<number_of_layers;layer_no++)
		{
			if(k==0)
			{
			vector<int> layer_fired_store;
			fired_store[i].push_back(layer_fired_store);
			}
			neural_layer_vector[layer_no].calculate_output(i);
			cout<<"________________________\n";	
		}
		cout<<"************************************************************************************\n";
		update_weight(0);
		big_controller(i); // it adjustes the weight accordingly 
		clear_input();	
	}
    }	
}

int Network::update_weight(int layer_number)
{
	for(int i=layer_number;i<number_of_layers;i++)
	{
		neural_layer_vector[i].update_weight();	
	}	
}


int Network::clear_input()
{
	for(int i=0;i<number_of_layers;i++)
	{
		neural_layer_vector[i].clear_input();
	}
}


int Neural_layer::calculate_output(int index) //index here is input index
{
	cout<<"In function calculate_output\n";
	avg_layer_confidence = 0;	
	for(int i=0;i<neuron_count;i++)
	{
		cout<<"calculate_output neuron_count: "<<neuron_count<<" i: "<<i<<endl; 
		double current_input = inlayer_neuron[i].get_input_sum(); //TODO
		cout<<"calculate_output current_input: "<<current_input<<" threshold: "<<inlayer_neuron[i].threshold<<endl; 
		//inlayer_neuron[i].threshold = 60;
		if(current_input >= inlayer_neuron[i].threshold)
		{

			// change confidence to a quadratic function  
			inlayer_neuron[i].confidence = current_input - inlayer_neuron[i].threshold;
			inlayer_neuron[i].confidence = inlayer_neuron[i].confidence / inlayer_neuron[i].threshold;
			inlayer_neuron[i].confidence *= 100.0;
			inlayer_neuron[i].confidence  = pow (inlayer_neuron[i].confidence,1.0/1.8);	
			//cout<<"Confidence print: "<<inlayer_neuron[i].confidence<<"result: "<<result<<endl;
		
			inlayer_neuron[i].fired_flag = true;
			if(layer_index == 7)
			{
				cout<<"SUNNY weight: "<<current_input<<endl;
				network.fired_store[index][layer_index].push_back(i);
			}
			else
			{

				int j_max = inlayer_neuron[i].output_node.size();
				for(int j = 0;j<j_max;j++)
				{
					//fire and calculate output
					double weight = inlayer_neuron[i].output_weight[j]; 
					int output_node_local = inlayer_neuron[i].output_node[j]; 
					network.neural_layer_vector[next_layer_index].inlayer_neuron[output_node_local].input_value.push_back(weight);
					if( layer_index != 3)
					{
						network.neural_layer_vector[next_layer_index].inlayer_neuron[output_node_local].input_value_node.push_back(i);
					}
					else
					{
						network.neural_layer_vector[next_layer_index].inlayer_neuron[output_node_local].input_value_node.push_back(i+16);
					}
				}
				network.fired_store[index][layer_index].push_back(i);
			}

		}
		else
		{
			inlayer_neuron[i].confidence = inlayer_neuron[i].threshold - current_input;
			inlayer_neuron[i].confidence = inlayer_neuron[i].confidence / inlayer_neuron[i].threshold;
			inlayer_neuron[i].confidence *= 100.0;
			inlayer_neuron[i].confidence  = pow (inlayer_neuron[i].confidence,1.0/1.8);	
			//cout<<"Confidence print: "<<inlayer_neuron[i].confidence<<endl;
		}	
		//clear neuron input //or you could do it else where also but right now do it here
		avg_layer_confidence += inlayer_neuron[i].confidence; 
		//inlayer_neuron[i].input_value.clear();
		//inlayer_neuron[i].input_node.clear();	
	}
	cout<<" "<<avg_layer_confidence<<endl;
	avg_layer_confidence = avg_layer_confidence/neuron_count; 
}

int Network::get_inhibition_stimulation(int input_index ,vector<int> &v_inhibition_neuron,vector<int> &v_stimulation_neuron)
{
	v_inhibition_neuron.clear();
	v_stimulation_neuron.clear();
	int result = coordinate_result[input_index]; 
	//get result
	if( result == 1 )//neuron 0 should fire
	{
		if( find(fired_store[input_index][7].begin() , fired_store[input_index][7].end() , 0 ) == fired_store[input_index][7].end() )
		{
			//neuron did not fire
			v_stimulation_neuron.push_back(0);
		}

		if( find(fired_store[input_index][7].begin() , fired_store[input_index][7].end() , 1 ) != fired_store[input_index][7].end() )
		{
			//wrong neuron fired
			v_inhibition_neuron.push_back(1);
		}
	}
	if( result == 2 )//neuron 1 should fire
	{
		if( find(fired_store[input_index][7].begin() , fired_store[input_index][7].end() , 1 ) == fired_store[input_index][7].end() )
		{
			//neuron did not fire
			v_stimulation_neuron.push_back(1);
		}

		if( find(fired_store[input_index][7].begin() , fired_store[input_index][7].end() , 0 ) != fired_store[input_index][7].end() )
		{
			//wrong neuron fired
			v_inhibition_neuron.push_back(0);
		}
	}


}

int Network::big_controller(int input_index)
{
	//analyize the nodes and compare it with output
	//int result = coordinate_result[i];
	//print the value of stored

	total_points++;
	bool change_required_flag = false;
	map<double,int> m_confidence; 
	int result = coordinate_result[input_index]; 
	for(int i=0;i<number_of_layers;i++)
	{
		cout<<"For layer "<<i<<" : ";
		for(int j=0;j<fired_store[input_index][i].size();j++)
		{
			cout<<fired_store[input_index][i][j]<<" ";
		}

		//find the confidence level of each layer
		cout<<" Confidence: "<<neural_layer_vector[i].avg_layer_confidence<<endl;

		//if( ( input_index < 500 || i != 7) && i != 0 && i != 1)

		if( i != 0 && i != 1)
		{
			if( m_confidence[neural_layer_vector[i].avg_layer_confidence] == 0)
			{
				m_confidence[neural_layer_vector[i].avg_layer_confidence] = i;  //check for duplicates here 
				//cout<<"Entering: "<<neural_layer_vector[i].avg_layer_confidence<<" = "<< i<<endl;  //check for duplicates here 
			}
			else
			{
				m_confidence[neural_layer_vector[i].avg_layer_confidence + 0.000001] = i;  //check for duplicates here 
				//cout<<"Entering: "<<neural_layer_vector[i].avg_layer_confidence + 0.000001<<" = "<< i<<endl;  //check for duplicates here 
			}
		}

	} 	

	for(map<double,int>::iterator itr = m_confidence.begin() ; itr != m_confidence.end() ;itr++)
	{
		cout<<"value: "<<itr->first<<" layer: "<<itr->second<<endl;
	}	

	//find out the current output and trace inhibition and stimulation to lowest confidence layer
	// if possible trace it to the end , find neurons with low confidence witin this walk and change it value

	vector<int> v_inhibition_neuron;
	vector<int> v_stimulation_neuron;
	get_inhibition_stimulation(input_index,v_inhibition_neuron , v_stimulation_neuron);	
	bool inhibition_completed = false;
	for(int i=0;i< 50 && (v_inhibition_neuron.size() || v_stimulation_neuron.size()) ;i++) //TODO change this number //loop for inhibition get this right then loop for stimulation
	{
		if( change_required_flag == false)
			change_required++;
		change_required_flag = true;

		while( v_inhibition_neuron.size() )
		{
			//implement inhibition //need to better this area
			map<pair<int,int>,double> inhibition_data;
			for(int j = 0;j<v_inhibition_neuron.size();j++)
			{
				inhibition(v_inhibition_neuron[j],7,inhibition_data);
			}

			for(map<pair<int,int>,double>::iterator itr = inhibition_data.begin() ; itr != inhibition_data.end(); itr++ )
			{
				cout<<"layer: "<<itr->first.first<<" neuron: "<<itr->first.second<<" confidence: "<<itr->second<<endl;
			}

			//inhibition weight changes	
			//TEMP change weight of lowest confidence to not fire //need more versatility in this function
			//idealy would have wanted to test previous values as well to see if they are still working. 

			inhibition_change_weight(inhibition_data,m_confidence);

			//run it again and see if we get the desired result of not


			//start executing for each layer //here we are considering 100 as activation point
			
			fired_store[input_index][m_confidence.begin()->second-1].clear();
			for(int layer_no=m_confidence.begin()->second;layer_no<number_of_layers;layer_no++)
			{
				//vector<int> layer_fired_store;
				//fired_store[i].push_back(layer_fired_store);
				fired_store[input_index][layer_no].clear();
				neural_layer_vector[layer_no].clear_input();
			}


			for(int layer_no=m_confidence.begin()->second-1;layer_no<number_of_layers;layer_no++)
			{
				//vector<int> layer_fired_store;
				neural_layer_vector[layer_no].calculate_output(input_index);
			}

			for(int layer_no=m_confidence.begin()->second-1;layer_no<number_of_layers;layer_no++) //hack for number_of_layers -1*

			{
				//vector<int> layer_fired_store;
				neural_layer_vector[layer_no].update_weight();
			}

			m_confidence.clear(); //clear previour confidence
			for(int k=0;k<number_of_layers;k++)
			{
				cout<<"For layer "<<k<<" : ";
				for(int j=0;j<fired_store[input_index][k].size();j++)
				{
					cout<<fired_store[input_index][k][j]<<" ";
				}
				//find the confidence level of each layer
				cout<<" Confidence: "<<neural_layer_vector[k].avg_layer_confidence<<endl;
				//if( ( input_index < 500 || k != 7) && k != 0 && k != 1)
				if( k != 0 && k != 1)
				{
					if( m_confidence[neural_layer_vector[k].avg_layer_confidence] == 0)
					{
						m_confidence[neural_layer_vector[k].avg_layer_confidence] = k;  //check for duplicates here 
					}
					else
					{

						m_confidence[neural_layer_vector[k].avg_layer_confidence + 0.000001] = k;  //check for duplicates here 

					}
				}

			}

 			for(map<double,int>::iterator itr = m_confidence.begin() ; itr != m_confidence.end() ;itr++)
			{
				cout<<"inhibition value: "<<itr->first<<" layer: "<<itr->second<<endl;
			}	


			get_inhibition_stimulation(input_index,v_inhibition_neuron , v_stimulation_neuron);

		}
	
		if(v_stimulation_neuron.size())
		{
			//randomly stimulate neurons needing the least stimulus
			neural_layer_vector[m_confidence.begin()->second].new_stimulus_calculation();

			//start executing for each layer //here we are considering 100 as activation point
			
			fired_store[input_index][m_confidence.begin()->second-1].clear();
			for(int layer_no=m_confidence.begin()->second;layer_no<number_of_layers;layer_no++)
			{
				//vector<int> layer_fired_store;
				//fired_store[i].push_back(layer_fired_store);
				fired_store[input_index][layer_no].clear();
				neural_layer_vector[layer_no].clear_input();
			}


			for(int layer_no=m_confidence.begin()->second-1;layer_no<number_of_layers;layer_no++)
			{
				//vector<int> layer_fired_store;
				neural_layer_vector[layer_no].calculate_output(input_index);
			}

			for(int layer_no=m_confidence.begin()->second-1;layer_no<number_of_layers;layer_no++) //hack for number_of_layers -1*
			{
				//vector<int> layer_fired_store;
				neural_layer_vector[layer_no].update_weight();
			}

			m_confidence.clear(); //clear previour confidence
			for(int k=0;k<number_of_layers;k++)
			{
				cout<<"Stimulation: For layer "<<k<<" : ";
				for(int j=0;j<fired_store[input_index][k].size();j++)
				{
					cout<<fired_store[input_index][k][j]<<" ";
				}
				//find the confidence level of each layer
				cout<<"Stimulation:  Confidence: "<<neural_layer_vector[k].avg_layer_confidence<<endl;
				//if( ( input_index < 500 || k != 7) && k != 0 && k != 1)
				if(  k != 0 && k != 1)
				{
					if( m_confidence[neural_layer_vector[k].avg_layer_confidence] == 0)
					{

						m_confidence[neural_layer_vector[k].avg_layer_confidence] = k;  //check for duplicates here 
					}
					else
					{

						m_confidence[neural_layer_vector[k].avg_layer_confidence + 0.000001] = k;  //check for duplicates here 

					}
				}

			} 
 			for(map<double,int>::iterator itr = m_confidence.begin() ; itr != m_confidence.end() ;itr++)
			{
				cout<<"Stimulation value: "<<itr->first<<" layer: "<<itr->second<<endl;
			}	


			get_inhibition_stimulation(input_index,v_inhibition_neuron , v_stimulation_neuron);

		}

	}

	cout<<"Report Total: "<<total_points<<" Correct: "<<total_points-change_required<<endl; 
	cout<<"*******************GOOD VALUE****************\n";
}

int Network::inhibition_change_weight(map<pair<int,int>,double>inhibition_data,map<double,int>m_confidence  )
{
	//only one neuron is affecting change of weight , and that too in lowest confidence layer //need to change it so that change is done in multiple layer
	//TODO
	//find common neuron in layer and
	int lowest_confidence_layer = m_confidence.begin()->second;
	int min_confidence_neuron = 9999;
	double min_confidence = 9999;
	for(map<pair<int,int>,double>::iterator itr = inhibition_data.begin() ; itr != inhibition_data.end(); itr++ )
	{
		cout<<"layers: "<<itr->first.first<<" low_conf"<<lowest_confidence_layer<<endl;
		if(itr->first.first == lowest_confidence_layer )
		{
			if(itr->second < min_confidence)
			{
				min_confidence = itr->second; 
				min_confidence_neuron = itr->first.second;	 
			}
		}	
	}

	cout<<"Min confidence neuron: "<<min_confidence_neuron<<" layer: "<<lowest_confidence_layer<<" confidence: "<<min_confidence<<endl;
	
	//find out the neurons firing this neuron , 
	for(int i = 0 ; i < neural_layer_vector[lowest_confidence_layer].inlayer_neuron[min_confidence_neuron].input_value_node.size(); i++)
	{
		int prev_layer;
		int prev_layer_neuron; 
		get_prev_layer_neuron(lowest_confidence_layer , neural_layer_vector[lowest_confidence_layer].inlayer_neuron[min_confidence_neuron].input_value_node[i],
		prev_layer,prev_layer_neuron);
		cout<<"prev_layer: "<<prev_layer<<" prev_layer_neuron: "<<prev_layer_neuron<<" min_confidence_neuron: "<<min_confidence_neuron<<endl; 
		neural_layer_vector[prev_layer].inlayer_neuron[prev_layer_neuron].inhibitory_decrease(min_confidence_neuron);		
	}
		
}


int Network::get_prev_layer_neuron(int current_layer , int current_input_value_node , int &prev_layer , int &prev_layer_neuron)
{
		if( current_layer == 7)
		{
			prev_layer = 6;
			prev_layer_neuron = current_input_value_node;	
		}
		if( current_layer == 6)
		{
			prev_layer = 5;
			prev_layer_neuron = current_input_value_node;	
		}
		if( current_layer == 5)
		{
			prev_layer = 4;
			prev_layer_neuron = current_input_value_node;	
		}
		if( current_layer == 4)
		{
			if(current_input_value_node < 16)
			{
				prev_layer = 2;
				prev_layer_neuron = current_input_value_node;	
			}
			else
			{
				prev_layer = 3;
				prev_layer_neuron = current_input_value_node-16;	
			}
		}
		if(current_layer == 3)
		{
			prev_layer = 1;
			prev_layer_neuron = current_input_value_node;	
		}
		if(current_layer == 2)
		{
			prev_layer = 0;
			prev_layer_neuron = current_input_value_node;	
		}

}



int Network::inhibition(int neuron_inhibit , int layer , map<pair<int,int>,double> &inhibition_data ) //inhibition data(layer_no,inlayer_neuron)
{
	//inhibition neuron will be in layer 7 , we will go back from here and trace till low confidence layer

	if(layer < 2)
		return 0; 

	int neuron_toinhibit = neuron_inhibit;

	pair<int,int> new_pair(layer,neuron_toinhibit);
	inhibition_data[new_pair]= neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].confidence ;
	//cout<<"Neuron to inhibit: "<<neuron_toinhibit<<" layer: "<<layer<<" confidence: "<<inhibition_data[new_pair]<<endl;

	for(int j=0 ; j<neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node.size(); j++)
	{
		//cout<<"SIZE "<<neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node.size()<<endl; 
		if( layer == 7)
		{
			inhibition(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j],6,inhibition_data);	
		}
		if( layer == 6)
		{
			inhibition(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j],5,inhibition_data)	;
		}
		if( layer == 5)
		{
			inhibition(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j],4,inhibition_data);	
		}
		if( layer == 4)
		{
			if(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j] < 16)
			{
				inhibition(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j],2,inhibition_data)	;
			}
			else
			{
				inhibition(neural_layer_vector[layer].inlayer_neuron[neuron_toinhibit].input_value_node[j]-16,3,inhibition_data)	;
			}
		}
	}	

}

int Network::stimulation(vector<int>v_stimulation_neuron , map<double,int> m_confidence)
{

	int low_confidence_no = m_confidence.begin()->second; 
	cout<<"Low confidence layer: "<<low_confidence_no<<endl; 


	cout<<"In function stimulation\n";
}


int main()
{
	network.read_coordinates();
	network.start_stimulus();	
}
