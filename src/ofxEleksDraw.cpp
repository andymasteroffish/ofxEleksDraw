//
//  ofxEleksDraw
//
//  Created by Andrew Wallace on 11/7/19.
//

#include "ofxEleksDraw.hpp"

void ofxEleksDraw::setup(){
    //set some defaults
    max_speed = 10000;
    speed = 5000;
    pressure = 70;
    circle_resolution = 50;
    
    plotter_x_limit = 80;
    plotter_y_limit = 80;
    
    clip.setup(ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight()));
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    
    clear();
}

void ofxEleksDraw::clear(){
    fbo.begin();
    ofClear(255,255,255);
    ofSetColor(255);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    fbo.end();
    
    commands.clear();
    commands.push_back("M3 S0");
    commands.push_back("G0 X0 Y0");
}

void ofxEleksDraw::draw(){
    ofSetColor(255);
    fbo.draw(0,0);
}

void ofxEleksDraw::print(){
    for (int i=0; i<commands.size(); i++){
        cout<<commands[i]<<endl;
    }
}

void ofxEleksDraw::save(string name){
    ofFile myTextFile;
    myTextFile.open(name+".nc",ofFile::WriteOnly);
    //myTextFile << "some text";
    for (int i=0; i<commands.size(); i++){
        myTextFile<<commands[i]<<endl;
    }
    //add some closing steps
    myTextFile<<"M3 S0"<<endl;
    myTextFile<<"G0 X0 Y0"<<endl;
    
    cout<<"SAVED"<<endl;
}

void ofxEleksDraw::set_pressure(float val){
    if (val < 0 || val > 100){
        cout<<"speed shuld be between 0 and 100"<<endl;
        return;
    }
    pressure = val;
}
void ofxEleksDraw::set_speed(float val){
    if (val < 1 || val > max_speed){
        cout<<"speed shuld be between 1 and "<<max_speed<<endl;
        return;
    }
    speed = val;
}

void ofxEleksDraw::rect(float x, float y, float w, float h){
    line(x,y, x+w, y, true);
    line(x+w,y, x+w, y+h, false);
    line(x+w,y+h, x, y+h, false);
    line(x,y+h, x, y, false);
}

void ofxEleksDraw::circle(float x, float y, float size){
    //vector<ofVec2f> pnts;
    float angle_step =(TWO_PI/(float)circle_resolution);
    start_shape();
    for (int i=0; i<circle_resolution; i++){
        ofVec2f pnt;
        float angle = angle_step  * i;
        
        pnt.x = x + sin(angle) * size;
        pnt.y = y + cos(angle) * size;
        vertex(pnt.x, pnt.y);
        //pnts.push_back(pnt);
    }
    end_shape(true);
    //shape(pnts, true);
}

void ofxEleksDraw::start_shape(){
    shape_pnts.clear();
}
void ofxEleksDraw::vertex(float x, float y){
    shape_pnts.push_back(ofVec2f(x,y));
}
void ofxEleksDraw::end_shape(bool close){
    for (int i=0; i<shape_pnts.size()-1; i++){
        line(shape_pnts[i].x, shape_pnts[i].y, shape_pnts[i+1].x, shape_pnts[i+1].y, i==0);
    }
    if (close){
        line(shape_pnts[shape_pnts.size()-1].x, shape_pnts[shape_pnts.size()-1].y, shape_pnts[0].x, shape_pnts[0].y, false);
    }
}

void ofxEleksDraw::line(float x1, float y1, float x2, float y2){
    line(x1,y1, x2,y2, true);
}

void ofxEleksDraw::line(float x1, float y1, float x2, float y2, bool lift_pen){
    ofVec2f p1 = getModelPoint(x1,y1);
    ofVec2f p2 = getModelPoint(x2,y2);
    
    if (!clip.clip(p1, p2)) {
        //cout<<"no part of this line is on screen"<<endl;
        return;
    }
    
    fbo.begin();
    
    float speed_prc = ofMap(speed, 1000, max_speed, 1, 0);  //setting an arbitrary min
    speed_prc = powf(speed_prc, 0.5);                       //smoothing this out since a medium speed still looks pretty black
    ofSetColor(0, speed_prc * 255);
    ofDrawLine(p1.x, p1.y, p2.x, p2.y);
    fbo.end();
    
    p1.x = ofMap(p1.x,0,ofGetWidth(), -plotter_x_limit, plotter_x_limit);
    p1.y = ofMap(p1.y,0,ofGetWidth(), plotter_y_limit, -plotter_y_limit);
    p2.x = ofMap(p2.x,0,ofGetWidth(), -plotter_x_limit, plotter_x_limit);
    p2.y = ofMap(p2.y,0,ofGetWidth(), plotter_y_limit, -plotter_y_limit);
    
    if (lift_pen){
        commands.push_back("M3 S0");
        commands.push_back("G0 X"+ofToString(p1.x)+" Y"+ofToString(p1.y) );
    }else{
        commands.push_back("G1 X"+ofToString(p1.x)+" Y"+ofToString(p1.y) +" F"+ofToString(speed) );
    }
    commands.push_back("M3 S"+ofToString(pressure));
    commands.push_back("G1 X"+ofToString(p2.x)+" Y"+ofToString(p2.y) +" F"+ofToString(speed));
    
}

