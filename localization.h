// Localization header file

#include <vector>
#include <iostream>

using namespace std;


void printSensory(vector<bool> sensory) {
	cout << "Filtering after Evidence ["
		 << sensory[0] << ", " 
         << sensory[1] << ", "
		 << sensory[2] << ", " 
         << sensory[3] << "]" << endl;
}


struct RCBounds {
    int row_min;
    int row_max;
    int col_min;
    int col_max;
};

bool within(int row, int col, RCBounds& gBounds) {
    return (   (row >= gBounds.row_min) 
            && (row < gBounds.row_max) 
            && (col >= gBounds.col_min) 
            && (col < gBounds.col_max) );
}