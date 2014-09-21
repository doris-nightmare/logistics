#include <vector>
#include <fstream>
#include <string>
#include <gecode/int.hh> // IntArgs
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>
#include <math.h>

using namespace std;
using namespace Gecode;
using namespace Gecode::Driver;



class Logistics: public IntMinimizeScript {
    private:
        

        //number of cars
        int n_cars;

        //no. of types of cars
        int n_type_cars;

        //no. of types of carriers
        int n_type_carriers;

        //last position of each type of car
        IntArgs last_cars;

        IntArgs car1_length;
        IntArgs car1_width;
        IntArgs car1_height;

        IntArgs car2_length;
        IntArgs car2_width;
        IntArgs car2_height;

        IntArgs car3_length;
        IntArgs car3_width;
        IntArgs car3_height;

        IntArgs carrier_length;
        IntArgs carrier_width;


        //variable to hold the max & min
        int max_carrier;
        int min_carrier;


        // Number of carries
        IntVar _cost;
        IntVar n_carriers;
        IntVar n_type1;
        IntVar n_type2;
        IntVarArgs car1,car2,car3;
        IntVarArgs carrier1_low;//, carrier1_up,carrier2_low,carrier2_up;
        IntVarArray assigned_carriers;
        SetVarArray carriers;
        IntVarArray cars;

        IntVar temp;
       





    public:
        /* CONSTRUCTORS */

        Logistics(bool share, Logistics& s);
        Logistics(const SizeOptions & opt);

        virtual ~Logistics();

        /* COPY FOR GECODE */
        virtual Space* copy(bool share);

        /* COST FUNCTION */
        virtual IntVar cost(void) const;

        /* GETTERS */
        

        /* SETTERS */
         
        /* PRINTING */
        virtual void print(std::ostream& os) const;


       
};