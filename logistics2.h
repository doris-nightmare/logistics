#include <vector>
#include <fstream>
#include <string>
#include <gecode/int.hh> // IntArgs
#include <gecode/driver.hh>
#include <gecode/minimodel.hh>
#include <gecode/set.hh>
#include <gecode/gist.hh>
#include <math.h>

using namespace std;
using namespace Gecode;
using namespace Gecode::Driver;



class Logistics: public IntMinimizeScript {
    private:
        

        //number of cars
        int n_cars;


        //variable to hold the max & min
        int max_carrier;
        int min_carrier;

        IntArgs car_length;  //l_i
        IntArgs car_width;    //w_i
        IntArgs car_height;   //h_i

        IntArgs carrier_length; //dimension k
        IntArgs carrier_width; //dimension 2k
        IntArgs carrier_type; //dimension k
        IntArgs carrier_cost; //dimension k


        //Decision variables
        IntVar _cost;
        BoolVarArray assigned_carrier; //wether the carrier is assigned, sk
        SetVar assigned_carrier_set;
        BoolVarArray cars; //x_ijk
        IntVarArray xcoord; //width
        IntVarArray ycoord; //length
       


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