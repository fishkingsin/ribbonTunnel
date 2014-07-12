#include "ofApp.h"
bool drawMode = false;
//Universal function which sets normals for the triangle mesh
void setNormals( ofMesh &mesh ){
    //The number of the vertices
    int nV = mesh.getNumVertices();
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    vector<ofPoint> norm( nV ); //Array for the normals
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        //Compute the triangle's normal
        //normal to outside
//        ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        
        //normatl to inside
        ofPoint dir = ( ( v3 - v1 ).crossed(v2 - v1 ) ).normalized();
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        norm[i].normalize();
    }
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals( norm );
}
float Rad = 1000; //Radius of circle
float circleStep = 3; //Step size for circle motion
int circleN = 20; //Number of points on the circle

ofPoint axeX, axyY, axyZ; //Circle's coordinate system
//--------------------------------------------------------------
void ofApp::setup(){
	//just set up the openFrameworks stuff
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(255);
    
	//initialize the variable so it's off at the beginning
    usecamera = false;
    light.enable(); //Enable lighting

}

//--------------------------------------------------------------
void ofApp::update(){
	//don't move the points if we are using the camera
    if(!usecamera){
        ofVec3f sumOfAllPoints(0,0,0);
        for(unsigned int i = 0; i < points.size(); i++){
            points[i].z -= 50;
            sumOfAllPoints += points[i];
        }
        while (points.size()>100) {
            points.erase(points.begin());
        }
        center = sumOfAllPoints / points.size();
        camera.setPosition(mouseX,mouseY,0);
        camera.lookAt(ofVec3f(mouseX,mouseY,0));

        
        light.setPosition(ofVec3f(mouseX,mouseY,0));
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofEnableDepthTest(); //Enable z-buffering
    //Set a gradient background from white to gray
    ofBackgroundGradient( ofColor( 0 ), ofColor( 128 ) );
	//if we're using the camera, start it.
	//everything that you draw between begin()/end() shows up from the view of the camera
    camera.begin();
    ofMesh mesh;
    for(unsigned int i = 1; i < points.size(); i++){
		ofVec3f nextPoint = points[i];
        addCircle(nextPoint, mesh);
    }
    if(drawMode)
    {
        mesh.draw();
    }
    else
    {
        mesh.drawWireframe();
    }
    camera.end();
    ofSetColor(255);
    ofDrawBitmapString("'c' toggle camera \n 'm' toggle wireframe", ofPoint(20,20));
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //hitting any key swaps the camera view
    if(key == 'c')
    {
        
        usecamera = !usecamera;
    }
    if(key == 'm')
    {
        drawMode = !drawMode;
    }
    if(key == ' ')
    {
        ofSaveFrame();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	//if we are using the camera, the mouse moving should rotate it around the whole sculpture
    if(usecamera){
        float rotateAmount = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 360);
        ofVec3f furthestPoint;
        if (points.size() > 0) {
            furthestPoint = points[0];
        }
        else
        {
            furthestPoint = ofVec3f(x, y, 0);
        }
        
        ofVec3f directionToFurthestPoint = (furthestPoint - center);
        ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.rotated(rotateAmount, ofVec3f(0,1,0));
        camera.setPosition(center + directionToFurthestPointRotated);
        camera.lookAt(center);
    }
	//otherwise add points like before
    else{
        ofVec3f mousePoint(x,y,0);
        points.push_back(mousePoint);
    }
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//http://www.packtpub.com/sites/default/files/9781849518048_Chapter_07.pdf
//knotExample
void ofApp::addCircle( ofVec3f nextPoint, ofMesh &mesh ){
    float time = ofGetElapsedTimef();    //Time
    //Parameters – twisting and rotating angles and color

    ofFloatColor color( ofNoise( time * 0.05 ),
                       ofNoise( time * 0.1 ),
                       ofNoise( time * 0.15 ));
    color.setSaturation( 1.0 );  //Make the color maximally
    
    //Add vertices
    for (int i=0; i<circleN; i++) {
        float angle = float(i) / circleN * TWO_PI+(PI*0.25);
        float x = Rad * cos( angle );
        float y = Rad * sin( angle );
        ofPoint p =    nextPoint+ofVec3f(x ,y , 0);
        mesh.addVertex( p );
        mesh.addColor( color );
    }
    //Add the triangles
    int base = mesh.getNumVertices() - 2 * circleN;
    if ( base >= 0 ) {  //Check if it is not the first step
        //and we really need to add the triangles
        for (int i=0; i<circleN; i++) {
            int a = base + i;
            int b = base + (i + 1) % circleN;
            int c = circleN  + a;
            int d = circleN  + b;
            mesh.addTriangle(a,b,d);
            mesh.addTriangle(a, d, c);
        }
        //Update the normals
        setNormals( mesh );
    }
}