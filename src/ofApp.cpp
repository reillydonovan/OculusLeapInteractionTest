//
//      Oculus / Leap interaction prototype
//      created by Jason Walters
//      last edited on November 7, 2013
//
//      made with openFrameworks 0.8
//
//--------------------------------------------------------------

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground( 0 );
    ofSetVerticalSync( true );
	
    leap.open();                        // open our leap motion device
	oculusRift.baseCamera = &cam;       // setup our oculus cam
	oculusRift.setup();                 // open our oculus rift
	
	//enable mouse;
	cam.begin();
	cam.end();
    
    boxGrow = 300;                      // white boxes, default size
    r = 100;                            // default cube color is grey
    g = 100;                            // default cube color is grey
    b = 100;                            // default cube color is grey
    rRing = 40;                         // default red node ring size is 40
    gRing = 40;                         // default green node ring size is 40
    bRing = 40;                         // default blue node ring size is 40
    starMaxZ = 2000;                    // max star field z position
    
    // setup our star field
    for (int i = 0; i < 1000; i++) {
        stars[i].pos = ofVec3f(ofRandomWidth(), ofRandomHeight(), ofRandom(-2000, 2000));
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    leapUpdates();
    leapControls();
}

//--------------------------------------------------------------
void ofApp::leapUpdates()
{
    // new frame, clear finger & hand count
    fingersFound.clear();
    hands.clear();
    fingerCount = 0;
    
    // hand detection / tracking
    // --------------------------------------------
    ofPoint handPos;
    hands = leap.getLeapHands();
    
	if( leap.isFrameNew() && hands.size() )
    {
        // map leap's coordinates to our world
        leap.setMappingX(-100, 100, -200, 200);
        leap.setMappingY(250, 350, -200, 200);
        leap.setMappingZ(-150, 150, -200, 200);
        
		for(int i = 0; i < hands.size(); i++)
        {
            // will crash if no hands are detected
            if (hands.size() > 0)
            {
                handPos = leap.getMappedofPoint( hands[0].palmPosition() );
                
                handsVel.set(hands[0].palmVelocity().x, hands[0].palmVelocity().y, hands[0].palmVelocity().z);
                handsPos.set(handPos.x, handPos.y, handPos.z);
                handsPos.z = ofClamp(handsPos.z, -200, 200);
                
                // get our finger count
                for(int j = 0; j < hands[i].fingers().count(); j++)
                {
                    const Finger & finger = hands[i].fingers()[j];
                    fingersFound.push_back(finger.id());
                    fingerCount = fingersFound.size();
                }
            }
		}
	}
    
    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
	leap.markFrameAsOld();
}

