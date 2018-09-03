#ifndef ESPERCE_READER_H
#define ESPERCE_READER_H


#include <fstream>

using namespace std;


#include "Image.h"


class Esperce_Reader
{
    private:

    std::string Path_Data;
    std::string Path_Image ;
    vector<Image> Liste_Images;


    public:

    Esperce_Reader();
    Esperce_Reader( std::string&  path_Data,  std::string&  path_Image);

    void get_Dataset();

    const vector<Image>& get_ListeImage();

    //virtual ~Esperce_Reader();

};

#endif // ESPERCE_READER_H
