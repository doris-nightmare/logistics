#include "logistics.h"


const int ncar1 = 152;
const int ncar2 = 106;
const int ncar3 = 39;






/* CONSTRUCTION AND COPY */
Logistics::Logistics(const SizeOptions & opt):
    n_cars(ncar1+ncar2+ncar3), n_type_cars(3), n_type_carriers(2)
{

        //implied constraints
    max_carrier = ceil(n_cars/6)+1;
    min_carrier = floor(n_cars/27);
    

    //initialize the constants
    car1_length = IntArgs(ncar1,4.61);
    car1_width = IntArgs(ncar1,1.7);
    car1_height = IntArgs(ncar1,1.51);

    car2_length = IntArgs(ncar2,3.615);
    car2_width = IntArgs(ncar2,1.605);
    car2_height = IntArgs(ncar2,1.394);

    car3_length = IntArgs(ncar3,4.63);
    car3_width = IntArgs(ncar3,1.785);
    car3_height = IntArgs(ncar3,1.77);

    carrier_length = IntArgs(n_type_carriers*2);
    carrier_width = IntArgs(n_type_carriers*2);

    carrier_length[0] = 19; carrier_length[1] = 19; 
    carrier_length[2] = 24.3; carrier_length[3] = 24.3;
    carrier_width[0] = 2.7; carrier_width[1] = 2.7;
    carrier_width[2] = 3.5; carrier_width[3] = 2.7;

    _cost = IntVar(*this, 0, Gecode::Int::Limits::max);
    n_carriers = IntVar(*this,min_carrier,max_carrier);
    carriers = SetVarArray(*this, max_carrier*2,IntSet::empty, IntSet(0,n_cars-1));
    // car1 = IntVarArray(*this, ncar1, 1, 2 * n_type_carriers);
    // car2 = IntVarArray(*this, ncar2, 1, 2 * n_type_carriers);
    // car3 = IntVarArray(*this, ncar3, 1, 2 * n_type_carriers);
    cars = IntVarArray(*this, n_cars,1, 2 * n_type_carriers);
    temp = IntVar(*this, 1, 3);

    n_type1 = IntVar(*this, 0, max_carrier);
    n_type2 = IntVar(*this, 0, max_carrier);

    assigned_carriers= IntVarArray(*this, max_carrier-1,0,1);

    car1 << cars.slice(40,1,152);
    car2 << cars.slice(193,1,106);
    car3 << cars.slice(0,1,39);

    //carrrier_low << expr(*this, cars==1);


    
    rel(*this, temp, IRT_NQ, 2);
   
    rel(*this, car3, IRT_EQ, temp);
     cout << car3 << endl;


    rel(*this, n_type2 <= 0.2 * n_type1);
    //rel(*this, n_type2>=1);

     _cost = expr(*this, n_type1*1000+n_type2*1002);

     n_carriers=expr(*this, n_type1+n_type2);
     rel(*this, n_carriers>=min_carrier && n_carriers<=max_carrier);


   

    // for(int i = 0; i < max_carrier; i++){
    //     for(int j = 0; j < ncar1; j++ ){
    //         rel(*this, (assigned_carriers[i]==1)==(car1[j]==2*i || car1[j]==2*i+1));
    //     }
    //     for(int j = 0; j < ncar2; j++ ){
    //         rel(*this, (assigned_carriers[i]==1)==(car2[j]==2*i || car2[j]==2*i+1));
    //     }
    //     for(int j = 0; j < ncar3; j++ ){
    //         rel(*this, (assigned_carriers[i]==1)==(car3[j]==2*i || car3[j]==2*i+1));
    //     }
    //}
    // count(*this, assigned_carriers,1,IRT_EQ, n_carriers);


       //each layer of carriers must have different cars
        //and the disjoint union should be the full set
     //rel(*this, SOT_DUNION, carriers, SetVar(*this,0,n_cars-1,0,n_cars-1));
     //element(*this, SOT_DUNION, carriers, assigned_carriers, SetVar(*this,0,n_cars-1,0,n_cars-1));


    
    for(int i=0; i< max_carrier; i++){

    }

    branch(*this, cars, INT_VAR_NONE(), INT_VAL_RND(0));
    // branch(*this, car2, INT_VAR_SIZE_MAX(), INT_VAL_SPLIT_MIN());
    // branch(*this, car3, INT_VAR_SIZE_MAX(), INT_VAL_SPLIT_MIN());
}

Logistics::Logistics(bool share, Logistics& s) :
    IntMinimizeScript(share, s) {

        //update variables
        n_carriers.update(*this, share, s.n_carriers);
        _cost.update(*this, share, s._cost);
        carriers.update(*this, share, s.carriers);
        assigned_carriers.update(*this, share, s.assigned_carriers);
        n_type1.update(*this, share,s.n_type1);
        n_type2.update(*this, share, s.n_type2);
        // car1.update(*this, share, s.car1);
        // car2.update(*this, share, s.car2);
        // car3.update(*this, share, s.car3);
        cars.update(*this, share, s.cars);
        temp.update(*this, share, s.temp);
        
        //copy the args
        n_cars = s.n_cars;
        n_type_cars=s.n_type_cars;
        n_type_carriers=s.n_type_carriers;
        last_cars=s.last_cars;
        max_carrier=s.max_carrier;
        min_carrier=s.min_carrier;       
        car1_length = s.car1_length; 
        car1_height = s.car1_height; 
        car1_width = s.car1_width; 
        car2_length = s.car2_length; 
        car2_height = s.car2_height; 
        car2_width = s.car2_width; 
        car3_length = s.car3_length; 
        car3_height = s.car3_height; 
        car3_width = s.car3_width; 
        
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
    os << cars  <<" "
         //<< n_carriers
        // << carriers
        << endl;
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

    SizeOptions opt("Logistics");
    opt.solutions(0);

    //opt.mode(Gecode::SM_GIST);
    opt.parse(argc, argv);
    // 90 seconds:  
    //opt.time(40000);
   // opt.time(1200000); //20 min

    opt.time(60000); //1 min


    //opt.out_file(output.c_str());

    ScriptOutput::run<Logistics, BAB, SizeOptions>(opt);
    return 0;
}
