#include <stdio.h>
#include <iostream>
#include <vector>


//    2D Path Finding System auf Netzwerk von Wegpunkten
//    –Laden des Netzes von Disk
//    –Pfad finden von Start zu Ziel
//    –Optimieren von Pfad optional
//    –Wie sieht die Low-Level API aus?

enum tile {walkable, blocked};

namespace MyEngine
{
	//defined for compilation, should be included externally
	class FileSystem
	{

	};

	struct Point
	{
		float x;
		float y;

		Point(float _x, float _y): x(_x),y(_y){}
	};
		
	struct Grid
	{
		unsigned int sizeX, sizeY;
		bool** tileWalkable;
		Grid(unsigned int width, unsigned int height);

		void setTileWalkable(tile walkable, int posX, int posY);
		
	};
	
	struct Path
	{
		std::vector<Point> points;
		bool canGoDiagonally = true;
		float getLength();
	};

	// Grid based path finding system
	class PathFindingSystem
	{
	public:
		
		// High level interface

		void findPath(Path& path, Grid& grid, const Point& point1, const Point& point2);
		void optimizePath(Path& path);
		void interpolatePath(Path& path, float interpolation_amount);
		

		// Mid level interface

		bool isConnected(const Grid& grid, const Point& point1, const Point& point2);
		void loadGrid(FileSystem& filesystem, std::string filePath, Grid& grid);

		
		// Low level interface

		bool findMultiplePaths(int numPaths, Path* pathArray, const Grid& grid);
	};

}



using namespace MyEngine;
int main(int argc, const char* argv[])
{
	FileSystem fs;
	PathFindingSystem pfs;
	
	Point point1 = Point(1, 2);
	Point point2 = Point(2, 4);
	
	Grid grid = Grid(10, 10);
	pfs.loadGrid(fs, "myPath", grid);	

	grid.setTileWalkable(blocked, 5, 5);

	Path path1 = Path();
	pfs.findPath(path1, grid, point1, point2);

	if (pfs.isConnected(grid, point1, point2)) {
		std::cout << "A path exists between the two points" << std::endl;
	}	
}
