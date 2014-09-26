#include "logistics2.h"
#include <iostream>
#include <string>

using namespace std;



/* CONSTRUCTION AND COPY */
Logistics::Logistics(const SizeOptions & opt):
    n_cars(ncar1+ncar2+ncar3)
{
      //implied constraints
    max_carrier = ceil(n_cars/6)+1;
    min_carrier = floor(n_cars/27);

    int n_type1_max = max_carrier;
    int n_type2_max = max_carrier*0.2;
    int total_max = n_type1_max + n_type2_max;



    //initialize the items(cars)

    for(int i=0; i< ncar1; i++){
         car_length << lcar[0];
         car_width << wcar[0];
         car_height << hcar[0];
    }
     for(int i=0; i< ncar2; i++){
         car_length << lcar[1];
         car_width << wcar[1];
         car_height << hcar[1];
    }
     for(int i=0; i< ncar3; i++){
         car_length << lcar[2];
         car_width << wcar[2];
         car_height << hcar[2];
    }

    //initialize the bins(carriers)
    //total number is n_type1_max+n_type2_max, k = n_type1_max + n_typ2_max


    //carrier_length << IntArgs::create(n_type1_max, lcarrier[0]);

    for(int i=0; i<n_type1_max; i++){
        carrier_length << lcarrier[0];
        carrier_width << wcarrier[0][0];
        carrier_width << wcarrier[0][1];
        carrier_type << 0;
        carrier_cost << 1000;
    }
    for(int i=0; i<n_type2_max; i++){
        carrier_length << lcarrier[1];
        carrier_width << wcarrier[1][0];
        carrier_width << wcarrier[1][1];
        carrier_type << 1;
        carrier_cost << 1002;
    }

//for Q1, Q2, Q3, assume max_carrier is the number of type1
//so initialize 20% * max_carrier as type2


    //initialization of decision variables
    assigned_carrier = BoolVarArray(*this, total_max, 0, 1);  //s_k ; dimension k
    assigned_carrier_set = SetVar(*this, IntSet::empty, 0, total_max - 1);
    cars = BoolVarArray(*this, n_cars * 2 * total_max, 0, 1); //x_ijk, use slice to access
    _cost = IntVar(*this, 0, Gecode::Int::Limits::max);
    xcoord = IntVarArray(*this, n_cars, 0, Gecode::Int::Limits::max);
    ycoord = IntVarArray(*this, n_cars, 0, Gecode::Int::Limits::max);

    //post constraints

    //calculate the cost, sum cost if the value in assigned_carrier is 1
    channel(*this,assigned_carrier,assigned_carrier_set);
    weights(*this,IntArgs::create(total_max,0), carrier_cost, assigned_carrier_set, _cost);
    rel(*this, _cost, IRT_GQ, min_carrier*1000);  

    
    Matrix<BoolVarArray> temp(cars,2 * n_cars, total_max);

    for(int k=0; k < n_type1_max+n_type2_max; k++){
        //an item can be assigned to a bin layer only if the bin is used
       rel(*this, temp.row(k), IRT_LQ, assigned_carrier[k]); 
    }

    for(int i=0; i<n_cars; i++){
        //sigma x_ijk = 1
       linear(*this, cars.slice(2*i, 2*total_max ,n_cars)+
           cars.slice(2*i+1, 2*total_max ,n_cars), IRT_EQ, 1);  
    }

    //cumulative... to be continued
   
    for(int k = 0; k < total_max ; k++){
        //IntVarRanges i(assigned_carrier[k]);
        for(int j=0; j<2; j++){
            //if there's item in this bin
            SetVar selected_cars = SetVar(*this, IntSet::empty, 0,n_cars-1);
            //cout <<  cars.slice(2*total_max*k+j, 2, n_cars);
            channel(*this, cars.slice(2*total_max*k+j, 2, n_cars), selected_cars); //set may be empty

           IntVar size = expr(*this, cardinality(selected_cars));

            IntVarArgs xcoord_current;
            IntVarArgs ycoord_current;
            IntArgs width_current;
            IntArgs length_current;

            IntVarRanges i_size(size);
            if(i_size.min()!=0 && i_size.min()==i_size.max()){
                //fixed & nonzero
                int card = i_size.min(); // size of the set
                IntVar idx = IntVar(*this,  0, Gecode::Int::Limits::max);
                rel(*this, selected_cars, IRT_EQ, idx);

                for(IntVarValues i(idx); i(); ++i){
                    xcoord_current << xcoord[i.val()];
                    ycoord_current << ycoord[i.val()];
                    width_current << car_width[i.val()];
                    length_current << car_length[i.val()];

                    rel(*this, xcoord_current, IRT_LQ, carrier_width[2*k+j]);
                    rel(*this, ycoord_current, IRT_LQ, carrier_length[k]);


                    cumulative(*this, carrier_width[2*k+j], xcoord_current, width_current,length_current);
                    cumulative(*this, carrier_length[k], ycoord_current,length_current,width_current);

                }    

            }
        
        }
    }

    //20% constraint
    IntVar n_type1 = IntVar(*this, 0, Gecode::Int::Limits::max);
    IntVar n_type2 = IntVar(*this, 0, Gecode::Int::Limits::max);

    n_type1 = expr(*this, sum( assigned_carrier.slice(0,1,n_type1_max)));
    n_type2 = expr(*this, sum( assigned_carrier.slice(n_type1_max,1,n_type2_max)));

    rel(*this, n_type2 <= n_type1*0.2);


    //if height > 1.7 , bin can be packed only to lower layer

    for(int i=0; i< n_cars; i++){
        if(car_height[i]>1.7){
            //cannot be packed in the upper layer
            rel(*this, cars.slice(2*i+1, 2*total_max ,n_cars),IRT_EQ, 0);  
            //must be packed in the lower layer
            linear(*this, cars.slice(2*i, 2*total_max ,n_cars), IRT_EQ, 1);  
        }
    }




    
     branch(*this, assigned_carrier, INT_VAR_SIZE_MAX(), INT_VAL_MAX()) ;
    // branch(*this, cars, INT_VAR_SIZE_MAX(), INT_VAL_MAX()) ;
}

