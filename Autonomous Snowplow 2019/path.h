#ifndef _PATH_PLANNER
#define _PATH_PLANNER

#include <vector>
#include "wayqueue.h"

using namespace std;

/*  */
struct point_ {
	int x, y;
	char type;
	char buffer[3];
	bool visited;
};

/*  */
struct obstacle {
	int x, y, rad;
};

/*  */
class Path {
private:

	/*
	 mesh: meshes the area that the robot should cover.
	 map: Keeps major information about the overall map.
	 */
	point_ **mesh, **map;
	point_ robot;

	/**/
	std::vector<obstacle> *obs_list;

	/**/
	int mesh_rows, mesh_cols, map_width, map_height, definition, mesh_spacing;

	/*
	 'snow_start[2]' is a 2D array that holds the value of the mesh starting point. The mesh will grow along the x-axix and y-axis in the positive directions in euclidian space.
	 */
	int snow_start[2];

public:
	/*  */
	Path();

	/*  */
	void setMapAttributes(int width, int height, int definition);

	/*  */
	void initializeMap();

	/*  */
	void setBoxMeshAttributes(int cols, int rows, int startX, int startY, int spacing);

	/*  */
	void initializeBoxMesh();

	/*  */
	void initialzieObstacleList();

	/*  */
	void setPathAttributes(int robot_x, int robot_y);

	/*  */
	void initializeBasicPath(Wayqueue &queue);

	/*  */
	void avoid_obstacle(Wayqueue &queue, snowplow_nav_point obj);

	/*  */
	void addObstacle(int x, int y, int rad);

	/*  */
	bool check_obstacle(int *x, int *y);

	/*  */
	void printObstacleList();

	/*  */
	void printMap();

	/*  */
	void printMesh();

	/*  */
	~Path() {};
};

#endif
