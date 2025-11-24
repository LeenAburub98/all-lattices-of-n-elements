#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

// Function to parse the relation matrix from the input file
vector<vector<int>> parseRelationMatrix(ifstream& infile, int n) {
    vector<vector<int>> matrix(n, vector<int>(n, 0));
    string line;

    for (int i = 0; i < n; ++i) {
        getline(infile, line);
        stringstream ss(line);
        for (int a=0; a<n; ++a){
                for (int b=a+1;b<n;++b){
        while (ss >> a) {
            if (ss.peek() == '-') {
                ss.ignore();
                ss >> b;
                matrix[a][b] = 1;
            }
        }
                }
        }
    }
    return matrix;
}

// Function to convert matrix to canonical form
void canonicalize(vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<int> permutation(n);
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }

    vector<vector<int>> min_matrix = matrix;
    do {
        vector<vector<int>> permuted_matrix(n, vector<int>(n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                permuted_matrix[i][j] = matrix[permutation[i]][permutation[j]];
            }
        }
        if (permuted_matrix < min_matrix) {
            min_matrix = permuted_matrix;
        }
    } while (next_permutation(permutation.begin(), permutation.end()));

    matrix = min_matrix;
}

// Function to output the lattice matrix to a string for comparison
string matrixToString(const vector<vector<int>>& matrix) {
    stringstream ss;
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ss << matrix[i][j] << " ";
        }
        ss << "\n";
    }
    return ss.str();
}

// Function to extract cover relations from a relation matrix, including element 0
vector<pair<int, int>> extractCoverRelations(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<pair<int, int>> coverRelations;

    // Check all pairs (i, j) for direct cover relations, including element 0
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] == 1) {
                // Check if this is a cover relation (i covers j)
                bool isCover = true;
                for (int k = 0; k < n; ++k) {
                    if (k != i && k != j && matrix[i][k] == 1 && matrix[k][j] == 1) {
                        isCover = false; // There's an intermediate element
                        break;
                    }
                }
                if (isCover) {
                    coverRelations.push_back({i, j});
                }
            }
        }
    }

    return coverRelations;
}

int main() {
    ifstream infile("lattices.txt");
    ofstream outfile("unique_lattices.txt");

    if (!infile) {
        cerr << "Error opening input file." << endl;
        return 1;
    }
    if (!outfile) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    set<string> uniqueLattices;
    int n = 8; // Number of elements in the lattice
    int latticeCounter = 0; // Counter for unique lattices

    string line;
    while (getline(infile, line)) {
        if (line.find("Lattice") != string::npos) {
            getline(infile, line); // Skip "Covering Relations" line
            vector<vector<int>> matrix = parseRelationMatrix(infile, n);

            canonicalize(matrix);  // Canonicalize the relation matrix
            string matrixStr = matrixToString(matrix);

            // Check if the matrix is unique
            if (uniqueLattices.find(matrixStr) == uniqueLattices.end()) {
                uniqueLattices.insert(matrixStr);
                latticeCounter++; // Increment the counter for unique lattices

                // Output unique relation to the file with the counter
                outfile << "Unique Lattice " << latticeCounter << ":\n";
                outfile << matrixStr << endl;

                // Now extract and output the cover relations for this unique relation
                vector<pair<int, int>> coverRelations = extractCoverRelations(matrix);
                outfile << "Cover Relations:\n";
                for (const auto& relation : coverRelations) {
                    outfile << relation.first << " -> " << relation.second << "\n";
                }
                outfile << endl;
            }
        }
    }

    infile.close();
    outfile.close();

    // Output the total number of unique lattices
    cout << "Total number of unique lattices: " << latticeCounter << endl;
    return 0;
}
