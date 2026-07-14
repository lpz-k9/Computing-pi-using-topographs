#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <sstream>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const std::vector<int>& v){
    os << "[";
    for(int i = 0; i < v.size(); ++i){
        os << v[i];
        if(i < v.size()-1) os << ", ";
    }
    os << "]";
    return os;
}

//Input a vector {0,1,1} e.g. as 011
std::istream& operator>>(std::istream& is, std::vector<int>& path) {
    path.clear();
    std::string line;
    std::getline(is, line);
    for (char c : line) {
        if (c == '0') path.push_back(0);
        else if (c == '1') path.push_back(1);
    }
    return is;
}

// Decode: integer n -> path as vector of 0s and 1s (without the sentinel)
// e.g. 9 = 0b1001 -> {0, 0, 1}
std::vector<int> decode(int n) {
    if (n == 0) throw std::invalid_argument("n must be >= 1");

    int d = 0;
    int tmp = n >> 1;
    while (tmp) { ++d; tmp >>= 1; }   // d = depth = position of sentinel

    std::vector<int> path(d);
    int bits = n ^ (1 << d);    // strip sentinel
    for (int i = d - 1; i >= 0; --i) {
        path[i] = bits & 1;
        bits >>= 1;
    }
    return path;                       // path[0] is the first move
}

// Encode: path as vector of 0s and 1s -> integer n (with sentinel prepended)
// e.g. {0, 0, 1} -> 9 = 0b1001
int encode(const std::vector<int>& path) {
    int n = 1u;                   // start with sentinel
    for (int bit : path) {
        n = (n << 1) | (bit & 1);
    }
    return n;
}

struct Tuple {
    int k, n;
};

struct Triple {
    int a, b, c;
};

struct configuration {
    Tuple address; 
    Triple config;
    configuration(int d, int n, int a, int b, int c) : address({d,n}), config({a,b,c}){
    }
};

//address.k determines if we go forward or backwards
//address.n describes the path. 0 means a left turn, 1 a right turn
//          so e.g. left, left, left, right, left is determined by {0,0,0,1,0}
Triple ConfigurationIs(Tuple address, Triple initial){
    std::vector<int> path = decode(address.n);
    Triple config = initial;
    int d = (int)path.size();
    assert(address.k == 1 || address.k == -1);

    if(address.k == 1){
        //This means forward

        int k = 0;
        while(k < d){
            int a = config.a, b = config.b, c = config.c;

            if(path[k] == 0){
                //left turn
                config.b = b+2*a;
                config.c = a+b+c;
            } else {
                //right turn
                config.a = a+b+c;
                config.b = b+2*c;
            }
            ++k;
        }

    } else {
        //This means backward

        int k = 0;
        while(k < d){
            int a = config.a, b = config.b, c = config.c;

            if(path[k] == 0){
                //back left turn
                config.b = b-2*a;
                config.c = a-b+c;
            } else {
                //back right turn
                config.a = a-b+c;
                config.b = b-2*c;
            }
            ++k;
        }

    }
    return config;
}

std::ostream& operator<<(std::ostream& os, Triple trip){
    os << "[" << trip.a << "," << trip.b << "," << trip.c << "]";
    return os;
}

struct topograph {
    std::map<std::pair<int,int>, Triple> T;
    topograph(int d, int a, int b, int c) {
        Triple initial = {a, b, c};
        for (int k = 1; k < (1 << (d+1)); ++k) {
            Tuple address  = {1,  k};
            Tuple address2 = {-1, k};
            T[{1,  k}] = ConfigurationIs(address,  initial);
            T[{-1, k}] = ConfigurationIs(address2, initial);
        }
    }
};

double approximate_pi(int depth) {
    Triple initial = {1, 0, 1};
    double sum = 0.0;

    auto add_contribution = [&](Triple cfg) {
        double a = cfg.a, b = cfg.b, c = cfg.c;
        double r = a, s = c, t = a + b + c;
        sum += 1.0 / (r * s * t);
    };

    // Root configuration (depth 0)
    add_contribution(initial);

    // Forward and backward branches, depth 1 to `depth`
    for (int k = 2; k < (1 << (depth + 1)); ++k) {
        add_contribution(ConfigurationIs({1,  k}, initial));
        add_contribution(ConfigurationIs({-1, k}, initial));
    }

    return (4.0/3.0) * sum;
}

int main(){

    //int a,b,c;
    //int k;
    //int n;
    //std::vector<int> path;

    //a = -49;
    //b = 119;
    //c = 229;

    //std::cout << "Input topograph:\n";
    //std::cin >> a >> b >> c;
    //std::cout << "Input path to configuration:\n";
    //std::cin >> k;
    //k = -1;
    //std::cin.ignore();
    //std::cin >> path;

    //n = (int)path.size();

    //topograph topo(n, a, b, c);
    //Triple cfg = topo.T[{k, encode(path)}];
    //std::cout << cfg;

    //int n;
    //std::cout << "Enter depth of approximation:\n";
    //std::cin >> n;
    //check_duplicates(n);
    int n = 25;
    std::cout << "depth = " << n << ", approximation = " << std::setprecision(15) << approximate_pi(n);

    //check_vertex_duplicates(8);

    return 0;
}