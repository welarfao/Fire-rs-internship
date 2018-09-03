#ifndef SENSOR_MODEL_H
#define SENSOR_MODEL_H



#include <math.h>
#include <iostream>


class sensor_model
{
  public:
    sensor_model();
    virtual double sensor_model_HavingFire(double x);
    virtual double sensor_model_notHavingFire(double x);

};


/*
class plynome_sensor_model:public sensor_model(){

  public:
    plynome_sensor_model(float ax3,float bx2,float cx,float d);
    double sensor_model_HavingFire(double x);
    double sensor_model_notHavingFire(double x);


}


*/
























#endif // SENSOR_MODEL_H
