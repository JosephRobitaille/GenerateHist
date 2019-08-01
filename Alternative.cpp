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
			attributs.resize(nb_days);
			if(daychosen != -1)
				attributs[daychosen].valeur = 1;
			
			if (daychosen == -1)
				price = 0.0;
			else
				price = 5*((double)rand() / ((double)RAND_MAX + 1));
		}
};
