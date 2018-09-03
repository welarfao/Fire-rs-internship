#include "Raster_Tile.h"


Raster_Tile::Raster_Tile(const std::string& path_DEM){

     Map = new Raster_Reader( path_DEM );
     Map->geoTransform();
     Map->Put_Raster_invector();

     fireMap = cv::Mat::zeros(Map->nRows-1,Map->nCols-1,CV_8UC1);
     fireMap_bayes = cv::Mat::zeros(Map->nRows-1,Map->nCols-1,CV_8UC1);
     occupancy_map = cv::Mat::zeros(Map->nRows-1,Map->nCols-1,CV_32FC1);


     FireMap_modified = false;


     cout<<"This map's range is : \n \t West "<<Map->max_west <<  "  \t  East "<< Map->max_east << endl;

     printf(   " \n \t North %.6f  South %.6f ",Map->max_north,Map->max_south );

     cout<<"\n no data value :"<<Map->no_data_value<<endl;
};


void  Raster_Tile::set_fireMap(int row,int col,uchar value,bool use_occupancygrid){

     FireMap_modified = true;
     fireMap.cv::Mat::at<uchar>(row,col) = (uchar)value ;

    if(!use_occupancygrid){

        fireMap.cv::Mat::at<uchar>(row,col) = (uchar)value ;

    }else{

        double probabilite_de_feu = value/255.0;
        double logOdd = log(  s_model.sensor_model_HavingFire(probabilite_de_feu)/ s_model.sensor_model_notHavingFire(probabilite_de_feu)  );
        occupancy_map.cv::Mat::at<double>(row,col) += logOdd  ;
        cout<<occupancy_map<<endl;

        //cout <<probabilite_de_feu<<endl;
        //cout <<occupancy_map.cv::Mat::at<double>(row,col)<<endl;
        //cout <<exp(occupancy_map.cv::Mat::at<double>(row,col))<<endl;
        //cout <<(1-1/( exp(occupancy_map.cv::Mat::at<double>(row,col))+1  ) )<<endl;
        if(  (1-1/( exp(occupancy_map.cv::Mat::at<double>(row,col))+1  ) )  <  0 ){

          fireMap_bayes.cv::Mat::at<uchar>(row,col) = (uchar) 255*0 ;

        }else if((1-1/( exp(occupancy_map.cv::Mat::at<double>(row,col))+1  ) )  >  1 ){

          fireMap_bayes.cv::Mat::at<uchar>(row,col) = (uchar) 255*1 ;
        }else{
          fireMap_bayes.cv::Mat::at<uchar>(row,col) = static_cast<uchar>(255*(1-1/( exp(occupancy_map.cv::Mat::at<double>(row,col))+1  )));

        }
    }

}

void Raster_Tile::set_sensor_model(sensor_model sen_mod ){

    s_model = sen_mod ;


}
cv::Mat Raster_Tile::get_fireMap(){

        FireMap_modified = false;

        return fireMap.clone();

}
cv::Mat Raster_Tile::get_fireMapbayes(){

        FireMap_modified = false;

        return fireMap_bayes.clone();

}


double  Raster_Tile::get_max_west(){

        return   Map->max_west;

}
double    Raster_Tile::get_max_east(){

        return  Map->max_east;

}

double   Raster_Tile::get_max_north(){

        return Map->max_north ;

}

double   Raster_Tile::get_max_south(){

        return Map->max_south ;

}


void Raster_Tile::get_DEM_info(){


   if( Map->get_projection() != "" ){

     string line;
     std::stringstream ss;
     ss.str(Map->get_projection());
     while(std::getline( ss, line,',')){

        cout<<line<<endl;

     }
   }
}
////////////////////////////////////////////////////
void Raster_Tile::Put_firemap_inGdal(string gdal_result_path ){

    Map->Put_in_Raster(fireMap,gdal_result_path);

}


