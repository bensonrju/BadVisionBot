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

bool boundsCheck(int rows, int columns, RCBounds gBounds) {
	bool err = false;
	if(gBounds.row_max != rows) {
		cout << "ERROR: Given row size: " << gBounds.row_max 
			 << "   Calculated row rize: " << rows << endl;
		err = true;
	}
	
	if(gBounds.col_max != columns) {
		cout << "ERROR: Given column size: " << gBounds.col_max 
			 << "   Calculated column rize: " << columns << endl;
		err = true;
	}
	return err;
}