//--------------------------------------------------------------
void ofApp::leapControls()
{
    if (fingerCount > 0 && fingerCount < 3)
    {
        if (handsPos.z < 0) {
            // when finger is in range, grow white boxes
            boxGrow+=10.0;
            if (boxGrow > 600.0) {
                boxGrow = 600.0;
            }
        }
        else {
            // when finger is behind z, decrease white boxes to default size
            boxGrow-=20.0;
            if (boxGrow < 300.0) {
                boxGrow = 300.0;
            }
        }
    }
    else if (fingerCount > 2 || (hands.size() > 0 && fingerCount == 0))
    {
        // decrease the white boxes to default size
        boxGrow-=20.0;
        if (boxGrow < 300.0)
        {
            boxGrow = 300.0;
        }
        
        // x rotation
        if (handsPos.x < -20)
        {
            rX = ofMap(handsPos.x, -200, -20, -16, 0);
            rX = ofClamp(rX, -16, 0);
            rotY += rX;
            
        }
        else if (handsPos.x > 20)
        {
            rX = ofMap(handsPos.x, 20, 200, 0, 16);
            rX = ofClamp(rX, 0, 16);
            rotY += rX;
        }
        
        // y rotation
        if (handsPos.y < -20)
        {
            rY = ofMap(handsPos.y, -200, -20, -16, 0);
            rY = ofClamp(rY, -16, 0);
            rotX += rY;
            
        }
        else if (handsPos.y > 20)
        {
            rY = ofMap(handsPos.y, 20, 200, 0, 16);
            rY = ofClamp(rY, 0, 16);
            rotX += rY;
        }
        
        // forward movement, fly through star field
        if (handsPos.z < 0)
        {
            float starSpeed = ofMap(handsPos.z, -50, -0, -32, 0);
            starSpeed = ofClamp(starSpeed, -32, 0);
            
            for (int s = 0; s < 1000; s++) {
                stars[s].pos.z -= starSpeed;
                
                if (stars[s].pos.z > starMaxZ) {
                    // once max z is reached, reset position to create space
                    stars[s].pos.x = ofRandomWidth();
                    stars[s].pos.y = ofRandomHeight();
                    stars[s].pos.z = -starMaxZ;
                }
            }
        }
    }
    else {
        boxGrow-=20.0;
        if (boxGrow < 300.0) {
            boxGrow = 300.0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
	glEnable(GL_DEPTH_TEST);
	if(oculusRift.isSetup())
    {
		oculusRift.beginLeftEye();
		drawScene();
		oculusRift.endLeftEye();
		
		oculusRift.beginRightEye();
		drawScene();
		oculusRift.endRightEye();
		
		oculusRift.draw();
	}
	else
    {
		cam.begin();
		drawScene();
		cam.end();
	}
	
    // write some values to screen
    ofSetColor(255);
    ofDrawBitmapString("press f for fullscreen", 20,20);
	ofDrawBitmapString("frameRate = " + ofToString(ofGetFrameRate()), 20,60);
    ofDrawBitmapString("fingerCount = " + ofToString(fingerCount), 20,80);
    ofDrawBitmapString("handPos X = " + ofToString(handsPos.x), 20,100);
    ofDrawBitmapString("handPos Y = " + ofToString(handsPos.y), 20,120);
    ofDrawBitmapString("handPos Z = " + ofToString(handsPos.z), 20,140);
}

//--------------------------------------------------------------
void ofApp::drawScene()
{    
    // gradient background - 2d
    ofPushMatrix();
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    ofBackgroundGradient(ofColor(25.0, 25.0, 25.0), ofColor(0.0,0.0,0.0), OF_GRADIENT_CIRCULAR);
    ofPopMatrix();
    
    // draw our content objects
    drawStarField();            // stars are awesome
    drawCubeObject();           // draw our center cube thingy
    drawSelectionNodes();       // draw our selection nodes
    drawLeapCursor();           // draw our leap cursor
}

//--------------------------------------------------------------
void ofApp::drawLeapCursor()
{
    ofShowCursor();
    ofSetColor(0, 255, 0);
    ofDrawSphere(handsPos.x, handsPos.y, handsPos.z, 10);    
}

//--------------------------------------------------------------
void ofApp::drawStarField()
{
    ofPushMatrix();
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    ofSetColor(255, 255, 255);
    for (int i = 0; i < 1000; i++) {
        ofRect(stars[i].pos.x, stars[i].pos.y, stars[i].pos.z, 2.0, 2.0);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawCubeObject()
{
    ofPushMatrix();
    ofTranslate(0.0, 0.0, -200.0);
    ofRotateX(-rotX);
    ofRotateY(rotY);
    ofSetColor( r, g, b );
    ofDrawBox( 200.0 );
    ofSetColor( 255, 255, 255 );
    ofDrawBox(0.0, 0.0, 0.0, 100.0, 100.0, boxGrow);
    ofDrawBox(0.0, 0.0, 0.0, boxGrow, 100.0, 100.0);
    ofDrawBox(0.0, 0.0, 0.0, 100.0, boxGrow, 100.0);
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawSelectionNodes()
{
    // if our finger is in place, do stuff per node...
    if (fingerCount > 0 && fingerCount < 3) {
        if (handsPos.y > -350.0 && handsPos.y < -250.0) {
            // red node
            if (handsPos.x > -350.0 && handsPos.x < -250.0) {
                r = 255;
                g = 0;
                b = 0;
                rRing+=1;
            }
            else {
                rRing = 40;
            }
            
            // green node
            if (handsPos.x > -50.0 && handsPos.x < 50.0) {
                r = 0;
                g = 255;
                b = 0;
                gRing+=1;
            }
            else {
                gRing = 40;
            }
            
            // blue node
            if (handsPos.x > 250.0 && handsPos.x < 350.0) {
                r = 0;
                g = 0;
                b = 255;
                bRing+=1;
            }
            else {
                bRing = 40;
            }
        }
        else {
            // if hand's are away, reset to default color
            r = 100;
            g = 100;
            b = 100;
        }
    }
    else {
        // if no hands
        // reset to default color
        r = 100;
        g = 100;
        b = 100;
        
        // reset ring size to default
        rRing = 40;
        gRing = 40;
        bRing = 40;
    }
    
    // draw red node
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofCircle(-300, -300, rRing);
    ofFill();
    ofDrawSphere(-300, -300, 0, 40);
    
    // draw green node
    ofSetColor(0, 255, 0);
    ofNoFill();
    ofCircle(0, -300, gRing);
    ofFill();
    ofDrawSphere(0, -300, 0, 40);
    
    // draw blue node
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofCircle(300, -300, bRing);
    ofFill();
    ofDrawSphere(300, -300, 0, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if( key == 'f' )
	{
		//gotta toggle full screen for it to be right
		ofToggleFullscreen();
	}
	
	if(key == 's'){
		oculusRift.reloadShader();
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
    leap.close();       // close leap on exit, prevents crashing
}