//////////////////////////////////////////////////////
bool Raster_Tile::Test_point(size_t x,size_t y) {//checking if the position demanded exists in the map we have as an entry

    bool testing= false;

    if (   x>= Map->max_west && x< Map->max_east ){

        if (   y> Map->max_south && y<= Map->max_north ){

                    testing = true;
        }
    }

    return testing;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Raster_Tile::get_elevation(size_t x,size_t y) {// x and y are the coordinates of a  position in the world coord.

    Pixel Elevation;
    double Height;

    Elevation = Map->get_pixel(x,y) ;

    Height = Map->get_height(Elevation.col,Elevation.row ) ;

    return Height ;


}
////////////////////////////////////////////////////
double Raster_Tile::get_maxheight(){

    return Map->maxheight;
}

double Raster_Tile::get_minheight(){

    return Map->minheight;

}
double  Raster_Tile::get_noDatavalue(){

    return Map->no_data_value;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//THis function just gives back the equivalent pixels for the 4 corners given in wolrd coordinates
Pixel_Range Raster_Tile::get_Rastercorners(double x_left,double x_right,double y_up, double y_down){

    Pixel_Range PR;

    Pixel pex=  Map->get_pixel(x_left,y_up);
    PR.col_left=pex.col;
    PR.row_up =pex.row;

    pex =  Map->get_pixel(x_right,y_down);

    PR.col_right=pex.col;
    PR.row_down=pex.row;

    return PR;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
THis function is used with the versiom of the function Map() that doesn t use  raytracing,
it uses the position of the camera once the picture is taken to approximate the ROI

*/
Pixel_Range Raster_Tile::get_corners(double x,double y){


    Pixel_Range pr;
    int ncol = Map->nCols - 1;//Taking the centre of every pixel in the raster decreases the size of our matrix by one column and row.
    int nrow = Map->nRows -1;
    int f=3;//it s the factor that defines how much away we ll be from the position of th image


    Pixel px = Map->get_pixel(x,y) ;

    if( (int)(px.col - ncol/f ) < 0  ){

        pr.col_left  = 0 ;
        pr.col_right =(int)(px.col + ncol/f) ;


    }else if(     (int)(px.col + ncol/f ) > ncol-1  ){

        pr.col_left  = (int)(px.col - ncol/f) ;
        pr.col_right = ncol -1 ;


    }else{

        pr.col_right = (int)(px.col + ncol/f) ;
        pr.col_left  = (int)(px.col - ncol/f) ;

    }

    if( (int)(px.row - nrow/f ) < 0  ){

        pr.row_up = 0;
        pr.row_down = (int)(px.row + nrow/f );

    }else if(   (int)(px.row + nrow/f ) > nrow  ){

        pr.row_up = (int)(px.row - nrow/f );
        pr.row_down = nrow -1 ;


    }else{

        pr.row_up = (int)(px.row - nrow/f) ;
        pr.row_down = (int)(px.row + nrow/f) ;

    }

    return pr ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*      For each pixel the position in  the world we have with the geotrnsform in GDAL is the position of the upper left corner ,
but the height we have is for the centerof the pixel so for the Mapping we will use the centre of 4 pixels to creat a pixel
that we know the height of its corners

                                         -----------------------------
                                        | Pixel Raster||Pixel Raster |
                     pt_upright --------|----->       ||     <-------|--------pt_upright
                                        |             ||             |
                                        |             ||             |
                                        ------------- ----------------
                                        | Pixel Raster||Pixel Raster |
                    pt_downleft --------|----->       ||     <-------|--------downright
                                        |             ||             |
                                        |             ||             |
                                        ------------------------------


*/

Pixel_Data Raster_Tile::All_data(int r,int c){

    Pixel_Data pt;

    Point2D pt_upleft = Map->get_World_coord( c+1/2,r+1/2 );
    Point2D pt_upright = Map->get_World_coord( c+1+1/2,r+1/2 );
    Point2D pt_downleft = Map->get_World_coord( c+1/2,r+1+1/2 );
    Point2D pt_downright = Map->get_World_coord( c+1+1/2,r+1+1/2);

    pt.x_upleft = pt_upleft.x;
    pt.y_upleft= pt_upleft.y;
    pt.z_upleft= Map->get_height(c,r) ;

    pt.x_upright= pt_upright.x;
    pt.y_upright= pt_upright.y;
    pt.z_upright= Map->get_height(c+1,r) ;

    pt.x_downleft= pt_downleft.x;
    pt.y_downleft= pt_downleft.y;
    pt.z_downleft= Map->get_height(c,r+1) ;

    pt.x_downright= pt_downright.x;
    pt.y_downright= pt_downright.y;
    pt.z_downright= Map->get_height(c+1,r+1) ;

    pt.col = c;
    pt.row = r;



    return pt;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Raster_Tile::ListePoints_Data(){

    Pixel_Data pt;

     for(int r = 0; r < Map->nRows -1 ;r++){

        for(int c = 0  ; c < Map->nCols -1 ; c++ ){

          pt = All_data(r,c);

          Liste_Points.push_back(pt);
        }
    }

}



Raster_ALL  Raster_Tile::get_ListePoints(){

    Raster_ALL  all;
    all.ncols=Map->nCols;
    all.nrows=Map->nRows ;
    all.ListeP = Liste_Points;


    return all;

}

bool  Raster_Tile::Test_fireMap_Modified(){

    return FireMap_modified;

}



/*

Raster_Tile::~Raster_Tile()
{//dtor

 };
*/
