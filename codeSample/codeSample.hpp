#ifndef __CODESAMPLE_H__
#define __CODESAMPLE_H__

// codeSample.hpp: main program module header. Simple code sample. 
//Find path on 2D map by simplest breadth-first search algorithm.

//I like use BOOST-\STL- algorithms and write pure code with const-correctness and small self-documented functions,
// but can change my code style if required by Code Style Guidelines.
//Code written with strong exception safety guarantee.

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
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/operators.hpp>
#include <boost/config/suffix.hpp>
#include <assert.h>

class Point2D : public ::boost::equality_comparable1<Point2D>{
public:
  Point2D() = default;
  Point2D(const size_t x, const size_t y) BOOST_NOEXCEPT_OR_NOTHROW :
    x(x),
    y(y)
  {}
  Point2D& operator=(const Point2D&) = default;
  Point2D(const Point2D&) = default;
  size_t X() const BOOST_NOEXCEPT_OR_NOTHROW { return x; }
  size_t Y() const BOOST_NOEXCEPT_OR_NOTHROW { return y; }
  friend bool operator==(const Point2D& l, const Point2D& r) BOOST_NOEXCEPT_OR_NOTHROW {
    return l.x == r.x && l.y == r.y;
  }
  Point2D add(const Point2D& r) const BOOST_NOEXCEPT_OR_NOTHROW {
    return{ x + r.x, y + r.y };
  }
  Point2D subtract(const Point2D& r) const BOOST_NOEXCEPT_OR_NOTHROW {
    return{ x - r.x, y - r.y };
  }
  template< class CharT, class Traits>
  friend ::std::ostream& operator<<(::std::basic_ostream<CharT, Traits>& stream, const Point2D& point){
    //to do: Should use ::std::basic_ostream at industry code instead ::std::ostream.
    stream << "(" << point.x << ", " << point.y << ")";
    return stream;
  }

private:
  size_t x = 0, y = 0;
};

typedef ::std::string HardCodedMapDimension;
typedef ::std::vector<HardCodedMapDimension> HardCodedMap;

typedef ::std::vector<Point2D> Path;

enum class Mapel {
  wall = 'X',
  void_ = ' ',
  step = '*'
};

template<class Container2D>
bool checkRectangularity(const Container2D& forVerify) BOOST_NOEXCEPT_OR_NOTHROW {
  if (forVerify.empty())
    return true;
  const auto width = forVerify.back().size();
  typedef typename Container2D::value_type Container;
  //to do: Should use template trait at industry code instead Container2D::value_type.
  return ::boost::accumulate(forVerify, true, [width](const bool forAll, const Container& forCheck){
    return forAll && (forCheck.size() == width);
  });
}

template<class Item>
class Matrix{
public:
  typedef ::std::vector<Item> Dimension;
  typedef ::std::vector<Dimension> Repository;
  Matrix() = default;
  Matrix(const Matrix&) = default;
  //Matrix(Matrix&&) = default;//Commented old code and replaced to another after porting from gcc.
  Matrix(Matrix&& source) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(Repository(::std::move(source.data)))) :
    data(::std::move(source.data))
  {}
  /*Matrix& operator=(const Matrix&) = default;
  Matrix& operator=(Matrix&&) = default;*/
  Matrix& operator=(Matrix source) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(::std::declval<Repository&>()=::std::move(source.data))){
    this->data = ::std::move(source.data);
    return *this;
  }
  Matrix(Repository data) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(Repository(::std::move(source.data)))) :
    data(::std::move(data))
  {
    assert(checkRectangularity(this->data));
  }
  Item& operator[](const Point2D& point) {//throw(::std::out_of_range)
    if (!this->inRange(point))
      throw out_of_range("Point is out of range.");
    return this->data[point.Y()][point.X()];
  }
  const Item& operator[](const Point2D& point) const {//throw(::std::out_of_range)
    if (!this->inRange(point))
      throw out_of_range("Point is out of range.");
    return this->data[point.Y()][point.X()];
  }
  bool inRange(const Point2D& point) const BOOST_NOEXCEPT_OR_NOTHROW {
    if (this->data.empty())
      return false;
    return point.X()<this->width() && point.Y()<this->height();
  }
  size_t width() const BOOST_NOEXCEPT_OR_NOTHROW {
    if (this->data.empty())
      return 0;
    return this->data[0].size();
  }
  size_t height() const BOOST_NOEXCEPT_OR_NOTHROW {
    return this->data.size();
  }
  const Repository& constData() const BOOST_NOEXCEPT_OR_NOTHROW {
    return this->data;
  }
private:
  Repository data;
};

class Map : public Matrix<Mapel>{
  typedef Matrix<Mapel> Base;
public:
  //using Base::Matrix<Mapel>;
  //using Base::operator=;
  Map() = default;
  Map(const Map&) = default;
  Map(Map&& source) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(Base(::std::move(source)))) :
    Base(::std::move(source))
  {};
  Map(Repository map) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(Base(::std::move(map)))) :
    Base(::std::move(map))
  {}
  Map& operator=(Map source) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(::std::declval<Base&>()=(::std::move(map)))) {
    this->Base::operator=(::std::move(source));
    return *this;
  }
  explicit Map(const HardCodedMap& map) : //throw (::std::bad_alloc, ...)
    Base(toMapRepository(map))
  {}
  static Map generateMap(const size_t width, const size_t height, const double wallFactor, const int seed);
  //^Not labyrinth-friendly generator.
  void fillPathOnMap(const Path& path);//throw(::std::out_of_range)
  bool isPassable(const Point2D& point) const BOOST_NOEXCEPT_OR_NOTHROW;
  Path pathFind(const Point2D& source, const Point2D& destination) const;//throw(::std::out_of_range, ::std::bad_alloc, ...)
  //^Return empty Path-container, if path not found.
  static Mapel charToMapItem(const char item) BOOST_NOEXCEPT_OR_NOTHROW;
  static char mapItemToChar(const Mapel item) BOOST_NOEXCEPT_OR_NOTHROW;
  static Repository toMapRepository(const HardCodedMap& map);//throw (::std::bad_alloc, ...)

private:
  typedef Matrix<int> Field;
  typedef ::std::list<Point2D> Wavefront;
  typedef ::std::vector<Point2D> Points;

  Field fieldConstruct() const;
  bool stepIsNotFolly(const Field& field, const int waveDistance, const Point2D& point) const BOOST_NOEXCEPT_OR_NOTHROW;
  static Points nearPoints(const Point2D& point);
  Points nextPoints(const Point2D& point) const;
  Path fillPath(const Field& field, const Point2D& source, const Point2D& destination) const;
};

void outputMap(::std::ostream& stream, const Map& map);
//to do: ^Should use ::std::basic_ostream at industry code instead ::std::ostream.

template< class CharT, class Traits>
void outputPath(::std::basic_ostream<CharT, Traits>& stream, const Path& path){
  ::boost::copy(path, ::std::ostream_iterator<Point2D>(stream));
}
#endif // !__CODESAMPLE_H__