#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>
// #include <ctime>

using namespace std;



// -- GLOBAL VARIABLES AND CONSTANTS --
const int max_N = 15;            // max possible number of vertices
const int max_E = max_N * max_N; // max possible number of edges

int N;    // number of vertices
int E;    // number of edges
double D; // length limit

vector< pair<double,double>> vertices; // list of vertices

vector< vector< pair<double, int>>> edges_matrix; // incidence matrix V*V (length, index)
vector< bitset<max_E>> edges_crossings;           // E*E (1/0 - crossing/no-crossing)

int best_cycle_num_vert = -1;   // number of vertices for the best path
double best_price = 99999999.0; // price for the best path
vector< bitset<max_N>> best_breadcrumbs; // vertices of best found paths 
int start_v;                    // index of starting vertex




// -- FUNCTION PROTOTYPES --
void read_data(istream&);
void build_matrices(void);

void search(const int v, 
            const double path_price, 
            const bitset<max_E>& bad_edges, 
            bitset<max_N>& visited_v, 
            const int depth);

// mathematical functions
inline double dot_2d( double, double, double, double);
inline double cross_2d( double, double, double, double);
inline double vec_length( double, double, double, double);
inline bool calc_intersected( double, double, double, double, 
                  double, double, double, double);



/* -- MAIN -- */

int main(int argc, char const *argv[])
{
    // ifstream myfile("test.txt");
    // read_data(myfile);
    read_data(std::cin);

    // clock_t begin = clock();
    build_matrices();

    for( int i = 0; i < N; i++)
    {
        start_v = i;
        bitset<max_N> visited_vertices;
        best_breadcrumbs.push_back(bitset<max_N>());
        search(start_v, 0.0, bitset<max_E>(), visited_vertices, 0);
    }

    // clock_t end = clock();
    // double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    // cout << "best total vertices: " << best_cycle_num_vert << endl;
    // cout << "best price: " << ceil(best_price) << endl;
    // cout << "elapsed secs: " << elapsed_secs << endl;
    cout << ceil(best_price) << endl;

    return 0;
}



/* -- DATA PREPARATION FUNCTIONS -- */


// read data from IO-stream
void read_data(istream& in)
{
    string row;
    stringstream temp;
    
    getline(in, row); // read first line
    temp << row;
    temp >> N >> D;   // read N and D

    vertices.resize(N); // allocate space for vertices

    temp.clear();

    for (int i = 0; i < N; i++)
    {
        // read pair of coords for each vertex
        getline(in, row);
        temp << row;
        double x, y;
        temp >> x >> y;

        // store new vertex in vector array
        vertices[i] = make_pair(x,y);

        temp.clear();
    }
}


// compute the incidence matrix, length of all edges
// and also the crossings for each 2 distinct edges
void build_matrices(void)
{
    // compute the total number of edges (comb(V,2))
    E = N*(N-1)/2;

    // prepare incidence matrix
    edges_matrix.resize(N);
    for( int i = 0; i < N; i++)
        edges_matrix[i].resize(N);
    
    // auxiliary list of edges (for further combinatorial generation pf pairs)
    // each edge is represented by pair of related vertex indices (a,b)
    vector<pair<int,int>> edges_list(E, {0, 0});

    int edge_index_counter = 0;


    for( int row = 0; row < N; row++)
    {
        for( int column = row+1; column < N; column++)
        {
            // fill the list of edges step-by-step
            edges_list[edge_index_counter] = {row, column};

            // extract both vertices from incidence matrix
            auto v1 = vertices[row];
            auto v2 = vertices[column];

            // compute the length of edge
            double l = vec_length(v1.first, v1.second, 
                                  v2.first, v2.second);

            // store the length along with edge index
            edges_matrix[row][column] = {l, edge_index_counter};
            edges_matrix[column][row] = {l, edge_index_counter};
            edge_index_counter++;
        }
    }

    // prepare edge crossing matrix
    edges_crossings.resize(E);

    // fill diagonal with `true` values
    for( int i = 0; i < E; i++)
        edges_crossings[i][i] = true;

    // generate all possible combinations of 2 distinct edges
    // and apply the intersection test on them
    for (int i = 0; i < E; i++)
    {
        for (int j = i+1; j < E; j++)
        {
            // extract the edge coordinates
            auto e1 = edges_list[i];
            auto e2 = edges_list[j];

            double e1_v1_x = vertices[e1.first].first;
            double e1_v1_y = vertices[e1.first].second;
            double e1_v2_x = vertices[e1.second].first;
            double e1_v2_y = vertices[e1.second].second;

            double e2_v1_x = vertices[e2.first].first;
            double e2_v1_y = vertices[e2.first].second;
            double e2_v2_x = vertices[e2.second].first;
            double e2_v2_y = vertices[e2.second].second;

            // check for possible intersection
            bool inter = calc_intersected(e1_v1_x, e1_v1_y, e1_v2_x, e1_v2_y,
                                          e2_v1_x, e2_v1_y, e2_v2_x, e2_v2_y);
            // store the test results
            edges_crossings[i][j] = inter;
            edges_crossings[j][i] = inter; // mirroring
        }
    }
}


