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

int draw(vector<double> roulette, vector<int> days_chosen, vector<int> days_tobechoose)
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
			if (test_int == days_chosen.at(i))
				break;
			if (test_int != days_chosen.at(i) && i == days_chosen.size()-1)
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

vector<Alternative> makechoice(vector<Alternative> alternatives, vector<double> betas, extreme_value_distribution<double> gumbel)
{
	double bestutility = betas.at(betas.size()-1);
	int bestday = -1;
	int besti = 0;
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	for (int i = 0; i < alternatives.size(); i++)
	{
		int d = alternatives.at(i).day;
		while (d > 5)
			d -= 5;
		double utility = betas.at(betas.size() - 1);
		double a = betas.at(betas.size() - 2)*alternatives.at(i).price;
		double b = gumbel(generator);
		if (alternatives.at(i).day != -1)
			utility = betas.at(betas.size() - 1) + betas.at(d) - betas.at(betas.size() - 2)*alternatives.at(i).price + b;
		if (utility > bestutility)
		{
			besti = i;
			bestday = alternatives.at(i).day;
			bestutility = utility;
		}
	}
	Alternative alt = alternatives.at(bestday);
	alternatives.at(bestday) = alternatives.at(0);
	alternatives.at(0) = alt;
	return alternatives;
}
int main()
{
	//We define the parameters and initialise vectors (setup)
	int Number_of_historics = 1; int Nb_Cust = 1000; int nb_Segments = 1;
	
	double beta_a = 1.00; double beta_e = 1.25; double beta_b = 0.9; 
	double beta_c = 0.9; double beta_d = 1.00; double beta_p = -1.;
	double beta_0 = 0.5;
	int horizon_temp = 5;
	vector<double> vbetas;
	vbetas.push_back(beta_a);
	vbetas.push_back(beta_b);
	vbetas.push_back(beta_c);
	vbetas.push_back(beta_d);
	vbetas.push_back(beta_e);
	vbetas.push_back(beta_p);
	vbetas.push_back(beta_0);
	std::extreme_value_distribution<double> gumbel(0.0, 1.0);
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
				alt.assignday(days_toassign, days_chosen.at(k));
				alternatives.push_back(alt);
			}
			Alternative alt;
			alt.assignday(days_toassign, -1);
			alternatives.push_back(alt);

			//We reorder the vector of options so the choice of the customer is first
			vector<Alternative> alternatives_ordered = makechoice(alternatives, vbetas, gumbel);

			//We put the options in the format wanted 
			for (int k = 0; k < alternatives_ordered.size(); k++)
			{
				string alt = to_string(alternatives_ordered.at(k).attributs.at(0).valeur);
				for (int l = 1; l < alternatives_ordered.at(k).attributs.size(); l++)
				{
					alt = alt + "," + to_string(alternatives_ordered.at(k).attributs.at(l).valeur);
				}
				alt = alt + "," + to_string(alternatives_ordered.at(k).price);
				Instance << alt << endl;
			}
			Size << days_toassign + 1 << "," << 1 << endl;
		}
	}
	cin.get();
}

