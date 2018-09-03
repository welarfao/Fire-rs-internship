#ifndef MAPPING_H
#define MAPPING_H



using namespace std;

#include "Raster_Tile.h"
#include "Image.h"
#include "sensor_model.h"
#include <string>






struct Pixel_Test {
    uchar Value;
    bool Test;
};


struct Corner_Test {
    Pixel_Range PR;
    bool Test;
};

class Mapping
{

    private:

    vector<cv::Mat> Images_Mapped;
    vector<Raster_Tile> Carte;
    vector < Raster_ALL > Liste;
    double threshold;
    bool Segmentation,using_vector;
    void Map_direct(Image IM);
    void Map_with_vector(Image IM);
    sensor_model sen_mode;


    public:

    Mapping();
    Mapping(const std::string& path_DEM, bool use_pixelvector=false,bool Image_segmentation=false,double threshold=230 );

    Pixel_Test Pixel_Mapping(Pixel_Data PD,double noDATA,Image IM);
    double IMask( cv::Mat UndistortedImage , std::vector<PixelImage> Corners );

    void Map(Image IM);
    void set_threshold(double th);
    double get_threshold() const;
    void set_sensor_model(sensor_model sensor_mod);

    vector<cv::Mat>& get_IMapped();
    void Save_Show_FireM( string path_result);
    void DEM_infos();

    Point3D Raytracer(PixelImage Pix,Image I,Raster_Tile Rs);
    Corner_Test get_Imagecorners(Image IM,Raster_Tile RS);

    virtual ~Mapping();


};















#endif // MAPPING_H
