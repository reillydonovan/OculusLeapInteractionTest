#pragma once

#include "ofMain.h"
#include "ofxOculusRift.h"
#include "ofxLeapMotion.h"

class ofApp : public ofBaseApp
{
public:
	
	void setup();
	void update();
	void draw();
    void exit();
	
	void drawScene();
    void leapUpdates();
    void leapControls();
    void drawLeapCursor();
    void drawSelectionNodes();
    void drawStarField();
    void drawCubeObject();
	
	void keyPressed(int key);
    
	ofxOculusRift		oculusRift;
    ofxLeapMotion       leap;
	ofLight				light;
	ofEasyCam			cam;
    
    ofTrueTypeFont      fontBook;
    ofTrueTypeFont      fontMedium;
    ofTrueTypeFont      fontItalic;
    
    // leap motion
    vector <Hand> hands;
    ofVec3f handsVel;
    ofVec3f handsPos;
    vector<int> fingersFound;
    int fingerCount;
    
    // objects we can touch
    float boxGrow;
    float rotX, rotY;
    float rX, rY;
    int r,g,b;
    int rRing, gRing, bRing;
    
    // star field stuff
    typedef struct
    {
        ofVec3f pos;
    }Star;
    Star stars[1000];
    int starMaxZ;
};