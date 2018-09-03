#include "../include/Esperce_Reader.h"

/// We have the option to choose between a default constructor where the path is defined once for all  ,or use a constructor that uses paths we chose in the definition of the object
Esperce_Reader::Esperce_Reader(){

    Path_Image = "/home/welarfao/images/Black and White/" ;//the path to reach the images to be mapped
    Path_Data = "/home/welarfao/fire-mapping/new_mapping/esperce3_metriccal.txt";//the file that holds the parameters of the flight

};

Esperce_Reader::Esperce_Reader( std::string&  path_Data, std::string&  path_Image){

    Path_Data = path_Data ;
    Path_Image = path_Image;

};

/*camera matrix K [3x3]
radial distortion [3x1]
tangential distortion [2x1]
camera position T [3x1]
camera rotation R [3x3]
camera model m = K [R|-Rt] X
*/


void Esperce_Reader::get_Dataset(){

  ifstream file( Path_Data.c_str() , ios::in);  // we open the file containing flight data

  if( file.is_open() )
  {

  string line;
  string Name;
  cv::Mat Translation ;/*InMeters*/
  cv::Mat Rotation;
  cv::Mat IntrinsicMatrix;
  float resize_ratio =1;
  vector<double> Radial_distortion(3);
  vector<double> Tangential_distortion(2);

  while(getline(file, line))  // tant que l'on peut mettre la ligne dans "line"
  {

    if( line.substr (0,3) == "IMG"){


        Name = line.substr (0,line.find_first_of(' ')) ;

        string path0 = Path_Image;
        std::string path = std::string(    path0.append(  std::string(Name) )     );

        cv::Mat Img = cv::imread(path,CV_LOAD_IMAGE_GRAYSCALE);
         if ( Img.data != NULL){//If the Image exists in the given path

           Translation = cv::Mat(3,1,CV_64FC1);
           Rotation=cv::Mat(3,3,CV_64FC1);
           IntrinsicMatrix=cv::Mat(3,3,CV_64FC1);
           /// Resizing the image so as it will take less time in being Mapped because we will reduce the number of pixels it holds,
           /// doing so implies that we multiply the intrinsic matrix too by the same coef.
           ///the infra-red pictures we will use might not be in need for this option because they will already be in a small size.
            if ((Img.rows > 1500) || (Img.cols > 1500)){
                            resize_ratio = 0.25;
                            cv::resize(Img, Img, cv::Size(), resize_ratio, resize_ratio); // resize image

           }


           for( int row = 0; row < 2; row++){///Having resized the Image we need to change the intrinsic Matrix that s why we multiply its two first lines with the same coef of the resize.

                std::getline( file, line,' ');
                IntrinsicMatrix.cv::Mat::at<double>(row,0) = resize_ratio*std::strtod(line.c_str(), NULL);


                std::getline(file, line,' ');
                IntrinsicMatrix.cv::Mat::at<double>(row,1) =resize_ratio*std::strtod(line.c_str(), NULL);

                std::getline(file, line);
                IntrinsicMatrix.cv::Mat::at<double>(row,2) = resize_ratio*std::strtod(line.c_str(), NULL);

            }
            std::getline( file, line,' ');
            IntrinsicMatrix.cv::Mat::at<double>(2,0) = std::strtod(line.c_str(), NULL);


            std::getline(file, line,' ');
            IntrinsicMatrix.cv::Mat::at<double>(2,1) = std::strtod(line.c_str(), NULL);

            std::getline(file, line);
            IntrinsicMatrix.cv::Mat::at<double>(2,2) = std::strtod(line.c_str(), NULL);

            //get radial distortion [3x1]

            std::getline(file, line,' ');
            Radial_distortion[0]= std::strtod(line.c_str(), NULL) ;

            std::getline(file, line,' ');
            Radial_distortion[1]=std::strtod(line.c_str(), NULL) ;

            std::getline(file, line);
            Radial_distortion[2]=std::strtod(line.c_str(), NULL) ;

            //get tangential distortion [2x1]

            std::getline(file, line,' ');
            Tangential_distortion[0]=std::strtod(line.c_str(), NULL) ;

            std::getline(file, line);
            Tangential_distortion[1]=std::strtod(line.c_str(), NULL) ;



            //get camera position T [3x1]

            std::getline(file, line,' ');
            Translation.at<double>(0) =  std::strtod(line.c_str(), NULL);

            std::getline(file, line,' ');
            Translation.at<double>(1) = std::strtod(line.c_str(), NULL);

            std::getline(file, line);
            Translation.at<double>(2) =  std::strtod(line.c_str(), NULL);



            //get camera rotation R [3x3]
            for( int row = 0; row < 3; row++){

                std::getline(file, line,' ');
                Rotation.at<double>(row,0) =  std::strtod(line.c_str(), NULL);

                std::getline(file, line,' ');
                Rotation.at<double>(row,1) =  std::strtod(line.c_str(), NULL);

                std::getline(file, line);
                Rotation.at<double>(row,2) =  std::strtod(line.c_str(), NULL);
            }


            Liste_Images.push_back(Image(Name, Img,Translation,Rotation,IntrinsicMatrix,Radial_distortion,Tangential_distortion));


          }

        }
      }
      file.close();

    }else cerr << "Can't open Data file!" << endl;


}


const vector<Image>&  Esperce_Reader::get_ListeImage(){

    return Liste_Images ;

}

/*

Esperce_Reader::~Esperce_Reader()
{
    //dtor
}
*/
