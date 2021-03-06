#include "RoadNet.h"


RoadNetwork::RoadNetwork(int randNr, Vector2 max, Vector2 min, float rotation)
{
	this->roadNetworkMax = max;
	this->roadNetworkMin = min;
	setAngle(rotation);
	this->forwardSymbol = 'F';
	this->turnLeftSymbol = '-';
	this->turnRightSymbol = '+';
	this->halfSymbol = 'H';
	this->forward = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	//Calculate startpoint
	Vector2 fromMinToMaxDir = (roadNetworkMax - roadNetworkMin);
	Vector2 partway = fromMinToMaxDir / 1024.0f;
	
	int rand = randNr % 1024 + 1;
	int xOrZ = rand % 2;
	bool toMin;
	if (rand % 10 > 5) {
		toMin = false;
	}
	else {
		toMin = true;
	}
	Vector2 possibleStart = roadNetworkMin + (partway * float(rand));
	if (xOrZ == 0) { //move start to x value
		if (toMin) {
			possibleStart.x = this->roadNetworkMin.x;
			this->forwardDistance = fromMinToMaxDir.y / 32.0f;
			this->forward.x = forwardDistance;
		}
		else {
			possibleStart.x = this->roadNetworkMax.x;
			this->forwardDistance = fromMinToMaxDir.y / 32.0f;
			this->forward.x = -1 * forwardDistance;
		}
	}
	else if (xOrZ == 1) { //move start to z value
		if (toMin) {
			possibleStart.y = this->roadNetworkMin.y;
			this->forwardDistance = fromMinToMaxDir.y / 32.0f;
			this->forward.z = forwardDistance;
		}
		else {
			possibleStart.y = this->roadNetworkMax.y;
			this->forwardDistance = fromMinToMaxDir.y / 32.0f;
			this->forward.z = -1 * forwardDistance;
		}
	}
	this->startPoint = Vector4(0.0f, 0.0f,0.0f, 0.0f);
	Segment startSegment;
	startSegment.firstPoint = this->startPoint - forward;
	startSegment.secondPoint = this->startPoint;
	this->roadNetwork.push_back(startSegment);
}

RoadNetwork::~RoadNetwork()
{
	clearSegments();
	for (int i = 0; i < this->roadFiles.size(); i++) {
		remove(roadFiles[i].c_str());
	}
}

void RoadNetwork::setLetters(char forward, char left, char right, char half)
{
	this->forwardSymbol = forward;
	this->turnLeftSymbol = left;
	this->turnRightSymbol = right;
	this->halfSymbol = half;
}

void RoadNetwork::setAngle(float angle)
{
	this->rotationAngle = (angle) * ((PI)/180.0L);
}

float RoadNetwork::getAngle() const
{
	return float(rotationAngle) / 180.0f / float(PI);
}

void RoadNetwork::clearSegments()
{
	Segment first = this->roadNetwork.front();
	this->roadNetwork.clear();
	this->roadNetwork.push_back(first);
}

