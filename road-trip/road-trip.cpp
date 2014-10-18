#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>
#include <limits>
#include <queue>


using namespace std;

// type synonym for map<>
template < typename Key, typename Value > 
using map = unordered_map< Key,Value >;


// -- DATA STRUCTURES --

struct VertexData
{
    uint32_t best_path;
    bool is_oyster_city;
    int fuel_left;
    int visiting_time;
};


// -- GLOBAL VARIABLES AND CONSTANTS --

int M; // number of cities with Oyster
int N; // number of cities without Oyster
int V; // total number of vertices/cities (M+N)
int E; // total number of edges/highways

int S; // starting city
int T; // ending city

int max_fuel_capacity; // max. fuel cap. in motorcycle's tank

const int max_V = 116000;  // maximum number of vertices
bitset<max_V> visited_set; // "visited" vertices will be stored in this set
vector< pair< int, int >> Q; // proirity queue for path prices

vector< vector< pair< int,int >>> adj_list; // main graph represented as adj. list
vector< VertexData > vertices_data;         // additional data about vertices 
map< int, map< int, int >> oyster_pumps;    // adj. list, but only for pumps



// -- FUNCTION PROTOTYPES --
void read_data(istream&);

template < bool final = false, typename neighbor_container >
void Dijkstra( int start_pump, int time, neighbor_container& neib_co );



/* -- MAIN -- */

int main( int argc, char const *argv[] )
{
    // ifstream myfile("test1.txt");
    // read_data(myfile);
    read_data( std::cin );

    int time = 0;

    for( auto& kv : oyster_pumps )
    {
        visited_set = bitset<max_V>(); // refresh set
        int key = kv.first;
        Dijkstra(key, time, adj_list);
        time++;
    }

    // now search only between the petrol-stations
    // switch to the second version of Dijkstra's algorithm
    // (with final="true" option in template and 3rd parameter)
    time++;
    visited_set = bitset<max_V>(); // refresh set
    Dijkstra<true>(S, time, oyster_pumps); // enable "final" version

    // print best resulst
    cout << vertices_data[T].best_path << endl;

    return 0;
}




/* -- DATA PREPARATION FUNCTIONS -- */


// read data from IO-stream
void read_data( istream& in )
{
    string row;
    stringstream temp;
    
    getline(in, row); // read first line
    temp << row;
    // read all size constants
    temp >> M >> N >> E >> max_fuel_capacity >> S >> T;

    V = M+N; // calculate total number of vertices
    adj_list.resize(V); // allocate space for adj. list
    vertices_data.resize(V); // same for vertices data

    // initialize vertices additional data
    for( auto& v : vertices_data )
    {
        v.best_path = std::numeric_limits<uint32_t>::max();
        v.is_oyster_city = false;
        v.fuel_left = -1;
        v.visiting_time = -1;
    }

    temp.clear();

    // read all pump indices
    for( int i = 0; i < M; i++ )
    {
        getline(in, row); // read new line
        temp << row;
        
        int v_index; // read pump index
        temp >> v_index;

        oyster_pumps[v_index] = {}; // allocate new petrol-station

        // update info about current city with Oyster
        VertexData& v_data = vertices_data[v_index];
        v_data.fuel_left = max_fuel_capacity;
        v_data.is_oyster_city = true;

        temp.clear();
    }

    // read all edges/highways
    for( int i = 0; i < E; i++ )
    {
        getline(in, row); // read new line
        temp << row;

        int a, b, w; // pair of end vertices and edge price
        temp >> a >> b >> w;

        // pridame hranu do seznamu sousednosti
        adj_list[a].push_back({b, w});
        adj_list[b].push_back({a, w});

        temp.clear();
    }   
}



/* -- MAIN CALCULATION PROCEDURES -- */

// our "augmented" Dijkstra's algorithm :D

// this algorithm exists in two versions
// 1st version of alg. can scan between any vertex/city 
// whereas the second one takes into account only cities with 
// Oyster's filling stations with highways leading between only them

