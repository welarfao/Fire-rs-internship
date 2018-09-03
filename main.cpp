
#include "Mapping.h"
#include "Esperce_Reader.h"
#include <string>
#include <fstream>
using namespace std;


/*     Esperce Reader Test


Esperce_Reader Esp = Esperce_Reader();
Esp.get_Dataset();

const vector<Image>& IM = Esp.get_ListeImage();
cout<<IM.size()<<endl;

Image IM1 =IM[0] ;
cout<<IM1.get_name()<<endl;
cout<< IM1.get_CameraMatrix()<<endl;

*/
/*
class better_sensor_model : sensor_model {

    double sensor_model_HavingFire(double x){

     return ;
    };
    double sensor_model_notHavingFire(double x){

     return ;
    };
}

*/
int main()
{

  std::string path_Data = "/home/welarfao/fire-mapping/new_mapping/esperce3_metriccal.txt";
  std::string path_Image = "/home/welarfao/images/Black and White/";
  std::string path_DEM = "DEM.txt";//we chose to give a file that holds the paths of all the DEM knowing that in the real case we will need more than one DEM

  Esperce_Reader Esp = Esperce_Reader( path_Data, path_Image);
  Esp.get_Dataset();
  vector<Image> Images_to_Map= Esp.get_ListeImage();

  Mapping Mp =  Mapping(path_DEM,0,1);//first bool :FALSE direct ; TRUE with the vector of points // second bool:FALSE without segmentation /TRUE with

  Mp.set_threshold(240);

  for(size_t i = 0; i < Images_to_Map.size(); i++){

    Mp.Map(Images_to_Map[i]);
    Mp.Save_Show_FireM("/home/welarfao/results/");

  }


  cout << "Hello world!" << endl;

    return 0;
}


