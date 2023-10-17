// AdaptiveMutualInformation.cpp
// AUTHOR: ddm-j
// DATE: 10/12/2023
// DESCRIPTION: Mutual Information via Adaptive Partitioning Algorithm. 
//              Two part chi-squared criterion. Inspiration: Assessing and Improving Classification & Prediction, Timothy Masters
//#define DEBUG_MODE

#include <iostream>
#include <fstream>
#include <array>
#include <random>
#include <vector>
#include <cmath>
#include <numeric>
#include <cassert>

#ifndef DEBUG_MODE
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#endif // !1

template <typename T>
std::vector<size_t> argsort(const std::vector<T>& v) {
    // initialize original index locations
    std::vector<size_t> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    // using std::stable_sort instead of std::sort
    // to avoid unnecessary index re-orderings
    // when v contains elements of equal values 
    std::stable_sort(idx.begin(), idx.end(),
        [&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });

    return idx;
}

std::vector<int> rank_transform(std::vector<double> &a) {

    // Initialize Vector for the Sorted Output
    int n = a.size();
    std::vector<int> a_i(n);

    // Perform the Ranking Transform
    int c = 0;
    for (auto(i): argsort(a)) {
        a_i[i] = c;
        c++;
    }

    return a_i;
}

void children(std::vector<int> &x, std::vector<int> &y, std::vector<std::vector<int> > &x_n, std::vector<std::vector<int> > &y_n, int xc, int yc) {
    // Compute Child Cell Counts
    // std::cout << "Performing Cell Count for: " << xc << ", " << yc << ", " << std::endl;

    for (int i = 0; i < x.size(); i++) {
        // Is point on the left?
        if (x[i] <= xc) {
            // Is it on the Bottom?
            if (y[i] <= yc) {
                x_n[0].push_back(x[i]);
                y_n[0].push_back(y[i]);
            }
            else {
                x_n[1].push_back(x[i]);
                y_n[1].push_back(y[i]);
            }
        }
        else {
            // Is it on the bottom?
            if (y[i] <= yc) {
                x_n[2].push_back(x[i]);
                y_n[2].push_back(y[i]);
            }
            else {
                x_n[3].push_back(x[i]);
                y_n[3].push_back(y[i]);
            }
        }
    }
}

double mi_contribution(std::array<int, 4> edges, int pcount, int n) {
    // This function computes the cell's contribution to the MI
    double px = (((double) edges[1] - edges[0] + 1.0)) / (double)n;
    double py = (((double) edges[3] - edges[2] + 1.0)) / (double)n;
    double pxy = (double(pcount)) / ((double)n);

    return pxy * std::log(pxy / (px * py));
}

double chi_test(std::vector<std::vector<int> >& x_n, int pcount, bool child) {
    double e = (child) ? (double)pcount / 4.0 : (double)pcount / 16.0;
    double diff = 0.0;
    double testval = 0.0;
    for (int i = 0; i < 4; i++) {
        diff = std::abs((double)x_n[i].size() - e) - 0.5;
        testval += (diff * diff)/e;
    }
    //testval /= e;
    
    return testval;
}

double mi_recursion(std::vector<int>& x, std::vector<int>& y, std::array<int, 4> edges, int n, int pcount, double chi1, bool force) {
    // Variable Declarations
    double local_MI = 0.0;
    double testval = 0.0;

    // We have less than the minimum number of points, stop recursion.
    if (pcount <= 3) {
        if (pcount != 0) {
            local_MI += mi_contribution(edges, pcount, n);
        }
    }
    else {
        // Calculate the Child Cell Counts
        int x_m = std::floor((edges[0] + edges[1]) / 2);
        int y_m = std::floor((edges[2] + edges[3]) / 2);
        std::vector<std::vector<int> > x_n(4, std::vector<int>());
        std::vector<std::vector<int> > y_n(4, std::vector<int>());
        children(x, y, x_n, y_n, x_m, y_m);

        // Check Child Sums
        int sum = 0;
        for (int i = 0; i < 4; i++) {
            sum += x_n[i].size();
        }
        assert(sum == pcount);

        // Prepare the Next Set of Recursion Edges
        std::array<std::array<int, 4>, 4> edges4 = {
            {
                { edges[0],  x_m,  edges[2],  y_m},
                { edges[0],  x_m, y_m + 1, edges[3]},
                {x_m + 1, edges[1],  edges[2],  y_m},
                {x_m + 1, edges[1], y_m + 1, edges[3]}
            }
        };

        // Compute the Chi-Squared Test (for Children)
        testval = chi_test(x_n, pcount, true);

        if ((testval > chi1) || force) {
            // Child Chi-Squared Test Passed, Split Cell and recurse deeper
            int c = 0;
            for (int i = 0; i < 4; i++) {
                if (x_n[i].size() > 0) {
                    local_MI += mi_recursion(x_n[i], y_n[i], edges4[i], n, x_n[i].size(), chi1, false);
                    }
                }
        }
        else {
            // Cell "Unsplittable" Compute MI Contribution
            local_MI += mi_contribution(edges, pcount, n);
        }
    }

    return local_MI;
}

double adaptive_mutual_information(std::vector<double>& x, std::vector<double>& y, double chi1) {
    // Preprocessing function that calls a recursive adaptive MI Calculator
    // Sort the Data
    std::vector<int> x_i = rank_transform(x);
    std::vector<int> y_i = rank_transform(y);

    // Calculate the starting range width
    int n = x_i.size();
    std::array<int, 4> edges = {0, n-1, 0, n-1};

    
    // Recursive MI
    double MI = mi_recursion(x_i, y_i, edges, n, n, chi1, true);
    
    return MI;
}

int main()
{
    std::cout << "Adaptive Mutual Information, C++ CLI";
    std::cout << std::endl;

    // Number of Datapoints and Correlation
    int N = 10000;
    double R = 0.1;

    std::vector<double> Z1(N), Z2(N);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0, 1);

    for (int i = 0; i < N; i++) {
        double X = dis(gen);
        double Y = dis(gen);

        Z1[i] = X;
        Z2[i] = R * X + std::sqrt(1 - R * R) * Y;
    }

    // Call The MI Calculator
    double MI = adaptive_mutual_information(Z1, Z2, 8.0);

    std::cout << "Mutual Information: " << MI << ", Actual: " << -0.5*std::log(1 - R*R) << std::endl;
    
    // Export to CSV for plotting
    //std::ofstream file("data_ranked.csv");
    //for (int i = 0; i < N; ++i) {
    //    file << X_i[i] << "," << Y_i[i] << std::endl;
    //}
    //file.close();

    return 0;
}

#ifndef DEBUG_MODE
namespace py = pybind11;

PYBIND11_MODULE(aminfo, m) {
    m.def("adaptive_mutual_information", &adaptive_mutual_information,
        "Compute the mutual information between two continuous random variables using the adaptive partitioning algorithm.",
        py::arg("X"),
        py::arg("Y"),
        py::arg("chi") = 8.0);
}
#endif // !1