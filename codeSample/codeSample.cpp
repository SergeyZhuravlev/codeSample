
// codeSample.cpp: main program module. Simple code sample. 
//Find path on 2D map by simplest breadth-first search algorithm.

//I like use BOOST-\STL- algorithms and write pure code with const-correctness and small self-documented functions,
// but can change my code style if required by Code Style Guidelines.

//Code above or project have some disadvantages:
//  Not subdivided into several files;
//  Not satisfy for release compilation;
//  Not configured PCH and compiled code elimination;
//  Hasn't overloaded operators, ::std::swap, ::std::hash, because it's not library for generic use;
//  Not use optimization.

#include "stdafx.h"
#include "codeSample.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <random>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm_ext/insert.hpp>
#include <boost/range/numeric.hpp>
#include <boost/config/suffix.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <assert.h>
#include <limits>
using namespace std;
using namespace boost;

Map Map::generateMap(const size_t width, const size_t height, const double wallFactor, const int seed){
//Not labyrinth-friendly generator.
  assert(width<(numeric_limits<decltype(width)>::max)());
  assert(height<(numeric_limits<decltype(height)>::max)());
  assert((!width) || height);
  assert(wallFactor>0. && wallFactor<1.);
  const int generatorGranularity = 1000;
  default_random_engine engine_generator(seed);
  uniform_int_distribution<> distribution(0, generatorGranularity);
  auto rand = bind(distribution, engine_generator);
  const auto generator = [&rand, wallFactor, generatorGranularity](){
    if (rand() / (1.*generatorGranularity)<wallFactor)
      return Mapel::wall;
    else
      return Mapel::void_;
  };
  Repository result(height);
  generate(result, [width, generator](){
    Dimension result(width);
    generate(result, generator);
    return result;
  });
  return result;
}

void Map::fillPathOnMap(const Path& path){
  if (!algorithm::all_of(path, bind(&Map::inRange, this, placeholders::_1)))
    throw out_of_range("Path item is out of range");
  for (const Point2D& point : path)
    (*this)[point] = Mapel::step;
}

bool Map::isPassable(const Point2D& point) const BOOST_NOEXCEPT{
  return inRange(point) && (*this)[point] != Mapel::wall;
}

Path Map::pathFind(const Point2D& source, const Point2D& destination) const{
  //Return empty Path-container, if path not found.
  if (!(inRange(destination) && inRange(source)))
    throw out_of_range("Source or destination is out of map.");
  if (!(isPassable(destination) && isPassable(source)))
    return Path();
  Wavefront wavefront = { destination };
  auto field = fieldConstruct();
  int waveDistance = 0;
  const auto wavefrontEnd = wavefront.end();
  auto currentPoint = wavefront.begin();
  while (!wavefront.empty())
  {
    while (currentPoint != wavefrontEnd)
    {
      if (stepIsNotFolly(field, waveDistance, *currentPoint)){
        field[*currentPoint] = waveDistance;
        if (source == *currentPoint)
          return fillPath(field, source, destination);
        const auto nextPoints = this->nextPoints(*currentPoint);
        insert(wavefront, currentPoint, nextPoints);
      }
      wavefront.erase(currentPoint++);
    }
    currentPoint = wavefront.begin();
    ++waveDistance;
  }
  return Path();
}

Mapel Map::charToMapItem(const char item) BOOST_NOEXCEPT_OR_NOTHROW{
  assert(item == char(Mapel::step) || item == char(Mapel::void_) || item == char(Mapel::wall));
  return static_cast<Mapel>(item);
}

char Map::mapItemToChar(const Mapel item) BOOST_NOEXCEPT_OR_NOTHROW{
  return static_cast<char>(item);
}

Map::Repository Map::toMapRepository(const HardCodedMap& map){
  assert(checkRectangularity(map));
  assert(map.size()<(numeric_limits<Repository::size_type>::max)());
  assert(map.empty() || map[0].size()<(numeric_limits<Dimension::size_type>::max)());
  assert(map.empty() || (!map[0].empty()));
  Repository result(map.size());
  transform(map, result.begin(),
    [](const HardCodedMapDimension& line){
    Dimension result;
    result.reserve(line.size());
    transform(line, back_inserter(result), charToMapItem);
    return result;
  });
  return result;
}

Map::Field Map::fieldConstruct() const{
  const auto maxField = width()*height() + 1;
  return Field::Repository(height(), Field::Dimension(width(), maxField));
}

bool Map::stepIsNotFolly(const Field& field, const int waveDistance, const Point2D& point) const BOOST_NOEXCEPT{
  return waveDistance<field[point];
}

Map::Points Map::nearPoints(const Point2D& point){
  return{ point.subtract({ 1, 0 }), point.add({ 1, 0 }), point.subtract({ 0, 1 }), point.add({ 0, 1 }) };
}

Map::Points Map::nextPoints(const Point2D& point) const {
  const auto path = nearPoints(point);
  const auto result = adaptors::filter(path, bind(&Map::isPassable, this, placeholders::_1));
  return Points(result.begin(), result.end());
}

Path Map::fillPath(const Field& field, const Point2D& source, const Point2D& destination) const {
  auto currentPoint = source;
  Path path = { source };
  while (!(currentPoint == destination)){
    const auto nextPoints = this->nextPoints(currentPoint);
    assert(!nextPoints.empty());
    if (nextPoints.empty())
      return Path();
    path.push_back(currentPoint = *min_element(nextPoints, [&field](const Point2D& current, const Point2D& next){
      return field[current]<field[next];
    }));
  }
  return path;
}

void outputMap(ostream& stream, const Map& map){
  //to do: Should use ::std::basic_ostream at industry code instead ::std::ostream.
  stream << endl;
  if (map.constData().empty()){
    stream << "Empty map." << endl;
    return;
  }
  const size_t frameSize = 2;
  ostream_iterator<char> out(stream);
  ::std::fill_n(out, map.width() + frameSize, '_');
  stream << endl;
  for (const Map::Dimension& line : map.constData()){
    stream << "|";
    transform(line, out, &Map::mapItemToChar);
    stream << "|" << endl;
  };
  ::std::fill_n(out, map.width() + frameSize, '-');
  stream << endl;
}