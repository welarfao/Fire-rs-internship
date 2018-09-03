#ifndef RASTER_READER_H
#define RASTER_READER_H


#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>


using namespace std;
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()


struct Point2D {
    double x;
    double y;
};

struct Pixel {
    size_t col;
    size_t row;
};






class Raster_Reader
{
private:
    GDALDataset *gDataSet;
    int  nBands;
    double gTransform[6];
    double  originX, originY, pWidth, pHeight;
    vector<double> RasterData;




public:

    Raster_Reader();
    Raster_Reader(const std::string& path);

    int nCols, nRows;
    double no_data_value;
    double maxheight;
    double minheight;

    double max_east;
    double max_west;
    double max_north;
    double max_south;


    void geoTransform();
    void Put_Raster_invector();

    Point2D get_World_coord( float col, float row );
    Pixel get_pixel( double X, double Y);


    string get_projection();

    double get_height(size_t col , size_t row );
    void Put_in_Raster(cv::Mat FP,string gdal_result_path);



    virtual ~Raster_Reader();




protected:


};

#endif // RASTER_READER_H
