#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "path.h"
#include "wayqueue.h"

using namespace std;

/*--------------------------- CONSTRUCTORS ---------------------------*/

/*  */
Path::Path() {}

/*--------------------------- MAP ---------------------------*/

/*  */
void Path::setMapAttributes(int width, int height, int definition)
{
	this->definition = definition;  // set definition / map-precision
	this->map_width = width * definition;    // set width of overall map
	this->map_height = height * definition;  // set height of overall map

	int y;  // for-loop variable
	map = new point_*[map_height];
	for (y = 0; y < map_height; y++)
		map[y] = new point_[map_width];
}

/*  */
void Path::initializeMap()
{
	int x, y; // for-loop variables
	for (y = 0; y < map_height; y++) {
		for (x = 0; x < map_width; x++) {
			map[y][x].x = 0;
			map[y][x].y = 0;
			map[y][x].type = '-';
		}
	}
}

/*  */
void Path::printMap()
{
	int x, y; // for-loop variables
	for (y = 0; y < map_height; y++) {
		for (x = 0; x < map_width; x++) {
			cout << " " << map[y][x].type;
		}
		cout << endl;
	}
}

/*--------------------------- MESH ---------------------------*/

/*  */
void Path::setBoxMeshAttributes(int cols, int rows, int startX, int startY, int spacing)
{
	this->mesh_rows = rows;   // sets number of rows that will be meshed
	this->mesh_cols = cols;   // sets the number of cols that will be meshed
	this->snow_start[0] = startX;    // sets start x-value of Mesh
	this->snow_start[1] = startY;    // sets start y-value of Mesh
	this->mesh_spacing = spacing;
	int y;  // for-loop variable
	mesh = new point_*[mesh_cols];
	for (y = 0; y < mesh_cols; y++) {
		mesh[y] = new point_[mesh_rows];
	}
}

/*  */
void Path::initializeBoxMesh()
// Needs to take in a set of obstacles so that it can avoid them (possible inputs)
{
	// Make sure the total i_x and i_y doesn't go beyond the maps dimensions.
	// need to make sure to check all failures
	int x, y, m;
	int inc_x, inc_y, i_x, i_y;
	inc_x = snow_start[0];
	inc_y = snow_start[1];
	i_x = inc_x;
	i_y = inc_y;
	for (y = 0; y < mesh_cols; y++) {
		for (x = 0; x < mesh_rows; x++) {
			mesh[y][x].x = i_x;
			mesh[y][x].y = i_y;
			mesh[y][x].type = 'O';
			map[i_y][i_x] = mesh[y][x];
			i_x = i_x + mesh_spacing;
		}
		i_x = snow_start[0];
		i_y = i_y + mesh_spacing;
	}
}

/*  */
void Path::printMesh()
{
	int x, y; // for-loop variables
	for (y = 0; y < mesh_cols; y++) {
		for (x = 0; x < mesh_rows; x++) {
			cout << "(" << mesh[y][x].x << ", " << mesh[y][x].y << ")  ";
		}
		cout << endl;
	}
}

/*--------------------------- OBSTACLES ---------------------------*/

/*  */
void Path::initialzieObstacleList()
{
	obs_list = new vector<obstacle>;
}

/*  */
void Path::addObstacle(int at_x, int at_y, int rad)
{
	obstacle obs_temp;
	obs_temp.x = at_x;
	obs_temp.y = at_y;
	obs_temp.rad = rad;
	obs_list->push_back(obs_temp); // pushes the obstacel to the obs_list.
	map[at_y][at_x].type = 'X'; // sets the obstacle positions to the map.

	int x, y;
	for (y = 0; y < mesh_cols; y++) {
		for (x = 0; x < mesh_rows; x++) {
			int mesh_x, mesh_y;
			mesh_x = mesh[y][x].x;
			mesh_y = mesh[y][x].y;
			if (check_obstacle(&mesh_x, &mesh_y)) {
				cout << "(x, y): " "(" << mesh_x << ", " << mesh_y << ")" << endl;

			}
		}
	}
}

/*  */
bool Path::check_obstacle(int *x, int *y)
{
	int i, sz;
	sz = obs_list->size();
	//iterate through all the obstacles
	for (i = 0; i < sz; i++) {
		//get x and y diff between obstacle and point sent to function
		int x_diff = abs(obs_list->at(i).x - *x);
		int y_diff = abs(obs_list->at(i).y - *y);

		int dist = x_diff * x_diff + y_diff * y_diff;
		int rads = obs_list->at(i).rad;
		int rad_sqr = rads * rads;
		if (dist <= rad_sqr) {
			return true;
		}
	}
	return false;
}

/*  */
void Path::printObstacleList()
{
	int i, sz;

	sz = obs_list->size();
	cout << obs_list->size() << endl;
	for (i = 0; i < sz; i++)
		cout << "(" << obs_list->at(i).x << ", " << obs_list->at(i).x << ")  ";
	cout << endl;
}

/*--------------------------- PATH ---------------------------*/

/*  */
void Path::setPathAttributes(int robot_x, int robot_y)
{
	robot.x = robot_x;
	robot.y = robot_y;
}

/*  */
void Path::initializeBasicPath(Wayqueue &queue)
{
	Point temp2;
	int x, y, loop_config; // for-loop variables
	loop_config = 0;
	for (y = 0; y < mesh_cols; y++) {

		// forward loop
		if (loop_config == 0) {
			for (x = 0; x < mesh_rows; x++) {
				Point temp;
				temp.x = mesh[y][x].x/100.0;
				temp.y = mesh[y][x].y/100.0;
				queue.push(&temp);
			}
			loop_config = 1;
			continue;
		}

		// backward loop
		if (loop_config == 1) {
			for (x = mesh_rows - 1; x >= 0; x--) {
				Point temp;
				temp.x = mesh[y][x].x/100.0;
				temp.y = mesh[y][x].y/100.0;
				queue.push(&temp);
			}
			loop_config = 0;
		}
		
		temp2.x = 2;
		temp2.y = 1.5;

	}
		queue.push(&temp2);
}