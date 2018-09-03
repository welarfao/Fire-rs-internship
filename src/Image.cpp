#include "Image.h"

Image::Image(string InputImage,cv::Mat &Imat,cv::Mat Trans,cv::Mat Rot,cv::Mat Intr,vector<double> R_Dis,vector<double> T_Dis)
{
    Nom_Image = std::string(InputImage);//exp : IMG_0570.JPG
    CameraMatrix=cv::Mat(3,4,CV_64FC1);

    IMatrix = Imat;
    Translation = Trans;
    Rotation = Rot  ;
    IntrinsicPixelMatrix = Intr;
    Radial_distortion = R_Dis;
    Tangential_distortion =T_Dis;
    DistMatrix = (cv::Mat_<double>(1,5) <<  Radial_distortion[0],
                                            Radial_distortion[1],
                                            Tangential_distortion[0],
                                            Tangential_distortion[1],
                                            Radial_distortion[2]
                  );

    ncols=IMatrix.cols;
    nrows=IMatrix.rows;
    ///Remove distortion

    cv::undistort(IMatrix, undistortedImg, IntrinsicPixelMatrix, DistMatrix);

    ///Given that our positions are in meters,we need to have all our Matrix in meters ,so we multiply with 0.001 to get to m.(This transformation can, not be done since we devide u and v by z to get the homogeneous coordinates)

    IntrinsicMatrix=0.001*Intr;// intrinsic in meters

    /*
    This Matrix is given in a scale related to pixels ,so as to convert it to meters we will multiply it with the ratio between mm and pixel; wich is  0.001339453 m2
    we have
                        #Focal Length (mm) asuming a sensor width of 6.1722000000000001x4.6291500000000001mm
                        #Image size 4608x3456 pixel

                        6.1722000000000001 / 4608 = 0.001339453

     */

    IntrinsicMatrix = 0.001339453*IntrinsicMatrix; //intrinsic in mm

    /// Compute camera matix

    //http://ksimek.github.io/2012/08/22/extrinsic/
    //P = K [R|-Rt]
    // [R|-Rt] extrinsics

    cv::Mat Extrinsic ;
    /*
    /!\

     In our case we won't need to Transpose the Rotation Matrix ,its apparently already done in the parameters,
     in case the results of this code were t satisfaying trying to put the Tranpose Mtrix of the rotation might give the wanted results
     cv::transpose(Rotation, Rotation);
*/
    cv::hconcat(Rotation, -1*Rotation*Translation, Extrinsic );
    cv::Mat affineRow = (cv::Mat_<double>(1,4)<<0.0, 0.0, 0.0, 1.0);
    cv::vconcat(Extrinsic, affineRow, Extrinsic );
    //K Intrinsics

    cv::Mat zeroCol = (cv::Mat_<double>(3,1)<<0.0, 0.0, 0.0);
    cv::hconcat(IntrinsicMatrix, zeroCol, IntrinsicMatrix);

    CameraMatrix  = IntrinsicMatrix * Extrinsic;



}



//////////////////////////////////////////////////////////////////////////////////////////////////

string Image::get_name(){

return Nom_Image;
}


cv::Mat Image::get_CameraMatrix(){


    return CameraMatrix;
}
cv::Mat Image::get_IntrinsicMatrix(){


    return IntrinsicPixelMatrix;
}

cv::Mat Image::get_Rotation(){
return Rotation;
}

cv::Mat Image::get_Translation(){
return Translation;
}

cv::Mat Image::get_UndistortedImage(){


    return undistortedImg;

}
cv::Mat Image::get_IMatrix(){

    return IMatrix;


}


void Image::Segment(size_t thresholdValue){

    ///Segmentation

    cv::threshold(undistortedImg, undistortedImg, thresholdValue, 255, cv::THRESH_BINARY);
}

////////////////////////////////////////////////////////////////////////////////////////////
/*THis function Maps a point in the world in the image */

PixelImage Image::get_PixelImage(double x,double y,double z){

    PixelImage PI ;

    cv::Mat Pointworld = (cv::Mat_<double>(4,1)<<x, y,  z , 1.0);

    cv::Mat PointImage = CameraMatrix*Pointworld;
    //cout<< PointImage <<endl;


    PI.col = static_cast<int>(round(PointImage.at<double>(0) / (PointImage.at<double>(2)))); // divide by Z-cordinate
    PI.row = static_cast<int>(round(PointImage.at<double>(1) / (PointImage.at<double>(2)))); // divide by Z-cordinate

    return  PI;

}




//////////////////////////////////////////////////////////////////////////////////////////////////



Point3D Image::get_RayPosition(int u ,int v ,double Z){

  /*we know that :

  | u |      | x |                  | a1  b1  c1  d1 |                                        u = a1x + b1y + c1z + d1
  | v | = C  | y |     with    C =  | a2  b2  c2  d2 |   so we get this equations :           v = a2x + b2y + c2z + d2
  | z'|      | z |                  | a3  b3  c3  d3 |                                        z' = a3x + b3y + c3z + d3
             | 1 |


  u0=u/z' and v0 = v/z'

   u0 = a1x + b1y + c1z + d1 / a3x + b3y + c3z + d3
   v0 = a2x + b2y + c2z + d2 / a3x + b3y + c3z + d3

*/

   Point3D PW;

   double a1 =CameraMatrix.at<double>(0,0) ;
   double b1 =CameraMatrix.at<double>(0,1) ;
   double c1 =CameraMatrix.at<double>(0,2) ;
   double d1 =CameraMatrix.at<double>(0,3) ;
   double a2 =CameraMatrix.at<double>(1,0) ;
   double b2 =CameraMatrix.at<double>(1,1) ;
   double c2 =CameraMatrix.at<double>(1,2) ;
   double d2 =CameraMatrix.at<double>(1,3) ;
   double a3 =CameraMatrix.at<double>(2,0) ;
   double b3 =CameraMatrix.at<double>(2,1) ;
   double c3 =CameraMatrix.at<double>(2,2) ;
   double d3 =CameraMatrix.at<double>(2,3) ;


  // PW.x = (  (((b2-v*b3)/(u*b3-b1))*(c1-u*c3)+(c2-v*c3))*Z +((b2-v*b3)/(u*b3-b1))*(d1-u*d3)+(d2-v*d3)  ) / (   (v*a3-a2)*( 1 - ( (a1-u*a3)*(b2-v*b3) / ((u*b3-b1)*(v*a3-a2)) ) ));

   //PW.y =( (a1-u*a3)*PW.x + (c1-u*c3)*Z +(d1-u*d3) ) / (u*b3-b1) ;

   PW.x = (  (((b1-u*b3)/(b2-v*b3))*(v*c3-c2)+(c1-u*c3))*Z + ((b1-u*b3)/(b2-v*b3))*(v*d3-d2)+(d1-u*d3)  ) / (   (u*a3-a1)*( 1 - ( (v*a3-a2)*(b1-u*b3) / ((b2-v*b3)*(u*a3-a1)) ) ));
   PW.y =( (v*a3-a2)*PW.x + (v*c3-c2)*Z +(v*d3-d2) ) / (b2-v*b3) ;
   PW.z = Z;

    return  PW;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/* THis function tests if the pixel given exists in the IMage considering the number of rows and cols that the Image has*/

bool Image::Test_Image(PixelImage pxi){

  bool test=false;
  if (   pxi.col >= 0 && pxi.col <= IMatrix.cols){

    if (   pxi.row >= 0 && pxi.row <= IMatrix.rows ){

                    test = true;
    }
  }


  return test;

};

















/*
Image::~Image()
{
    //dtor
}
*/
