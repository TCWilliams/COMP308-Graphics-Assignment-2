//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "skeleton.hpp"

using namespace std;
using namespace comp308;

//base radius of bone
float r = 0.015;
//frame counter
int frameCount = 0;

Skeleton::Skeleton(string filename) {
	bone b = bone();
	b.name = "root";
	b.freedom |= dof_rx;
	b.freedom |= dof_ry;
	b.freedom |= dof_rz;
	b.freedom |= dof_root;
	m_bones.push_back(b);
	readASF(filename);
}

// [Assignment2] you may need to revise this function
void Skeleton::renderSkeleton() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	//Create a new GLUquadric object; to allow you to draw cylinders
	GLUquadric *quad = gluNewQuadric();
	if (quad == 0) {
		cerr << "Not enough memory to allocate space to draw" << endl;
		exit(EXIT_FAILURE);
	}
	drawXYZaxis();
	//Actually draw the skeleton
	renderBone(&m_bones[0], quad);

	// Clean up
	gluDeleteQuadric(quad);
	glPopMatrix();
}


//-------------------------------------------------------------
// [Assignment 2] :
// Should render each bone by drawing the axes, and the bone
// then translating to the end of the bone and drawing each
// of it's children. Remember to use the basis rotations
// before drawing the axes (and for Completion, rotation).
// The actual bone is NOT drawn with the basis rotation.
//
// Should not draw the root bone (because it has zero length)
// but should go on to draw it's children
//-------------------------------------------------------------

// draw the XYZ axes
void Skeleton::drawXYZaxis(){
	//X axis
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-1000, 0, 0);
	glVertex3f(1000, 0, 0);
	glEnd();
	
	// Y asix
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, -1000, 0);
	glVertex3f(0, 1000, 0);
	glEnd();

	//Z axis
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, -1000);
	glVertex3f(0, 0, 1000);
	glEnd();
}

void Skeleton::renderBone(bone *b, GLUquadric* q) {
	// YOUR CODE GOES HERE
	if (b == NULL) return;
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	drawAllBones(b, q);
}

void Skeleton::drawAllBones(bone* b, GLUquadric* q){
	if (b == NULL) return;

	glPushMatrix();
	
	drawOneBone(b, q);
	//draw all children
	for (unsigned int i = 0; i < b->children.size(); i++){
		drawAllBones(b->children[i], q);
	}
	glPopMatrix();
}