// because in the essence both of these two algorithms perform almost the same
// kind of calculations, we can easily use the template function with 
// a few template variables acting like switches between these two versions
template < bool final = false, typename neighbor_container >
void Dijkstra( int start_pump, int time, neighbor_container& neib_co )
{
    // assing to the starting city zero price
    VertexData& start_p = vertices_data[start_pump];
    start_p.best_path = 0;
    // we always start with max. cap. of fuel
    start_p.fuel_left = max_fuel_capacity;

    // insert starting city into the queue
    Q.push_back({-start_p.best_path, start_pump});

    // until the queue is not empty
    while( Q.size() != 0 )
    {
        int u; // extract element with MIN price
        pop_heap( Q.begin(),Q.end() ); 
        tie(ignore,u) = Q.back();
        Q.pop_back();

        // mark vertex as "visited"
        visited_set[u] = true;

        // extract the additional data for vertex `u`
        VertexData& u_data = vertices_data[u];

        // depending of the version of algoritm,
        // we extract the neighbors of given vertex 
        // either from `adj_list` (1st version)
        // or from `oyster_pumps` map (2nd one)
        auto& neighbors = neib_co[u];

        // -> iterate over all the neighbors
        for( auto& v_pair: neighbors )
        {
            // extract index and price of the adj. vertex
            int v, price_to_v;
            tie(v, price_to_v) = v_pair;

            // extract the additional data for vertex `v`
            VertexData& v_data = vertices_data[v];

            // do we have enough fuel to continue walking?
            if (final == false and 
                u_data.fuel_left < price_to_v)
                continue; // no, we don't :(

            // calculate the new total path's length
            int new_path_price = u_data.best_path + price_to_v;
            int new_fuel_left  = v_data.is_oyster_city ? 
                                 0 : (u_data.fuel_left - price_to_v);

            // "fresh" vertex means that we're visiting it for the first time
            // moreover, if the vertex was already visitied in the previous calls of 
            // Dijkstra() function (has lower time), it's considered "fresh" 
            bool fresh = (v_data.visiting_time < time);

            // if currently chosen vertex is not yet marked as "visited"
            // and if new calculated path is shorter than 
            // previous, stored in vertex `v`  
            // -> update info about best path found
            if( !visited_set.test(v) and  
                (new_path_price < v_data.best_path or fresh) )
            {

                // moreover, if the current vertex/city have oyster station
                // inside, it means that we have just found new shortest 
                // path between 2 oyster stations, store this path
                // in `oyster_pumps` map
                if( v_data.is_oyster_city )
                {
                    oyster_pumps[start_pump][v] = new_path_price;
                    oyster_pumps[v][start_pump] = new_path_price;
                }

                // update best/shortest path for `v`
                int old_price = v_data.best_path;
                v_data.best_path = new_path_price;
                v_data.visiting_time = time;
                v_data.fuel_left = new_fuel_left;

                // if vertex `v` is "fresh" and it's not yet
                // present in the priority queue `Q`
                // just simply add it into that queue

                // but if it's not "fresh"...
                if( !fresh )
                {
                    // by updating the path we've change the priority of the
                    // current vertex in priority queue, so we need to the 
                    // queue also has to be updated

                    // first, we need delete that altered vertex 
                    // from the queue
                    int element_index = -1; // to prevent unnecessary "warnings"
                    for( int i = 0; i < Q.size(); i++)
                    {
                        auto test = make_pair(-old_price, v);
                        if( Q[i] == test )
                        {
                            element_index = i;
                            break;
                        }
                    }

                    // swap this element with last and delete it
                    auto swp = Q[element_index];
                    Q[element_index] = Q[Q.size()-1];
                    Q[Q.size()-1] = swp;

                    Q.pop_back();

                    // heap now is broken, we need to rebuild it
                    make_heap(Q.begin(), Q.end());
                }
 
                // add vertex with updated price
                // to the queue/heap
                Q.push_back({-v_data.best_path, v});
                push_heap(Q.begin(), Q.end());

            } // if ("visited" test and "better path found" test)
        } // for (neighbors)
    } // while (queue)
} // Dijkstra()