void ofxEleksDraw::dot(float x, float y){
    ofVec2f pnt = getModelPoint(x,y);
    
    if (clip.check_point(pnt) == false){
        return;
    }
    
    fbo.begin();
    ofSetColor(0);
    ofDrawCircle(pnt.x, pnt.y, 1);
    fbo.end();
    
    pnt.x = ofMap(pnt.x,0,ofGetWidth(), -plotter_x_limit, plotter_x_limit);
    pnt.y = ofMap(pnt.y,0,ofGetWidth(), plotter_y_limit, -plotter_y_limit);
    
    commands.push_back("M3 S0");
    commands.push_back("G0 X"+ofToString(pnt.x)+" Y"+ofToString(pnt.y) );
    commands.push_back("M3 S"+ofToString(pressure));
    commands.push_back("M3 S0");
}



//This function is by Andy, it attempts to recreate the functionality of modelX() and modelY() in Processing
//Currently it only works in 2D. 3D transformations will break it.
//it sometimes gets locked at 90 degree angles when the actual angle is 89 or 91. Not sure why
//it could definitely be more efficient by using quaternions properly
ofVec2f ofxEleksDraw::getModelPoint(ofVec3f pnt){
    return getModelPoint(pnt.x, pnt.y);
}
ofVec2f ofxEleksDraw::getModelPoint(float x, float y){
    //get the model of th current matrix
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    ofMatrix4x4 mat(m);
    
    //check if this model matches the baseline, no matrix model and avoid a lot of unecessary work if it does
    ofVec4f baseline[4];
    baseline[0] = ofVec4f(1,0,0,0);
    baseline[1] = ofVec4f(0,1,0,0);
    baseline[2] = ofVec4f(0,0,1,0);
    baseline[3] = ofVec4f(-ofGetWidth()/2,-ofGetHeight()/2,-1,1);   //the z value is the wildcard. I'm not sure how it is set
    
    bool matches = true;
    for (int i=0; i<4; i++){
        if (mat._mat[i].x != baseline[i].x) matches = false;
        if (mat._mat[i].y != baseline[i].y) matches = false;
        if (mat._mat[i].z != baseline[i].z && i!=3) matches = false;
        if (mat._mat[i].w != baseline[i].w) matches = false;
    }
    
    //if it all matches the baseline, we're not in a matrix and can just return the input values
    if (matches){
        //cout<<"nothing doing"<<endl;
        return ofVec2f(x,y);
    }
    
    //get the model of the default screen (this can't be used as the baseline above for reasons I don't totally understand)
    ofMatrix4x4 ident = ofMatrix4x4::newIdentityMatrix();
    
    //extract info from that
    ofVec3f trans_val = mat.getTranslation();
    ofVec3f scale_val = mat.getScale();
    ofQuaternion quat = mat.getRotate();
    ofVec3f rot_val = quat.asVec3();
    ofVec3f euler = quat.getEuler();
    
    
    ofPoint origPoint = ofPoint(x,y);
    ofVec2f scaled_point;
    scaled_point.x = origPoint.x * scale_val.x;
    scaled_point.y = origPoint.y * scale_val.y;
    
    float euler_deg = euler.z;
    if (euler.x != 0){  //the euler degreees start counting back from 90 and the x and y values of rot_val go to 180
        euler_deg = 90 + (90-euler_deg);
    }
    float mat_angle = ofDegToRad (euler_deg);
    float base_angle = atan2(scaled_point.x, scaled_point.y);   //shouldn't this be y then x??? Not sure why this works
    float angle = base_angle - mat_angle;
    
    float dist = scaled_point.length();
    
    ofVec2f return_val;
    return_val.x = sin(angle) * dist +  (ofGetWidth()/2 + trans_val.x);
    return_val.y = cos(angle) * dist + (ofGetHeight()/2 + trans_val.y);
    
    return return_val;
}
