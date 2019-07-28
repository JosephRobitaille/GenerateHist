#include "Attribut.cpp"
#include <iostream>
#include <vector>
using namespace std;

class Alternative
{
	public:
		Alternative() {}
		~Alternative() {}
		vector<Attribut> attributs;
		int day;
		double price;
		void assignday(int nb_days, int daychosen)
		{
			day = daychosen;
			for (int i = 0; i < nb_days; i++)
			{
				if (i == day)
				{
					Attribut valjour(1);
					attributs.push_back(valjour);
				}
				else
				{
					Attribut valjour(0);
					attributs.push_back(valjour);
				}
			}
			if (daychosen == -1)
				price = 0.0;
			else
				price = (double)rand() / ((double)RAND_MAX + 1);
		}
};