Logistics::Logistics(bool share, Logistics& s) :
    IntMinimizeScript(share, s) {
        //update the variables
       _cost.update(*this, share, s._cost);
       assigned_carrier.update(*this, share, s.assigned_carrier);
       assigned_carrier_set.update(*this, share, s.assigned_carrier_set);
       cars.update(*this, share, s.cars);
       xcoord.update(*this, share, s.xcoord);
       ycoord.update(*this, share, s.ycoord);        
    }

Logistics::~Logistics() {
}

Space* Logistics::copy(bool share) {
    return new Logistics(share,*this);
}

 IntVar Logistics::cost(void) const{

        return _cost;
}




/* SYMMETRY BREAKING FUNCTION */




/* PRINTING OF A SOLUTION */
void Logistics::print(std::ostream& os) const {
    os <<  _cost << "\n"
        << assigned_carrier_set <<"\n"
     //<< cars
    <<endl;
    /*
    os << "cost: "   << total_cost << endl
        << "scenes: " << scenes     << endl
        << "days: "   << days       << endl
        << "sums: "   << sums       << endl
        << "actors per day: " << actorsPerDay << endl;
        */
}

/* CUSTOM VALUE & VARIABLE SELECTION HEURISTICS */


/* CUSTOM PARSING OF OPTIONS */

/* Wierd code: Overriding the default output */
template<class BaseSpace>
class ScriptBaseCustom : public BaseSpace {
    public:
        /// Default constructor
        ScriptBaseCustom(void) {}
        /// Constructor used for cloning
        ScriptBaseCustom(bool share, ScriptBaseCustom& e) : BaseSpace(share,e) {}
        /// Print a solution to \a os
        virtual void print(std::ostream& os) const { (void) os; }
        /// Compare with \a s
        virtual void compare(const Space&, std::ostream& os) const {
            (void) os;
        }
        /// Choose output stream according to \a name
        static std::ostream& select_ostream(const char* name, std::ofstream& ofs);

        template<class Script, template<class> class Engine, class Options>
            static void run(const Options& opt, Script* s=NULL);
    private:
        template<class Script, template<class> class Engine, class Options,
            template<template<class> class,class> class Meta>
                static void runMeta(const Options& opt, Script* s);
        /// Catch wrong definitions of copy constructor
        explicit ScriptBaseCustom(ScriptBaseCustom& e);
};

typedef ScriptBaseCustom<Space> ScriptOutput;

template<class Space>
std::ostream&
ScriptBaseCustom<Space>::select_ostream(const char* name, std::ofstream& ofs) {
    if (strcmp(name, "stdout") == 0) {
        return std::cout;
    } else if (strcmp(name, "stdlog") == 0) {
        return std::clog;
    } else if (strcmp(name, "stderr") == 0) {
        return std::cerr;
    } else {
        ofs.open(name);
        return ofs;
    }
}

template<class Space>
template<class Script, template<class> class Engine, class Options>
void
ScriptBaseCustom<Space>::run(const Options& o, Script* s) {
    //assert(s != 0);
    if (o.restart()==RM_NONE) {
        runMeta<Script,Engine,Options,EngineToMeta>(o,s);
    } else {
        runMeta<Script,Engine,Options,RBS>(o,s);
    }
}

