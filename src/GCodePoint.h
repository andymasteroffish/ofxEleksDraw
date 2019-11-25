//
//  GCodePoint.h
//  gcode_lib_test
//
//  Created by Andrew Wallace on 11/25/19.
//

#ifndef GCodePoint_h
#define GCodePoint_h

class GCodePoint{
public:
    //TODO: these should be floats
    int x;
    int y;
    int speed;
    int pressure;
    
    inline GCodePoint( float _x, float _y, float _speed, float _pressure ):x(_x), y(_y), speed(_speed), pressure(_pressure) {}
    
    bool equals(GCodePoint other){
        return x==other.x && y==other.y && speed==other.speed && pressure==other.pressure;
    }
};


#endif /* GCodePoint_h */
