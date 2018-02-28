#include "ofApp.h"
#include <vector>

// int xAxisMovement = 0;
// int yAxisMovement = 0;

//---------------------------------------------------
void extractIntegers(string str, int *intFoundArray) {
  stringstream bufferStringStream;
  bufferStringStream << str;

  string temp;
  int foundInteger;
  vector<int> result;

  while (!bufferStringStream.eof()) {
    bufferStringStream >> temp;
    if (stringstream(temp) >> foundInteger) {
      result.push_back(foundInteger);
    }
    temp = "";
  }

  // Order pair input case
  if (result.size() == 2) {
    intFoundArray[0] = result.at(0);
    intFoundArray[1] = result.at(1);
  }
  // Button input case
  else if (result.size() == 4) {
    for (int x = 0; x < 4; x++) {
      intFoundArray[x] = result.at(x);
    }
  }
}

//--------------------------------------------------------------
void rotateToNormal(ofVec3f normal) {
  normal.normalize();

  float rotationAmount;
  ofVec3f rotationAngle;
  ofQuaternion rotation;

  ofVec3f axis(0, 0, 1);
  rotation.makeRotate(axis, normal);
  rotation.getRotate(rotationAmount, rotationAngle);
  ofRotate(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
}

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetVerticalSync(true);
  // draw the vertices in pathLines as a line strip
  pathLines.setMode(OF_PRIMITIVE_LINE_STRIP);

  bSendSerialMessage = false;
  ofBackground(255);
  ofSetLogLevel(OF_LOG_VERBOSE);

  font.load("DIN.otf", 64);

  serial.listDevices();
  vector<ofSerialDeviceInfo> deviceList = serial.getDeviceList();

  int baud = 56000;
  serial.setup("COM17", baud); // Open device on COM17

  nTimesRead = 0;
  nBytesRead = 0;
  readTime = 0;
  memset(bytesReadString, 0, 16);
}

//--------------------------------------------------------------
void ofApp::update() {
  // ============== Communication ============================
  nTimesRead = 0;
  nBytesRead = 0;
  int nRead = 0; // a temp variable to keep count per read

  unsigned char bytesReturned[20];
  memset(bytesReadString, 0, 16);
  memset(bytesReturned, 0, 20);
  while ((nRead = serial.readBytes(bytesReturned, 20)) == 17) {
    nTimesRead++;
    nBytesRead = nRead;
  };
  memcpy(bytesReadString, bytesReturned, 20);

  string cleanBuffer;
  string spacedCleanBuffer;
  int result[4];

  // Process order pair and negative numbers
  for (int x = 0; x < 20; x++) {
    if (isprint(bytesReadString[x])) {
      if (bytesReadString[x] > 32 && bytesReadString[x] < 58) {
        cleanBuffer += bytesReadString[x];
        spacedCleanBuffer += bytesReadString[x];
        if (bytesReadString[x] != '-') {
          spacedCleanBuffer += " ";
        }
      }
    }
  }

  // Retrieve Intergers from order pairs
  if (cleanBuffer.length() != 0 && cleanBuffer.length() != 4) {
    cout << cleanBuffer << endl;
    extractIntegers(spacedCleanBuffer, result);
    if (result[0] >= -2 && result[1] >= -2) {
      xAxisMovement += result[0];
      yAxisMovement += result[1];
    }
  }
  // Process buttons
  else if (cleanBuffer.length() != 0 && cleanBuffer.length() == 4 &&
           strspn(cleanBuffer.c_str(), "01") == cleanBuffer.size()) {
    // cout << cleanBuffer << endl;
    extractIntegerWords(spacedCleanBuffer, result);
    string num = "";
    for (int val : result)
      num += to_string(val);
    // Converts 4-bit number into base 10
    // 0000 => 0, 0001 => 01, 0010 => 10, 0011 => 11.
    binInt = stoi(num);
  }

  bSendSerialMessage = false;
  readTime = ofGetElapsedTimef();

  //================= Graphics ===============================
  previous = current;
  // generate a 3d position
  current.x = xAxisMovement; 
  current.y = yAxisMovement; 
  current.z = 0;  
  current *= 30;  // scale from -1,+1 range to -30,+30

  // add the current position to the pathVertices deque
  pathVertices.push_back(current);
  // if we have too many vertices in the deque, get rid of the oldest ones
  while (pathVertices.size() > 200) {
    pathVertices.pop_front();
  }

  // clear the pathLines ofMesh from any old vertices
  pathLines.clear();
  // add all the vertices from pathVertices
  for (unsigned int i = 0; i < pathVertices.size(); i++) {
    pathLines.addVertex(pathVertices[i]);
  }
}

//--------------------------------------------------------------
void ofApp::draw() {

  ofColor cyan = ofColor::fromHex(0x0000FF);

  if (binInt == 10) {
    cyan = ofColor::fromHex(0x808000);
  } else if (binInt == 01) {
    cyan = ofColor::fromHex(0x00FFFF);
  } else if (binInt == 11) {
    cyan = ofColor::fromHex(0x00FF00);
  }

  ofColor magenta = ofColor::fromHex(0xec008c);
  ofColor yellow = ofColor::fromHex(0xffee00);

  ofBackgroundGradient(magenta * .6, magenta * .4);
  ofNoFill();

  easyCam.begin();
  ofRotateX(15);

  ofSetColor(0);
  ofDrawGrid(500, 10, false, false, true, false);

  // draw the path of the box
  ofSetLineWidth(2);
  ofSetColor(cyan);
  pathLines.draw();

  // draw a line connecting the box to the grid
  ofSetColor(yellow);
  ofDrawLine(current.x, current.y, current.z, current.x, 0, current.z);

  // translate and rotate to the current position and orientation
  ofTranslate(current.x, current.y, current.z);
  if ((current - previous).length() > 0.0) {
    rotateToNormal(current - previous);
  }
  ofSetColor(255);
  ofDrawBox(32);
  ofDrawAxis(32);
  easyCam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