template<class Space>
template<class Script, template<class> class Engine, class Options,
    template<template<class> class,class> class Meta>
    void
    ScriptBaseCustom<Space>::runMeta(const Options& o, Script* s) {
        using namespace std;

        ofstream sol_file, log_file;

        ostream& s_out = select_ostream(o.out_file(), sol_file);
        ostream& l_out = select_ostream(o.log_file(), log_file);

        try {
            switch (o.mode()) {
                case SM_SOLUTION:
                    {
                        //                l_out << o.name() << endl;
                        Support::Timer t;
                        int i = o.solutions();
                        t.start();
                        if (s == NULL)
                            s = new Script(o);
                        //                unsigned int n_p = s->propagators();
                        //                unsigned int n_b = s->branchers();
                        Search::Options so;
                        so.threads = o.threads();
                        so.c_d     = o.c_d();
                        so.a_d     = o.a_d();
                        so.stop    = CombinedStop::create(o.node(),o.fail(), o.time(),
                                o.interrupt());
                        so.cutoff  = createCutoff(o);
                        so.clone   = false;
                        if (o.interrupt())
                            CombinedStop::installCtrlHandler(true);
                        {
                            Meta<Engine,Script> e(s,so);
                            if (o.print_last()) {
                                Script* px = NULL;
                                do {
                                    Script* ex = e.next();
                                    if (ex == NULL) {
                                        if (px != NULL) {
                                            px->print(s_out);
                                            delete px;
                                        }
                                        break;
                                    } else {
                                        delete px;
                                        px = ex;
                                    }
                                } while (--i != 0);
                            } else {
                                do {
                                    Script* ex = e.next();
                                    if (ex == NULL)
                                        break;
                                    ex->print(s_out);
                                    delete ex;
                                } while (--i != 0);
                            }
                            if (o.interrupt())
                                CombinedStop::installCtrlHandler(false);
                            Search::Statistics stat = e.statistics();
                            //                    s_out << endl;
                            // s_out << "cost: " << cost << endl; 
                            if (e.stopped()) {
                                //                        l_out << "Search engine stopped..." << endl
                                //                        << "\treason: ";
                                int r = static_cast<CombinedStop*>(so.stop)->reason(stat,so);
                                if (r & CombinedStop::SR_INT)
                                    l_out << "user interrupt " << endl;
                                else {
                                    if (r & CombinedStop::SR_NODE)
                                        l_out << "node ";
                                    if (r & CombinedStop::SR_FAIL)
                                        l_out << "fail ";
                                    if (r & CombinedStop::SR_TIME)
                                        l_out << "time ";
                                    //                            l_out << "limit reached" << endl << endl;
                                    l_out << "$>\\Timeout$";
                                }
                            }
                            else {
                                double runtime_msec = t.stop();
                                double runtime_sec = runtime_msec/1000;

                                s_out << "runtime: " << showpoint << fixed
                                    << setprecision(3) <<  runtime_sec << endl;
                                // l_out.width(8);
                                //                    cout.fill(' ');
                                // l_out << showpoint << fixed
                                //     << setprecision(3) << runtime_sec ;
                                //                    cout << " (";
                                //                    cout.width(9);
                                //                    cout.fill('0');
                                //                    cout << runtime_msec << " ms)";
                            }
                            s_out << "failure: " << stat.fail << endl;
                        }
                        delete so.stop;
                    }
                    break;
                default:
                    Script::template run<Logistics,DFS,Options>(o);
            }
        } catch (Exception& e) {
            cerr << "Exception: " << e.what() << "." << endl
                << "Stopping..." << endl;
            if (sol_file.is_open())
                sol_file.close();
            if (log_file.is_open())
                log_file.close();
            exit(EXIT_FAILURE);
        }
        if (sol_file.is_open())
            sol_file.close();
        if (log_file.is_open())
            log_file.close();
    }


/* MAIN */
int main(int argc, char *argv[])
{
  
    cout<<"请输入文件路径："
    string path;
    cin >> path;
    ifstream file (path);
    ofstream outfile ("out.csv");
    string value;
    if(file.good())
    {
     		while(file.good())
            {
			cout<<path<<"文件打开";
                  getline(file,value,',');
		}
    }else
    {
		cout<<"文件路径或格式错误，按任意键退出"
            char a;
            cin >> a;
    }
    SizeOptions opt("Logistics");
    opt.solutions(0);

    opt.parse(value);

    opt.time(60000); //1 min

    opt.out_file(outfile);

    ScriptOutput::run<Logistics, BAB, SizeOptions>(opt);

    outfile.close();
    return 0;
}
