//
//  ofxEleksDraw.hpp
//  Created by Andrew Wallace on 11/7/19.
//

//A fair chunk of code in this project was ported from the Processing VST library by Trammell Hudson
//https://github.com/osresearch/vst

#ifndef ofxEleksDraw_hpp
#define ofxEleksDraw_hpp

#include "ofMain.h"
#include "Clipping.hpp"
#include "GCodePoint.h"

class ofxEleksDraw{
public:
    
    void setup();
    void clear();
    
    void draw();
    
    void generate_gcode();
    ofVec2f screen_point_to_plotter(float x, float y);
    void print();
    void save(string name);
    
    void set_pressure(float val);
    void set_speed(float val);
    
    void rect(float x, float y, float w, float h);
    
    void circle(float x, float y, float size);
    
    void start_shape();
    void vertex(float x, float y);
    void end_shape(bool close);
    
    void line(float x1, float y1, float x2, float y2);
    void line(float x1, float y1, float x2, float y2, bool lift_pen);
    
    void dot(float x, float y);
    
    void point(float x, float y, float speed, float pressure);
    
    ofVec2f getModelPoint(ofVec3f pnt);
    ofVec2f getModelPoint(float x, float y);
    
    void sort();
    
    float measureTransitDistance();
    
    
    float plotter_x_limit, plotter_y_limit;
    
    float speed;
    float max_speed;
    float pressure; //maybe getting rid of this
    
    int circle_resolution;
    
    vector<ofVec2f> shape_pnts;
    
    vector<string> commands;
    ofFbo fbo;
    
    Clipping clip;
    
    int last_x; //TODO: make these floats and check if values are very, very close
    int last_y;
    int last_pressure;
    int last_speed;
    
    vector<GCodePoint> list;
    
};

#endif /* ofxEleksDraw.hpp */
