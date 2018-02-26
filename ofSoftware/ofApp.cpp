#include "ofApp.h"
#include <vector>

int xx = 0;
int yx = 0;

//---------------------------------------------------
void extractIntegerWords(string str, int *output){
	stringstream ss;
	ss << str;

	string temp;
	int found;
	vector<int> result; 


	while (!ss.eof()) {
		ss >> temp;
		if (stringstream(temp) >> found) {
			result.push_back(found); 
		}
		temp = "";
	}
	
	if (result.size() == 2) {
		output[0] = result.at(0);
		output[1] = result.at(1);
	}
	else if (result.size() == 4) {
		for (int x = 0; x < 4; x++) {
			output[x] = result.at(x);
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
void ofApp::setup(){
	ofSetVerticalSync(true);
	// draw the vertices in pathLines as a line strip
	pathLines.setMode(OF_PRIMITIVE_LINE_STRIP);


	bSendSerialMessage = false;
	ofBackground(255);
	ofSetLogLevel(OF_LOG_VERBOSE);

	font.load("DIN.otf", 64);

	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

	int baud = 56000;
	serial.setup(0, baud); //open the first device
	serial.setup("COM17", baud); // windows example

	nTimesRead = 0;
	nBytesRead = 0;
	readTime = 0;
	memset(bytesReadString, 0, 16);


}

//--------------------------------------------------------------
void ofApp::update(){

	// ============== Communication ============================
	nTimesRead = 0;
	nBytesRead = 0;
	int nRead = 0;  // a temp variable to keep count per read

	unsigned char bytesReturned[20];
	memset(bytesReadString, 0, 16);
	memset(bytesReturned, 0, 20);
	while ((nRead = serial.readBytes(bytesReturned, 20)) == 17) {
		nTimesRead++;
		nBytesRead = nRead;
	};
	memcpy(bytesReadString, bytesReturned, 20);

	
	// Some "repetive" elements might appear as heere the broken input will be fixed 
	string cleanBuffer;
	string spacedCleanBuffer; 
	int result[4]; 

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

	// Buttons not implemented yet
	if (cleanBuffer.length() != 0 && cleanBuffer.length() != 4) {
		cout << cleanBuffer << endl; 
		extractIntegerWords(spacedCleanBuffer, result);
		if (result[0] >= -2 && result[1] >= -2) {
			xx += result[0];
			yx += result[1];
		}
	}
	else if (cleanBuffer.length() != 0 && cleanBuffer.length() == 4 && strspn(cleanBuffer.c_str(), "01") == cleanBuffer.size() ) {
		//cout << cleanBuffer << endl; 
		extractIntegerWords(spacedCleanBuffer, result);
		string num = "";
		//cout << "HERE" << endl; 
		for (auto val : result)
			num += to_string(val);

		binInt = stoi(num);
	}

	if (binInt != 0) {
	//	cout << binInt << endl; 
	}
	
	bSendSerialMessage = false;
	readTime = ofGetElapsedTimef();

	//================= Graphics ===============================
	previous = current;

	// generate a noisy 3d position over time 
	float t = (2 + ofGetElapsedTimef()) * .1;
	//cout << t << endl;
	current.x = xx; //(xx, 0, 0);//ofSignedNoise(xx, 0, 0);
	current.y = yx; //(0, yx, 0);//ofSignedNoise(0, 0, 0);
	current.z = 0; //(0, 0, 0);//ofSignedNoise(0, 0, yx);
	current *= 30; // scale from -1,+1 range to -400,+400

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

	// ============================================================

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofColor cyan = ofColor::fromHex(0x0000FF);

	if (binInt == 10) {
		 cyan = ofColor::fromHex(0x808000);
	}
	else if (binInt == 01) {
		 cyan = ofColor::fromHex(0x00FFFF);
	}
	else if (binInt == 11) {
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
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
