#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>  // setw
#include <fstream>
#include <functional>
#include <sstream>
#include<stdlib.h>
#include<algorithm>
#include<stdio.h>
#include <string>
#include <vector>
#include<ctype.h>
#include <memory>
#include<set>
#include<map>
#include <chrono>
#include <random>
#include "Alternative.cpp"
using namespace std;
unsigned seed;
std::default_random_engine generator;
std::extreme_value_distribution<double> gumbel; 

vector<double> createroulette(int nb_days)
{
	vector<double> weights;
	double indweights = 1.0 / double(nb_days);
	double sumweights = 0.0;
	for (int i = 0; i < nb_days-1; i++)
	{
		sumweights += indweights;
		weights.push_back(sumweights);
	}
	weights.push_back(1.0);
	return weights;
}

int draw(vector<double> & roulette, vector<int> & days_chosen, vector<int> & days_tobechoose)
{
	bool check = false;
	int final_int;
	while (check == false)
	{
		bool check2 = false;
		int test_int = 0;
		double rdm = (double)rand() / ((double)RAND_MAX + 1);
		int numb = 0;
		while (check2 == false)
		{
			if (rdm <= roulette.at(numb))
			{
				test_int = numb;
				check2 = true;
			}
			numb++;
		}
		if (days_chosen.size() == 0)
		{
			check = true;
			final_int = test_int;
		}
		for (int i = 0; i < days_chosen.size(); i++)
		{
			if (test_int == days_chosen[i])
				break;
			if (test_int != days_chosen[i] && i == days_chosen.size()-1)
			{
				check = true;
				final_int = test_int;
			}
		}
	}
	return final_int;
}

vector<int> choosedays(int nb_days,int timehorizon)
{
	vector<double> weights = createroulette(timehorizon);
	vector<int> days_chosen;
	vector<int> days_tobechoose;
	for (int i = 0; i < timehorizon; i++)
	{
		days_tobechoose.push_back(i);
	}
	for (int i = 0; i < nb_days; i++)
	{
		int drew = draw(weights, days_chosen, days_tobechoose);
		days_chosen.push_back(drew);
	}
	return days_chosen;
}

void makechoice(vector<Alternative> & alternatives, vector<double> & betas, vector<Alternative> & chosen)
{
	double bestutility = betas.back();
	int bestday = -1;
	int besti = 0;
	for (int i = 0; i < alternatives.size(); i++)
	{
		int d = alternatives[i].day;
		while (d > 5)
			d -= 5;
		double utility = betas.back();
		double vrng = gumbel(generator);
		if (alternatives[i].day != -1)
		{

			utility = betas.back() + betas[d] + betas[betas.size() - 2] * alternatives[i].price + vrng;
		}
		else
			utility = vrng;
		if (utility > bestutility)
		{
			besti = i;
			bestday = alternatives[i].day;
			bestutility = utility;
		}
	}

	chosen = alternatives;
	if (bestday == -1)
		besti = alternatives.size() - 1;
	std::swap(chosen[0], chosen[besti]);
}
int main()
{
	seed = 0;// std::chrono::steady_clock::now().time_since_epoch().count();
	generator = std::default_random_engine(seed);
	gumbel = std::extreme_value_distribution<double>(1.0, pow(3.1415926,2)/6);
	srand(seed);
	//We define the parameters and initialise vectors (setup)
	int Number_of_historics = 1; int Nb_Cust = 10000; int nb_Segments = 1;
	
	double beta_a = 2.00; double beta_e = 3.0; double beta_b = 1.5; 
	double beta_c = 1.5; double beta_d = 2.00; double beta_p = -1.0;
	double beta_0 = 1.0;
	int horizon_temp = 5;
	vector<double> vbetas;
	vbetas.push_back(beta_a);
	vbetas.push_back(beta_b);
	vbetas.push_back(beta_c);
	vbetas.push_back(beta_d);
	vbetas.push_back(beta_e);
	vbetas.push_back(beta_p);
	vbetas.push_back(beta_0);
	

	//We iterate on the number of instances we want for the parameters defined
	for (int i = 1; i <= Number_of_historics; i++)
	{

		//We instantiate the instances
		char Instance_Name[50];
		char Size_Name[50];
		sprintf(Instance_Name, "Data_%d%d.txt", i,Nb_Cust);
		sprintf(Size_Name, "Size_%d%d.txt", i, Nb_Cust);
		std::ofstream Instance(Instance_Name);
		std::ofstream Size(Size_Name);

		//We iterate on the number of customers in each instance
		for (int j = 1; j <= Nb_Cust; j++)
		{
			int days_toassign = 5;

			//We select a number of days that will be assigned.
			//days_toassign <= horizon_temp so if days_toassign = 3, 
			//then the vector of options could be 1,3,5 wich would 
			//mean monday,wednesday,friday. 
			vector<Alternative> alternatives;
			vector<int> days_chosen = choosedays(days_toassign, horizon_temp);

			//We add all the options to a vector of options
			for (int k = 0; k < days_chosen.size(); k++)
			{
				Alternative alt;
				alt.assignday(days_toassign, days_chosen[k]);
				alternatives.push_back(alt);
			}
			Alternative alt;
			alt.assignday(days_toassign, -1);
			alternatives.push_back(alt);

			//We reorder the vector of options so the choice of the customer is first
			vector<Alternative> alternatives_ordered;
			makechoice(alternatives, vbetas, alternatives_ordered);

			//We put the options in the format wanted 
			for (int k = 0; k < alternatives_ordered.size(); k++)
			{
				string str = to_string(alternatives_ordered[k].attributs.at(0).valeur);
				for (int l = 1; l < alternatives_ordered[k].attributs.size(); l++)
				{
					str = str + "," + to_string(alternatives_ordered[k].attributs.at(l).valeur);
				}
				str = str + "," + to_string(alternatives_ordered[k].price);
				Instance << str << endl;
			}
			Size << days_toassign + 1 << "," << 1 << endl;
		}
	}
	cout << "Finished" << endl;
	cin.get();
}