/* -- MAIN SEARCH RECURSIVE FUNCTION -- */

void search( const int v,
             const double path_price, 
             const bitset<max_E>& bad_edges, 
             bitset<max_N>& visited_v, 
             const int depth)
{

    // mark the current vertex (v) as visited
    visited_v[v] = true;

    // have we already found the best path 
    // passing this vertex?
    if( depth == 0)
    {
        for( int i = 0; i < best_breadcrumbs.size(); i++)
        {
            if( best_breadcrumbs[i].test(v))
                return;
        }
    }

    // iterate over all edges, starting from the current vertex (v)
    for( int e = 0; e < edges_matrix[v].size(); e++)
    {
        // extract the edge data from the incidence matrix
        auto& edge      = edges_matrix[v][e];
        int e_index     = edge.second;
        double e_length = edge.first;

        // update the path price
        double new_price = path_price + e_length;

        // have we already exceeded the limit D?
        if( new_price > D)
            continue;

        // have we met already visited vertex?
        if( e == v || (visited_v.test(e)))
        {
            // we could also try to enclose the cycle
            // (connecting starting vertex with current)
            if (e == start_v && depth > 0)
            {
                // how many vertices we've already passed?
                int total_vertices = depth + 1;

                // have we found better cycle?
                if ((total_vertices > best_cycle_num_vert) ||
                    (total_vertices == best_cycle_num_vert && new_price < best_price))
                {
                    // update the global variables containing best results
                    best_cycle_num_vert = total_vertices;
                    best_price = new_price;
                    // store also the best path
                    best_breadcrumbs[start_v] = visited_v;
                }
            }
            continue;
        }

        // does this edge cross some of our current path's edges?
        if( bad_edges.test(e_index))
            continue; // if yes, ignore this edge
            
        // update the set of "bad" edges 
        // (such the edges we can never cross in the future)
        // union of 2 sets, almost O(1)
        bitset<max_E> new_bad_edges = bad_edges | edges_crossings[e_index]; 

        // continue with next
        search(e, new_price, new_bad_edges, visited_v, depth+1); 
    }

    // "unvisit" v
    visited_v[v] = false;
}



/* -- MATHEMATICAL FUNCTIONS --- */


// compute the length of vector given its 2D coordinates
inline double vec_length( double x1, double y1, double x2, double y2)
{
    double x_d = x2-x1;
    double y_d = y2-y1;
    return sqrt(x_d*x_d + y_d*y_d);
}

// cross product of two 2d-vectors
inline double cross_2d( double x1, double y1, double x2, double y2)
{
    return (x1*y2 - x2*y1);
}

// dot product of two 2d-vectors
inline double dot_2d( double x1, double y1, double x2, double y2)
{
    return (x1*x2 + y1*y2);
}

// intersection test for 2 line segments 
// (True - intersection exist / False - doesn't exist)
// parameters => points p1,p2,q1,q2
inline bool calc_intersected( double p1_x, double p1_y, 
                              double p2_x, double p2_y, 
                              double q1_x, double q1_y, 
                              double q2_x, double q2_y)
{
    // p (p1_x,p1_y) -> (p2_x, p2_y)
    // q (q1_x,q1_y) -> (q2_x, q2_y)

    // p = p_s + t * r
    // q = q_s + u * s

    static const double eps = 0.000000001;

    // vector of direction for p
    double r_x = p2_x - p1_x;
    double r_y = p2_y - p1_y;

    // vector of direction for q
    double s_x = q2_x - q1_x;
    double s_y = q2_y - q1_y;

    // b = q - p (vector of substraction b)
    double b_x = q1_x - p1_x;
    double b_y = q1_y - p1_y;

    // test1 => r `cross` s
    double test1 = cross_2d(r_x, r_y, s_x, s_y);

    // test2 => (q - p) `cross` r
    double test2 = cross_2d(b_x, b_y, r_x, r_y);

    if (abs(test1) < eps)
    {
        // in our task, "overlapped" line segments are allowed
        // so no need to do more tests here, always return 
        // "doesn't exist" (false) instead
        return false;
    }
    else
    {
        //  find `t` and `u`
        double RS = cross_2d(r_x, r_y, s_x, s_y);
        double t  = cross_2d(b_x, b_y, s_x, s_y) / RS;
        double u  = cross_2d(b_x, b_y, r_x, r_y) / RS;

        if ((0.0 <= t && t <= 1.0) && 
            (0.0 <= u && u <= 1.0))
        {
            if ((abs(t) < eps || abs(t-1.0) < eps) && 
                (abs(u) < eps || abs(u-1.0) < eps))
                return false;
            return true;
        }
        else
        {
            return false;
        }
    } // if
}

