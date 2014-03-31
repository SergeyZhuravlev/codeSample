#include "codeSample.hpp"
#include <iostream>
#include <string>
using namespace std;

const HardCodedMap testMap = {
  "        ",
  " X X    ",
  " X XX XX",
  "XX X    ",
  "   X X  ",
  "  XX    ",
  " X      ",
  "   X    "
};

void monolog(Map& map, const string& mapName, const Point2D& source, const Point2D& destination)
{
  cout << "Map " << mapName << " with size " << Point2D(map.width(), map.height()) << ":";
  outputMap(cout, map);
  cout << "Path from " << source << " to " << destination << ":" << endl;
  const auto path = map.pathFind(source, destination);
  if (!path.empty()){
    outputPath(cout, path);
    cout << endl << "Map with path: ";
    map.fillPathOnMap(path);
    outputMap(cout, map);
  } else cout << "Path not found!";
  cout << endl << endl << endl;
}

int _tmain(int argc, _TCHAR* argv[])//int main(int argc, char* argv[])
{
  (void)argc; (void)argv;
  Map map0(testMap);
  monolog(map0, "Hard coded map", { 0, 0 }, { 5, 5 });
  auto map1 = Map::generateMap(10, 16, 0.2, 0);
  monolog(map1, "Generated map1", { 2, 2 }, { 8, 8 });
  auto map2 = Map::generateMap(16, 10, 0.3, 50);
  monolog(map2, "Generated map2", { 2, 1 }, { 12, 8 });
  return 0;
}