bool RoadNetwork::generateInitialSegments(const char* seed)
{ //'F' to move forward a segment, '+' or '-' to turn but doesn't move 'H' to move half a length for a segment
	bool success = false;
	int counter = 0;
	Vector4 currentForward = this->forward*0.06f;
	float nextX, nextZ;
	while (seed[counter]) {

		if (seed[counter] == forwardSymbol) {
			Segment temp;
			temp.firstPoint = this->roadNetwork.at(this->roadNetwork.size() - 1).secondPoint;
			temp.secondPoint = temp.firstPoint + currentForward;
			if (temp.secondPoint.x > this->roadNetworkMax.x) {
				temp.secondPoint.x = this->roadNetworkMax.x;
			}
			else if (temp.secondPoint.x < this->roadNetworkMin.x) {
				temp.secondPoint.x = this->roadNetworkMin.x;
			}
			if (temp.secondPoint.z > this->roadNetworkMax.y) {
				temp.secondPoint.z = this->roadNetworkMax.y;
			}
			else if (temp.secondPoint.z < this->roadNetworkMin.y) {
				temp.secondPoint.z = this->roadNetworkMin.y;
			}
			this->roadNetwork.push_back(temp);

			success = true;
		}

		else if (seed[counter] == turnLeftSymbol) { //Mainline turtle walk
			nextX = (currentForward.x * float(cos(rotationAngle))) - (currentForward.z * float(sin(rotationAngle)));
			nextZ = (currentForward.x * float(sin(rotationAngle))) + (currentForward.z * float(cos(rotationAngle)));
			currentForward.x = nextX;
			currentForward.z = nextZ;
			success = true;
		}

		else if (seed[counter] == turnRightSymbol) {
			nextX = (currentForward.x * float((cos(rotationAngle)))) + (currentForward.z * float(sin(rotationAngle)));
			nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
			currentForward.x = nextX;
			currentForward.z = nextZ;
			success = true;
		}

		else if (seed[counter] == halfSymbol) {
			Segment temp;
			temp.firstPoint = this->roadNetwork.at(this->roadNetwork.size() - 1).secondPoint;
			temp.secondPoint = temp.firstPoint + (currentForward * 0.5f);
			if (temp.secondPoint.x > this->roadNetworkMax.x) {
				temp.secondPoint.x = this->roadNetworkMax.x;
			}
			else if (temp.secondPoint.x < this->roadNetworkMin.x) {
				temp.secondPoint.x = this->roadNetworkMin.x;
			}
			else if (temp.secondPoint.z > this->roadNetworkMax.y) {
				temp.secondPoint.z = this->roadNetworkMax.y;
			}
			else if (temp.secondPoint.z < this->roadNetworkMin.y) {
				temp.secondPoint.z = this->roadNetworkMin.y;
			}
			this->roadNetwork.push_back(temp);

			success = true;
		}
		counter++;
	}
	return success;
}

bool RoadNetwork::generateAdditionalSegments(const char* seed, int segment, bool right) //Turtle walk from existing segments
{ //'F' to move forward a segment, '+' or '-' to turn but doesn't move 'H' to move half a length for a segment
	bool success = false;
	int counter = 0;
	Vector4 currentForward = this->forward*0.06f;
	float nextX, nextZ;
	auto  check = roadNetwork.size();

	if (segment > 0 && segment < roadNetwork.size()) {
		while (seed[counter]) {

			if (seed[counter] == forwardSymbol) {
				Segment temp;
				if (check == roadNetwork.size()) {
					temp.firstPoint = this->roadNetwork.at(segment).firstPoint;
					currentForward = this->roadNetwork.at(segment).secondPoint - temp.firstPoint;
					if (!right) { // turn left
						nextX = (currentForward.x * float(cos(rotationAngle))) - (currentForward.z * float(sin(rotationAngle)));
						nextZ = (currentForward.x * float(sin(rotationAngle))) + (currentForward.z * float(cos(rotationAngle)));
						currentForward.x = nextX;
						currentForward.z = nextZ;
					}
					else { // turn right
						nextX = (currentForward.x * float(cos(rotationAngle))) + (currentForward.z * float(sin(rotationAngle)));
						nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
						currentForward.x = nextX;
						currentForward.z = nextZ;
					}
					temp.secondPoint = temp.firstPoint + currentForward;
				}
				else if (check < roadNetwork.size()) {
					temp.firstPoint = this->roadNetwork.at(this->roadNetwork.size() - 1).secondPoint;
					temp.secondPoint = temp.firstPoint + currentForward;
				}
				if (temp.secondPoint.x > this->roadNetworkMax.x) {
					temp.secondPoint.x = this->roadNetworkMax.x;
				}
				else if (temp.secondPoint.x < this->roadNetworkMin.x) {
					temp.secondPoint.x = this->roadNetworkMin.x;
				}
				if (temp.secondPoint.z > this->roadNetworkMax.y) {
					temp.secondPoint.z = this->roadNetworkMax.y;
				}
				else if (temp.secondPoint.z < this->roadNetworkMin.y) {
					temp.secondPoint.z = this->roadNetworkMin.y;
				}
				this->roadNetwork.push_back(temp);

				success = true;
			}

			else if (seed[counter] == turnLeftSymbol) {
				nextX = (currentForward.x * float(cos(rotationAngle))) - (currentForward.z * float(sin(rotationAngle))) ;
				nextZ = (currentForward.x * float(sin(rotationAngle))) + (currentForward.z * float(cos(rotationAngle))) ;
				currentForward.x = nextX;
				currentForward.z = nextZ;
				success = true;
			}

			else if (seed[counter] == turnRightSymbol) {
				nextX = (currentForward.x * float(cos(rotationAngle))) + (currentForward.z * float(sin(rotationAngle)));
				nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
				currentForward.x = nextX;
				currentForward.z = nextZ;
				success = true;
			}

			else if (seed[counter] == halfSymbol) {
				Segment temp;
				if (check == roadNetwork.size()) {
					temp.firstPoint = this->roadNetwork.at(segment).firstPoint;
					currentForward = this->roadNetwork.at(segment).secondPoint - temp.firstPoint;
					if (!right) { // turn left
						nextX = (currentForward.x * float(cos(rotationAngle))) - (currentForward.z * float(sin(rotationAngle)));
						nextZ = (currentForward.x * float(sin(rotationAngle))) + (currentForward.z * float(cos(rotationAngle)));
						currentForward.x = nextX;
						currentForward.z = nextZ;
					}
					else { // turn right
						nextX = (currentForward.x * float(cos(rotationAngle))) + (currentForward.z * float(sin(rotationAngle)));
						nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
						currentForward.x = nextX;
						currentForward.z = nextZ;
					temp.secondPoint = temp.firstPoint + (currentForward * 0.5f);
					}
				}
				else if (check < roadNetwork.size()) {
					temp.firstPoint = this->roadNetwork.at(this->roadNetwork.size() - 1).secondPoint;
					temp.secondPoint = temp.firstPoint + (currentForward * 0.5);
				}
				if (temp.secondPoint.x > this->roadNetworkMax.x) {
					temp.secondPoint.x = this->roadNetworkMax.x;
				}
				else if (temp.secondPoint.x < this->roadNetworkMin.x) {
					temp.secondPoint.x = this->roadNetworkMin.x;
				}
				if (temp.secondPoint.z > this->roadNetworkMax.y) {
					temp.secondPoint.z = this->roadNetworkMax.y;
				}
				else if (temp.secondPoint.z < this->roadNetworkMin.y) {
					temp.secondPoint.z = this->roadNetworkMin.y;
				}
				this->roadNetwork.push_back(temp);

				success = true;
			}

		counter++;
		}
	}
	return success;
}

