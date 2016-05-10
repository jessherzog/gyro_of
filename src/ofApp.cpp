#include "ofApp.h"
#include <string>


// sounds:
// Setuniman Everyday 1D16
// PaulMorek Nom A
// frosthardr balloon
// Robinhood76 02098 cartoon laughter
// Video: Lauren"s Cartoon (youtube)

//  the above files are kept in the bin/data directory of our project

// initialize the objects we will use
ofTrueTypeFont myfont;
ofFbo fbo;
ofVideoPlayer movie;
ofSerial serial;

string staging;  // buffer for strings from serial port
int w, h, X, Y, Z, midX, midY, avatarX, avatarY, avatarZ = 10;
/* GLOBAL VARIABLES
 * w, h: width and height of screen
 * midX, midY: midpoints of screen
 * X, Y, Z: 3 axis gyro values coming from arduino on serial port
 * avatarX,Y,Z: X,Y,and radius of our avatar (the red dot of unending hunger)
 */

// Our one time setup for the program
void testApp::setup(){
  
  // setup globals
  w = ofGetWidth();
  h = ofGetHeight();
  midX = ofGetWidth()/2;
  midY = ofGetHeight()/2;
  avatarX = midX; // start the circle in the middle
  avatarY = midY;

  // setup font
  myfont.loadFont("arial.ttf", 12);
  
  // setup image
  gyroNom.loadImage("gyro.png");
  
  // set up the serial device
  //serial.setup("/dev/ttyACM0", 9600); // load a specific serial port when not on RPi
  serial.setup(0, 9600); //open device 0 @ 9600 baud

  // setup movie
  movie.load("eatSplosion.m4v");
  movie.setLoopState(OF_LOOP_NORMAL);
  movie.play();

  // setup sounds
  char fName[20];  // create file names to load from ints
  for (int i = 0; i < numSounds; i++) {
    sprintf (fName, "%d.wav", i);
    mySound[i].loadSound(fName);
  }
  
  backgroundMusic.loadSound("background.wav");
  laugh.loadSound("laugh.wav");
  stretch.loadSound("stretch.wav");
  stretch.setVolume(.2);  // make the stretch sound quieter than NOM NOM NOM
  
  while (!backgroundMusic.isLoaded());  // block till loaded
  backgroundMusic.setLoop(true);
  backgroundMusic.play();
  
  // limit drawn frames per sec
  ofSetFrameRate( 60 );
}

// This takes a delimiter and a string.  It will look for the delimiter, go one character beyond and convert the first 
// number it sees in the string to an integer.  This will be used for the strings coming from the arduino"s gyro.
int parseInt (string del, string str) {

  std::size_t found = str.find(del);
  int rv = atoi(str.c_str() + found + del.length());
  return (rv);
}

// update all the variables and data in the program!
void testApp::update(){

  for (int i = 0; i < numNPPoints; i++) { // update all the points
    NPPUpdate(i);
  }
  
  movie.update();
  
  // uncommenting the following will
  // look for strings from the arduino
  // if we read too many characters before the line end (\n)
  // (ttl) assume the string is bad and disregard it
  // bug: should read all chars in buffer

#if 0
  int ttl = 100; // read x chars MAX..  if more available... throw em out!
  char ch;
  while ((ch=serial.readByte())>0 && ttl-->0 && ch != '\n') {
    staging += ch;
  }

  if (ch == '\n') { // if we are done, create string to print

    // parse gyro input by delimiters

    X = parseInt("X:", staging); // relative value of x 
    Y = parseInt("Y:", staging); // relative value of y 
    Z = parseInt("Z:", staging); // relative value of z
    
    char buffer[300];
    sprintf(buffer, "%d %d %d", X, Y, Z);

    avatarX += X;
    avatarY += Y + 3;  // my Y is -3 at rest... so normalize it here!
    
    if (Z > 250) { // doing excercise!
      avatarZ = 10; 
      laugh.play();
    }
    
    avatarX = ofClamp(avatarX, 0, ofGetWidth());
    avatarY = ofClamp(avatarY, 0, ofGetHeight());
    staging = "";

  }
#endif

  // check for collisions between avatar and particle
  for (int i = 0; i < numNPPoints; i++) { 
  
    if (NPPCollisionDetection(i)) { 
      // printf("boop! I hit you...\n");
      
      NPPAlive[i] = false;  				// food is eaten... forever gone... drawn no more
      avatarZ += 10;         				// make avatar bigger
      avatarZ = ofClamp(avatarZ, 10, 300);  // make an upper limit to biggitude
      mySound[int(ofRandom(0,9))].play();   // play random eating sound
      stretch.play();						// play stretch sound
    }
  } 
}

// set the avatar to mouse x,y
void testApp::mouseMoved(int x, int y){

    avatarX = x;
    avatarY = y;

}

// look for key being pressed
void testApp::keyPressed(int key){
  if (key == 'z') { // weight reduction
    avatarZ = 10;
    laugh.play();
  }
}

// draw the whole scene <framerate> times a second
void testApp::draw(){
  
  // draw background
  ofColor light = ofColor(128,128,150);
  ofColor dark = ofColor(220,220,250);
  ofBackgroundGradient(light, dark, OF_GRADIENT_LINEAR);
 
  ofSetColor(0, 0, 255); // write info text in blue font  
  myfont.drawString("Sky Gyros: The GAME!! \n\n"
                    "As you can see, after the rainy Monday I had gyros \n"
                    "on my mind and I ate waaaaaay to much.  The game \n"
                    "can be played with a mouse and 'Z' key for 'excercise', \n"
                    "or via the Arduino with attached gyro.  Moving the \n"
                    "gyro's X and Y move your avatar, and dramatically \n"
                    "twisting in Z does excercise.\n\n"
                    "There is no winning or losing... Tasty gyros make everyone a winner!", 10,20);

  // draw avatar
  ofSetColor(255,0,0);
  ofCircle (avatarX, avatarY, avatarZ);
  
  // draw each particle
  for (int i = 0; i < numNPPoints; i++) { // draw all the points
    NPPDraw(i); 
  }
  
  if (avatarZ > 250)  // if I have eaten my fill of gyros play explosion video
    movie.draw(avatarX - (movie.getWidth()/2),avatarY - (movie.getHeight()/2));  

}
