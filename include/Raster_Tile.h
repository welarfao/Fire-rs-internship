#ifndef RASTER_TILE_H
#define RASTER_TILE_H


#include <fstream>
#include <opencv2/opencv.hpp>
#include "Raster_Reader.h"
#include "sensor_model.h"
#include <math.h>

using namespace std;


struct Pixel_Data {

    double x_upleft;
    double y_upleft;
    double z_upleft;

    double x_upright;
    double y_upright;
    double z_upright;

    double x_downleft;
    double y_downleft;
    double z_downleft;

    double x_downright;
    double y_downright;
    double z_downright;

    int col;
    int row;
};



struct Pixel_Range {

    size_t col_left;
    size_t col_right;

    size_t row_up;
    size_t row_down;

};

struct Raster_ALL {
    int ncols;
    int nrows;
    vector<Pixel_Data> ListeP;


};



class Raster_Tile
{

private:

Raster_Reader* Map;
vector<Pixel_Data> Liste_Points;
cv::Mat fireMap;
cv::Mat fireMap_bayes;
cv::Mat occupancy_map;
bool FireMap_modified;
sensor_model s_model;



public:

        //Raster_Tile() = default;

        Raster_Tile(const std::string& path);

        bool Test_point(size_t x,size_t y);
        double get_elevation(size_t x,size_t y);

        double get_maxheight();
        double get_minheight();
        double get_noDatavalue();
        double get_max_east();
        double get_max_west();
        double get_max_north();
        double get_max_south();

        void  set_fireMap(int row,int col,uchar value,bool use_occupancygrid);//the occupancy grid algorithme
        cv::Mat  get_fireMap();
        cv::Mat  get_fireMapbayes();
        void set_sensor_model(sensor_model sen_mod );
        bool  Test_fireMap_Modified();
        void get_DEM_info();


        void ListePoints_Data();
        Raster_ALL  get_ListePoints();

        Pixel_Range get_corners(double x,double y);///Calculates four corners in the dem matrix of the pixels that will be used to compute with in the raster, given the position of the camera centre.
        Pixel_Range get_Rastercorners(double x_left,double x_right,double y_up, double y_down);///Gets the equivalent pixel position for the coordinates of the corners .

        Pixel_Data All_data(int r,int c);
        void Put_firemap_inGdal(string gdal_result_path );

        static vector<Raster_Tile> get_allMaps(string folder){

            vector<Raster_Tile> Carte;

            ifstream fichier( folder.c_str(), ios::in);  // we open the file containing

            if(fichier)  // si l'ouverture a fonctionné
            {
                            string path;
                            while(getline(fichier, path))  // tant que l'on peut mettre la ligne dans "contenu"
                            {
                                    Raster_Tile RAST = Raster_Tile( path);
                                    Carte.push_back(RAST);
                            };
                            fichier.close();
            }
            else{
                cerr << "Impossible d'ouvrir le fichier !" << endl;
            };

            return Carte ;

        };

       //vector<Raster_Tile*> get_allMaps(string folder);
        //virtual ~Raster_Tile();

};




#endif // RASTER_TILE_H