void RoadNetwork::cleanRoadNetwork()
{
	for (int i = 0; i < this->roadNetwork.size(); i++) {
		for (int j = i + 1; j < this->roadNetwork.size(); j++) {
			if ((roadNetwork[i].firstPoint == roadNetwork[j].firstPoint &&
				roadNetwork[i].secondPoint == roadNetwork[j].secondPoint) ||
				(roadNetwork[i].firstPoint == roadNetwork[j].secondPoint &&
				roadNetwork[i].secondPoint == roadNetwork[j].firstPoint)) {
				roadNetwork.erase(roadNetwork.begin()+j);
				j--;
			}
			else {
				if (segmentIntersection(this->roadNetwork[i], this->roadNetwork[j])) { //Crossing
					this->roadNetwork[i].crossing = &this->roadNetwork[j];
					this->roadNetwork[j].crossing = &this->roadNetwork[i];
				}
			}
		}
	}
}

void RoadNetwork::drawRoadNetwork(Graphics* graph)
{
	graph->getdebugger()->DrawLine(XMFLOAT3(this->roadNetworkMin.x, 2.0f, this->roadNetworkMin.y), XMFLOAT3(this->roadNetworkMin.x, 2.0f, this->roadNetworkMax.y), XMFLOAT3(0.0f, 1.0f, 0.0f));
	graph->getdebugger()->DrawLine(XMFLOAT3(this->roadNetworkMin.x, 2.0f, this->roadNetworkMin.y), XMFLOAT3(this->roadNetworkMax.x, 2.0f, this->roadNetworkMin.y), XMFLOAT3(0.0f, 1.0f, 0.0f));
	graph->getdebugger()->DrawLine(XMFLOAT3(this->roadNetworkMin.x, 2.0f, this->roadNetworkMax.y), XMFLOAT3(this->roadNetworkMax.x, 2.0f, this->roadNetworkMax.y), XMFLOAT3(0.0f, 1.0f, 0.0f));
	graph->getdebugger()->DrawLine(XMFLOAT3(this->roadNetworkMax.x, 2.0f, this->roadNetworkMin.y), XMFLOAT3(this->roadNetworkMax.x, 2.0f, this->roadNetworkMax.y), XMFLOAT3(0.0f, 1.0f, 0.0f));
	
	for (int i = 0; i < this->roadNetwork.size(); i++) {
		graph->getdebugger()->DrawLine(XMFLOAT3(this->roadNetwork[i].firstPoint.x, 2.0f, this->roadNetwork[i].firstPoint.z),
			XMFLOAT3(this->roadNetwork[i].secondPoint.x, 2.0f, this->roadNetwork[i].secondPoint.z),
			XMFLOAT3(1.0f, 0.0f, 1.0f));
	}
}

