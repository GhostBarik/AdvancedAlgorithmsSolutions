#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
// #include <ctime> // for debugging purposes

using namespace std;


// -- DATA STRUCTURES --

struct EdgeData
{
    int index; // edge index
    int price; // edge price
    pair<int,int> coords; // indices of end vertices - (a,b) or (left, right)

    vector< pair<int,int>> left_neighbours; // left neighbours with price < this.price
    vector< pair<int,int>> right_neighbours; // right neighbours with price < this.price

    int best_left_price; // best path price found in the left side of edge
    int best_right_price; // best path price found in the right side of edge
};


// -- GLOBAL VARIABLES AND CONSTANTS --

int N; // number of vertices
int M; // number of edges

int global_max_price = -1; // best found path price will be stored here

vector< vector< pair<int, int>>> adj_list; // adjaciency list of vertices 
                                           // pair -> (intex_of_vertex, index_of_edge)

vector<EdgeData>  edges; // list of edges with detailed information stored
vector<EdgeData*> sorted_edges; // sorted pointers to the edges (will be sorted byt price)




// -- FUNCTION PROTOTYPES --
void read_data(istream&);
void prepare_data(void);
void calculate_best_price(void);


/* -- MAIN -- */

int main( int argc, char const *argv[] )
{
    // ifstream myfile("ex1.in");
    // read_data(myfile);
    read_data( std::cin );

    // clock_t begin = clock();
    
    prepare_data();
    calculate_best_price();

    // clock_t end = clock();
    // double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    cout << global_max_price << endl;
    // cout << "elapsed time: " << elapsed_secs << endl;

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
    temp >> N >> M;   // read N and M

    adj_list.resize(N); // allocate space for adj. list
    edges.resize(M);    // allocate space for edges
    sorted_edges.resize(M); // allocate space for sorted pointers to the edges 

    temp.clear();

    for( int edge_index = 0; edge_index < M; edge_index++ )
    {
        // read each edge along with price
        getline(in, row);
        temp << row;
        double a, b, p; // read (a,b) coords and price (p)
        temp >> a >> b >> p;

        // add this edge to the adj. list 
        // (2 times, because here we have undirected graph)
        adj_list[a].push_back( {b, edge_index});
        adj_list[b].push_back( {a, edge_index});

        // also store this edge's info in the edge list
        edges[edge_index].index  = edge_index;
        edges[edge_index].coords = {a, b};
        edges[edge_index].price = p;

        temp.clear();
    }
}


void prepare_data( void )
{
    // prochazime seznam hran
    for( int edge_index = 0; edge_index < edges.size(); edge_index++ )
    {
        auto& edge = edges[edge_index];
        int a = edge.coords.first;
        int b = edge.coords.second;
        int current_price = edge.price;

        // spocteme "leve" sousedni hrany pro danou hranu
        for( int e = 0; e < adj_list[a].size(); e++)
        {
            int e_index;
            tie(ignore, e_index) = adj_list[a][e];

            if( e_index != edge_index && 
                edges[e_index].price < current_price)
            {
                edge.left_neighbours.push_back( 
                    {e_index, edges[e_index].price} );
            }
        } // for

        // spocteme "prave" sousedi
        for( int e = 0; e < adj_list[b].size(); e++ )
        {
            int e_index;
            tie(ignore, e_index) = adj_list[b][e];

            if( e_index != edge_index && 
                edges[e_index].price < current_price)
            {
                edge.right_neighbours.push_back( 
                    {e_index, edges[e_index].price});
            }
        } // for
    } // for


    // initialize vector of pointers to edges
    for( int e = 0; e < sorted_edges.size(); e++)
        sorted_edges[e] = &edges[e];

    // sort the pointers by price
    sort( sorted_edges.begin(), sorted_edges.end(), 
    [](EdgeData* edge1, EdgeData* edge2) {
            return (edge1->price < 
                    edge2->price);
    });
}


/* -- MAIN CALCULATION PROCEDURE -- */

void calculate_best_price( void )
{
    // iterate over each edge in list starting 
    // from the smallest (with lowest price)
    for( int e = 0; e < sorted_edges.size(); e++)
    {
        // extract the data from edge
        auto& edge = (*sorted_edges[e]);
        int a = edge.coords.first;
        int b = edge.coords.second;

        // find best path from the left side of the edge
        int max_left_neib_price = 0;

        // search through left neighbours
        for (auto& n : edge.left_neighbours)
        {
            // extract the neighbour's data
            auto& neig = edges[n.first]; 

            // depending on which side this neighbour edge is
            // connected to the current one, we choose either
            // its right of left best price
            // (it should be already calculated for all
            // the edges with SMALLER price)
            if( neig.coords.first == a )
            {
               if( neig.best_left_price > max_left_neib_price)
                   max_left_neib_price = neig.best_left_price;
            }
            else // == b
            {
                if( neig.best_right_price > max_left_neib_price)
                   max_left_neib_price = neig.best_right_price;
            }
        }


        // find the best path from the right side of the edge
        int max_right_neib_price = 0;
        // search through the right neighbours of the edge
        for (auto& n : edge.right_neighbours)
        {
            // extract the neighbour's data
            auto& neig = edges[n.first]; 

            // left or right price?
            if( neig.coords.first == b )
            {
               if( neig.best_left_price > max_right_neib_price)
                   max_right_neib_price = neig.best_left_price;
            }
            else // == a
            {
                if( neig.best_right_price > max_right_neib_price)
                   max_right_neib_price = neig.best_right_price;
            }
        }

        // calculate the best paths prices for the current edge
        edge.best_left_price = edge.price + max_right_neib_price;
        edge.best_right_price = edge.price + max_left_neib_price;

        // choose the best path of two
        const int best_result = max(edge.best_left_price, 
                                    edge.best_right_price);

        // update global max price (if we've just found the better one)
        if( best_result > global_max_price)
            global_max_price = best_result;
    }
}