void Skeleton::drawOneBone(bone* b, GLUquadric* q){

	float angle = 0.0;

	vec3 v = { 0, 0, 1 };
	vec3 origin = { 0,0,0 };
	vec3 LightPos = 

	vec3 w = { b->boneDir.x, b->boneDir.y, b->boneDir.z };
	vec3 normal = { 0, 0, 0 };

	normal = cross(v, w);
	float dotP = dot(v, w);
	float lengthV = length(v);
	float lengthW = length(w);
	angle = acos(dotP / (lengthV*lengthW));
	angle = angle*(180.0 / 3.1416);

	glRotatef(b->basisRot.z, 0, 0, 1);
	glRotatef(b->basisRot.y, 0, 1, 0);
	glRotatef(b->basisRot.x, 1, 0, 0);

	//		cout << "before pose : " << b->rotation << endl;
	
	makePose(b);

	//		cout << "after pose : " << b->rotation.x << endl;
	
	glRotatef(-b->basisRot.x, 1, 0, 0);
	glRotatef(-b->basisRot.y, 0, 1, 0);
	glRotatef(-b->basisRot.z, 0, 0, 1);

	//draw joints and axes

	glPushMatrix();
//	glRotatef(angle, b->basisRot.x, b->basisRot.y, b->basisRot.z);
	glRotatef(b->basisRot.z, 0, 0, 1);
	glRotatef(b->basisRot.y, 0, 1, 0);
	glRotatef(b->basisRot.x, 1, 0, 0);
	drawJoints(b, q);
	glPopMatrix();

	// draw bone b if b is not root (0 length)
	if (b->length > 0){
		glPushMatrix();

		glColor3f(1, 1, 1);
		glRotatef(angle, normal.x, normal.y, normal.z);
		gluCylinder(q, r, r / 3, b->length, 100, 100);

		glPopMatrix();
	}
	glTranslatef(b->length * b->boneDir.x, b->length* b->boneDir.y, b->length*b->boneDir.z);

}
void Skeleton::drawJoints(bone* b, GLUquadric* q){

	//draw joints	
	glPushMatrix();
	glColor3f(0, 1, 1);
	glutSolidSphere(1*r, 100, 100);
	glPopMatrix();
	//draw z axis
	glPushMatrix();

	glColor3f(0, 0, 1);
	gluCylinder(q, 0.3 * r, 0.3*r, 4 * r, 100, 100);
	glTranslatef(0, 0, 4 *r);
	glutSolidCone(0.8*r, 0.8*r, 100, 100);

	glPopMatrix();
	// draw y axis
	glPushMatrix();

	glColor3f(0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(q, 0.3 * r, 0.3*r, 4 * r, 100, 100);
	glTranslatef(0, 0, 4 * r);
	glutSolidCone(0.8 * r, 0.8*r, 100, 100);

	glPopMatrix();
	// draw x axis
	glPushMatrix();

	glColor3f(1, 0, 0);
	glRotatef(90, 0, 1, 0);
	gluCylinder(q, 0.3 * r, 0.3*r, 4 * r, 100, 100);
	glTranslatef(0, 0, 4 * r);
	glutSolidCone(0.8*r, 0.8*r, 100, 100);

	glPopMatrix();
}

void Skeleton::rotateBone(string name, float rotateX, float rotateY, float rotateZ){
	int boneIndex = findBone(name);
	if (boneIndex > -1){
		bone b = m_bones[boneIndex];

		b.rotation.x = rotateX;
		b.rotation.y = rotateY;
		b.rotation.y = rotateZ;

		m_bones[boneIndex] = b;  // update bone in list

		cout << "new rotation : " << m_bones[boneIndex].rotation.x << endl;
	}
}

void Skeleton::makePose(bone* b){
//	glTranslatef(b->translation.x, b->translation.y, b->translation.z);
	if (b->name == "root"){
		glTranslatef(0.0458888 * b->translation.x,0, 0.0458888 * b->translation.z);
	}
	glRotatef(b->rotation.z, 0, 0, 1);
	glRotatef(b->rotation.y, 0, 1, 0);
	glRotatef(b->rotation.x, 1, 0, 0);
}

void Skeleton::resetPose() {
	for (unsigned int i = 0; i < m_bones.size(); i++) {
		m_bones[i].rotation.x = 0;
		m_bones[i].rotation.y = 0;
		m_bones[i].rotation.z = 0;
		m_bones[i].translation.x = 0;
		m_bones[i].translation.y = 0;
		m_bones[i].translation.z = 0;
	}
}


// Helper method for retreiving and trimming the next line in a file.
// You should not need to modify this method.
namespace {
	string nextLineTrimmed(istream &file) {
		// Pull out line from file
		string line;
		getline(file, line);
		// Remove leading and trailing whitespace and comments
		size_t i = line.find_first_not_of(" \t\r\n");
		if (i != string::npos) {
			if (line[i] != '#') {
				return line.substr(i, line.find_last_not_of(" \t\r\n") - i + 1);
			}
		}
		return "";
	}
}

int Skeleton::getNumFrames(){
	return frameCount;
}

int Skeleton::findBone(string name) {
	for (size_t i = 0; i < m_bones.size(); i++)
		if (m_bones[i].name == name)
			return i;
	return -1;
}


void Skeleton::readASF(string filename) {

	ifstream file(filename);

	if (!file.is_open()) {
		cerr << "Failed to open file " <<  filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file" << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {

		// Pull out line from file
		string line = nextLineTrimmed(file);

		// Check if it is a comment or just empty
		if (line.empty() || line[0] == '#')
			continue;
		else if (line[0] == ':') {
			// Line starts with a ':' character so it must be a header
			readHeading(line, file);
		} else {
			// Would normally error here, but becuase we don't parse
			// every header entirely we will leave this blank.
		}
	}

	cout << "Completed reading skeleton file" << endl;
}


void Skeleton::readHeading(string headerline, ifstream &file) {

	string head;
	istringstream lineStream(headerline);
	lineStream >> head; // get the first token from the stream

	// remove the ':' from the header name
	if (head[0] == ':')
		head = head.substr(1);

	if (lineStream.fail() || head.empty()) {
		cerr << "Could not get heading name from\"" << headerline << "\", all is lost" << endl;
		throw runtime_error("Error :: could not parse .asf file.");
	}

	if (head == "version") {
		//version string - must be 1.10
		string version;
		lineStream >> version;
		if (lineStream.fail() || version != "1.10") {
			cerr << "Invalid version: \"" << version << "\" must be 1.10" << endl;
			throw runtime_error("Error :: invalid .asf version.");
		}
	}
	else if (head == "name") {
		// This allows the skeleton to be called something
		// other than the file name. We don't actually care
		// what the name is, so can ignore this whole section
	}
	else if (head == "documentation") {
		// Documentation section has no meaningful information
		// only of use if you want to copy the file. So we skip it
	}
	else if (head == "units") {
		// Has factors for the units to be able to model the
		// real person, these must be parsed correctly. Only
		// really need to check if deg or rad, but that is 
		// not needed for this assignment.

		// We are going to assume that the units:length feild
		// is 0.45, and that the angles are in degrees
	}
	else if (head == "root") {
		// Read in information about root. Let's just assume
		// it'll be at the origin for this assignment.
	}
	else if (head == "bonedata") {
		// Read in each bone until we get to the
		// end of the file or a new header
		string line = nextLineTrimmed(file);
		while (file.good() && !line.empty()) {
			if (line[0] == ':') {
				// finished our reading of bones
				// read next header and return
				return readHeading(line, file);
			}
			else if (line == "begin") {
				// Read the bone data
				readBone(file);
			}
			else {
				cerr << "Expected 'begin' in bone data, found \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
			line = nextLineTrimmed(file);
		}
	}
	else if (head == "hierarchy") {
		// Description of how the bones fit together
		// Read in each line until we get to the
		// end of the file or a new header
		string line = nextLineTrimmed(file);
		while (file.good() && !line.empty()) {
			if (line[0] == ':') {
				// finished our reading of bones
				// read next header and return
				return readHeading(line, file);
			}
			else if (line == "begin") {
				// Read the entire hierarchy
				readHierarchy(file);
			}
			else {
				cerr << "Expected 'begin' in hierarchy, found \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
			line = nextLineTrimmed(file);
		}
	}
	else {
		// Would normally error here, but becuase we don't parse
		// every header entirely we will leave this blank.
	}
}


void Skeleton::readBone(ifstream &file) {
	// Create the bone to add the data to
	bone b;

	string line = nextLineTrimmed(file);
	while (file.good()) {
		if (line == "end") {
			// End of the data for this bone
			// Push the bone into the vector
			m_bones.push_back(b);
			return;
		}
		else {
			
			string head;
			istringstream lineStream(line);
			lineStream >> head; // Get the first token

			if (head == "name") {
				// Name of the bone
				lineStream >> b.name;
			}
			else if (head == "direction") {
				// Direction of the bone
				lineStream >> b.boneDir.x >> b.boneDir.y >> b.boneDir.z;
				b.boneDir = normalize(b.boneDir); // Normalize here for consistency
			}
			else if (head == "length") {
				// Length of the bone
				float length;
				lineStream >> length;
				length *= (1.0/0.45);  // scale by 1/0.45 to get actual measurements
				length *= 0.0254;      // convert from inches to meters
				b.length = length;
			}
			else if (head == "dof") {
				// Degrees of Freedom of the joint (rotation)
				while (lineStream.good()) {
					string dofString;
					lineStream >> dofString;
					if (!dofString.empty()) {
						// Parse each dof string
						if      (dofString == "rx") b.freedom |= dof_rx;
						else if (dofString == "ry") b.freedom |= dof_ry;
						else if (dofString == "rz") b.freedom |= dof_rz;
						else throw runtime_error("Error :: could not parse .asf file.");
					}
				}
			}
			else if (head == "axis") {
				// Basis rotations 
				lineStream >> b.basisRot.x >> b.basisRot.y >> b.basisRot.z;
			}
			else if (head == "limits") {
				// Limits for each of the DOF
				// Assumes dof has been read first
				// You can optionally fill this method out
			}

			// Because we've tried to parse numerical values
			// check if we've failed at any point
			if (lineStream.fail()) {
				cerr << "Unable to parse \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
		}

		// Get the next line
		line = nextLineTrimmed(file);
	}

	cerr << "Expected end in bonedata, found \"" << line << "\"";
	throw runtime_error("Error :: could not parse .asf file.");
}


void Skeleton::readHierarchy(ifstream &file) {
	string line = nextLineTrimmed(file);
	while (file.good()) {
		if (line == "end") {
			// End of hierarchy
			return;
		}
		else if (!line.empty()) {
			// Read the parent node
			string parentName;
			istringstream lineStream(line);
			lineStream >> parentName;

			// Find the parent bone and have a pointer to it
			int parentIndex = findBone(parentName);

			if (parentIndex < 0) {
				cerr << "Expected a valid parent bone name, found \"" << parentName << "\"" << endl;
				throw runtime_error("Error :: could not parse .asf file.");
			}

			//Read the connections
			string childName;
			lineStream >> childName;
			while (!lineStream.fail() && !childName.empty()) {

				int childIndex = findBone(childName);

				if (childIndex < 0) {
					cerr << "Expected a valid child bone name, found \"" << childName << "\"" << endl;
					throw runtime_error("Error :: could not parse .asf file.");
				}

				// Set a POINTER to the child to be recorded in the parents
				m_bones[parentIndex].children.push_back(&m_bones[childIndex]);
				
				// Get the next child
				lineStream >> childName;
			}
		}
		line = nextLineTrimmed(file);
	}
	cerr << "Expected end in bonedata, found \"" << line << "\"";
	throw runtime_error("Error :: could not parse .asf file.");
}



//-------------------------------------------------------------
// [Assignment 2] :
// Complete the following method to load data from an *.amc file
//-------------------------------------------------------------

// read asf pose file
void Skeleton::readPose(string filename){
	
	ifstream file(filename);

	if (!file.is_open()) {
		cerr << "Failed to open file " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file" << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {

		string line = nextLineTrimmed(file);

		string name;
		istringstream linestream(line);
		linestream >> name;

		bone* bone = &m_bones[0]; // initialise bone pointer

		int boneIndex = 0;
		// Check if it is a comment or just empty
		if (line.empty() || line[0] == '#' || line[0] == ':')
			continue;
		/*	if (name == "lhipjoint" || name == "rhipjoint") // skip these bones as they are not in amc
		continue;
		*/
		boneIndex = findBone(name);
		if (boneIndex > -1)
			bone = &m_bones[boneIndex];
		//check DOF
		if (bone->freedom & dof_rx) linestream >> bone->rotation.x;
		if (bone->freedom & dof_ry) linestream >> bone->rotation.y;
		if (bone->freedom & dof_rz) linestream >> bone->rotation.z;

	/*	if (bone->name == "lhumerus" || bone->name == "rhumerus"){
			bone->rotation.z = -bone->rotation.z;
		}
*/
		rotateBone(bone->name, bone->rotation.x, bone->rotation.y, bone->rotation.z);

		cout << "Completed reading skeleton file" << endl;
	}
}
int Skeleton::readAMC(string filename) {

	ifstream file(filename);
	
	if (!file.is_open()) {
		cerr << "Failed to open file " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file" << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {
				
		string line = nextLineTrimmed(file);
		
		if (isdigit(line[0])){
			frameCount++; // to next frame
			continue;
		}
		string name;
		istringstream linestream(line);
		linestream >> name;
				
		bone* bone = &m_bones[0]; // initialise bone pointer
				
		float rotX = 0;
		float rotY = 0;
		float rotZ = 0;
		float tranX = 0;
		float tranY = 0;
		float tranZ = 0;
		
		int boneIndex = 0;
		// Check if it is a comment or just empty
		if (line.empty() || line[0] == '#' || line[0] == ':')
			continue;
		if (name == "lhipjoint" || name == "rhipjoint") // skip these bones as they are not in amc
			continue;

		if (name == "root"){
			boneIndex = findBone(name);
			if (boneIndex > -1)
				bone = &m_bones[boneIndex];
			// apply next values to trans and rot variables 
			linestream >> tranX >> tranY >> tranZ >> rotX >> rotY >> rotZ;
		}
		else { // not root, 
			boneIndex = findBone(name);
			if (boneIndex > -1)
				bone = &m_bones[boneIndex];
			//check DOF
			if (bone->freedom & dof_rx) linestream >> rotX;
			if (bone->freedom & dof_ry) linestream >> rotY;
			if (bone->freedom & dof_rz) linestream >> rotZ;
		}
	
		//add frame to b frames list
		bone->frames[frameCount - 1].rotX = rotX;
		bone->frames[frameCount - 1].rotY = rotY;
		bone->frames[frameCount - 1].rotZ = rotZ;
		bone->frames[frameCount - 1].tranX = tranX;
		bone->frames[frameCount - 1].tranY = tranY;
		bone->frames[frameCount - 1].tranZ = tranZ;
		
	//	cout << "bone name  :" << bone->name << "frame count " << frameCount << "   rotX  :" << bone->frames[frameCount-1].rotX << "  rotY  :" << bone->frames[frameCount-1].rotY << " << rotZ << " << bone->frames[frameCount-1].rotZ << endl;
	}
	return frameCount;
	cout << "Completed reading skeleton file" << endl;
}
void Skeleton::loadFrame(int frameNum){
	int frame = frameNum - 1;

	for (unsigned int i = 0; i < m_bones.size(); i++){
		amcFrame frm = m_bones[i].frames[frame];

		// apply 0's to hipjoints
		if (m_bones[i].name == "lhipjoint" || m_bones[i].name == "rhipjoint"){
			m_bones[i].rotation.x = 0;
			m_bones[i].rotation.y = 0;
			m_bones[i].rotation.z = 0;
		}		
		// apply frame rot & trans to root
		else if (m_bones[i].name == "root"){
			m_bones[i].translation.x = frm.tranX;
			m_bones[i].translation.y = frm.tranY;
			m_bones[i].translation.z = frm.tranZ;

			m_bones[i].rotation.x = frm.rotX;
			m_bones[i].rotation.y = frm.rotY;
			m_bones[i].rotation.z = frm.rotZ;
			
			
			cout << "transX :" << m_bones[i].translation.x << endl;
			cout << "transY :" << m_bones[i].translation.y << endl;
			cout << "transZ :" << m_bones[i].translation.z << endl;
			
		}
		//apply frame rot to other bones
		else{			
			m_bones[i].rotation.x = frm.rotX;
			m_bones[i].rotation.y = frm.rotY;
			m_bones[i].rotation.z = frm.rotZ;
		}
		
	}
}