std::vector<Segment>& RoadNetwork::getSegments()
{
	return roadNetwork;
}

bool RoadNetwork::saveTestNetwork(std::string filename)
{
	std::ofstream testSave;
	std::string toSave;
	std::string filePath = "Road Network\\Generated Road Networks\\";
	std::string fileEnd = ".RoadNet";
	std::string file = filePath + filename + fileEnd;
	testSave.open(file);

	if (!testSave.is_open()) {
		return false;
	}
	toSave = std::to_string(this->roadNetwork.size()) + "\n";
	testSave << toSave;
	for (int i = 0; i < this->roadNetwork.size(); i++) {
		toSave = "(" + std::to_string(this->roadNetwork[i].firstPoint.x) +
			", " + std::to_string(this->roadNetwork[i].firstPoint.z) + ") to (" +
			std::to_string(this->roadNetwork[i].secondPoint.x) +
			", " + std::to_string(this->roadNetwork[i].secondPoint.z) + ")\n";
		testSave << toSave;
	}
	this->roadFiles.push_back(file);

	testSave.close();

	return true;
}

bool RoadNetwork::loadTestNetwork(std::string filename)
{
	std::ifstream testLoad;
	std::string loaded;
	int nrOfSegments;
	std::string filePath = "Road Network\\Generated Road Networks\\";
	std::string fileEnd = ".RoadNet";
	std::string file = filePath + filename + fileEnd;
	testLoad.open(file);
	if (!testLoad.is_open()) {
		return false;
	}
	clearSegments();
	this->roadNetwork.erase(this->roadNetwork.begin());

	if (testLoad >> loaded) {
		nrOfSegments = std::stoi(loaded);

		while (nrOfSegments != this->roadNetwork.size()) {
			Segment temp;
			if (testLoad >> loaded) {
				temp.firstPoint.x = std::stof(loaded.substr(1, loaded.find_first_of(',') - 1));

				if (testLoad >> loaded) {
					temp.firstPoint.z = std::stof(loaded.substr(0, loaded.find_first_of(')') - 1));
				}
				testLoad >> loaded;
			}
			if (testLoad >> loaded) {
				temp.secondPoint.x = std::stof(loaded.substr(1, loaded.find_first_of(',') - 1));
				if (testLoad >> loaded) {
					temp.secondPoint.z = std::stof(loaded.substr(0, loaded.find_first_of(')') - 1));
					this->roadNetwork.push_back(temp);
				}
			}
		}
	}

	testLoad.close();
	return true;
}

bool RoadNetwork::segmentIntersection(Segment& first, Segment& second)
{
	bool intersect = false;

	Vector2 point1, point2, point3, point4;
	point1 = Vector2(first.firstPoint.x, first.firstPoint.z);
	point2 = Vector2(first.secondPoint.x, first.secondPoint.z);
	point3 = Vector2(second.firstPoint.x, second.firstPoint.z);
	point4 = Vector2(second.secondPoint.x, second.secondPoint.z);
	float t1, t2, divider;
	divider = (((point4.x - point3.x) * (point1.y - point2.y))
			- ((point1.x - point2.x) * (point4.y - point3.y)));
	if (divider != 0) {
		t1 = (((point3.y - point4.y) * (point1.x - point3.x)) +
			((point4.x - point3.x) * (point1.y - point3.y)))
			/ divider;

		t2 = (((point1.y - point2.y) * (point1.x - point3.x)) +
			((point2.x - point1.x) * (point1.y - point3.y)))
			/ divider;

		if (t1 > 0 && t2 > 0 && t1 < 1 && t2 < 1) {
			intersect = true;
			first.crossingPosition = new Vector3((point1.x + (point2.x - point1.x) * t1), 0.0f, (point1.y + (point2.y - point1.y) * t1));
			second.crossingPosition = new Vector3((point3.x + (point4.x - point3.x) * t2), 0.0f, (point3.y + (point4.y - point3.y) * t2));
		}
	}

	return intersect;
}
