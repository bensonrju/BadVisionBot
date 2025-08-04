
struct RCBounds {
    int row_min;
    int row_max;
    int col_min;
    int col_max;
};

bool within(int row, int col, RCBounds& gBounds) {
    return (   (row >= gBounds.row_min) && (row < gBounds.row_max) 
            && (col >= gBounds.col_min) && (col < gBounds.col_max) );
}
 


/*
class Bounder {
    private:
        int minX, maxX, minY, maxY;

    public:
        Bounder(int min_x, int max_x, int min_y, int max_y) {
            this->minX = min_x;
            this->maxX = max_x;
            this->minY = min_y;
            this->maxY = max_y;
        }
};
*/
