
// test.cpp: Unit tests for codeSample project.

//to do: Write "compilation failure is success" unit tests.


#include "stdafx.h"
#include "codeSample.hpp"
#include <vector>
#include <list>
#include <limits>
#include "gtest.h"
#include "boost/range/numeric.hpp"
using namespace std;
using namespace boost;

namespace {
  struct Point2DTest : ::testing::Test {
    const size_t ax = 2, ay = 5, bx = 1, by = 5, cx = 0, cy = 0;
    const Point2D a{ ax, ay }, b{ bx, by }, c;
    Point2DTest():c(){}
  };

  TEST_F(Point2DTest, TestCtorAndAxes) {
    const Point2D d(a);
    Point2D e(move(d));
    const Point2D f(move(e));
    EXPECT_EQ(ax, a.X());
    EXPECT_EQ(ay, a.Y());
    EXPECT_EQ(bx, b.X());
    EXPECT_EQ(by, b.Y());
    EXPECT_EQ(cx, c.X());
    EXPECT_EQ(cy, c.Y());
    EXPECT_EQ(ax, d.X());
    EXPECT_EQ(ay, d.Y());
    EXPECT_EQ(ax, f.X());
    EXPECT_EQ(ay, f.Y());
    EXPECT_EQ(ax, e.X());
    EXPECT_EQ(ay, e.Y());
  }

  TEST_F(Point2DTest, TestEquality) {
    EXPECT_EQ(a, a);
    EXPECT_NE(b, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(c, c);
    EXPECT_NE(a, c);
    EXPECT_NE(c, a);
    EXPECT_EQ(Point2D(), c);
    EXPECT_EQ(Point2D(ax, ay), a);
  }

  TEST_F(Point2DTest, TestCopyAssigment) {
    Point2D d = a;
    EXPECT_EQ(a, d);
    EXPECT_EQ(b, d = b);
    EXPECT_EQ(b, d);
    EXPECT_EQ(d, d = d);
    EXPECT_EQ(b, d);
  }

  TEST_F(Point2DTest, TestOutput) {
     std::ostringstream output;
     output<<a;
     EXPECT_EQ("(2, 5)", output.str());
     output.str("");
     output<<b<<b;
     EXPECT_EQ("(1, 5)(1, 5)", output.str());
  }
  
  TEST_F(Point2DTest, TestOperations) {
    EXPECT_EQ(Point2D(3, 10), a.add(b));
    EXPECT_EQ(Point2D(4, 10), a.add(a));
    EXPECT_EQ(Point2D(6, 15), a.add(a).add(a));
    EXPECT_EQ(a, a.add(a).subtract(a));
    EXPECT_EQ(Point2D(1, 0), a.subtract(b));
    EXPECT_EQ(Point2D(0, 0), a.subtract(a));
    const size_t axisUpperBound = (numeric_limits<size_t>::max)();
    EXPECT_EQ(Point2D(axisUpperBound, axisUpperBound), c.subtract({ 1, 1 }));
    EXPECT_EQ(Point2D(ax, ay), a);
    EXPECT_EQ(Point2D(cx, cy), c);
  }
  
  TEST(checkRectangularityTest, forEmpty){
    list<list<int>> a;
    EXPECT_TRUE(checkRectangularity(a));
  }
  
  TEST(checkRectangularityTest, forRectangle){
    list<list<int>> a(5, list<int>(9));
    EXPECT_TRUE(checkRectangularity(a));
    vector<list<int>> b(2, list<int>(4));
    EXPECT_TRUE(checkRectangularity(b));
    vector<vector<int>> c(2, vector<int>(4));
    EXPECT_TRUE(checkRectangularity(c));
  }
  
  TEST(checkRectangularityTest, forNonRectangle){
    vector<list<int>> a(2, list<int>(4));
    ASSERT_TRUE(checkRectangularity(a));
    a[1].push_back(0);
    EXPECT_FALSE(checkRectangularity(a));
    a[1].pop_back();
    EXPECT_TRUE(checkRectangularity(a));
    a.emplace_back();
    EXPECT_FALSE(checkRectangularity(a));
  }
  
  struct MatrixTest: ::testing::Test{
    typedef Matrix<int> Tested;
    typedef Tested::Repository Repository;
    typedef Tested::Dimension RepositoryDimension;
    const Repository h1{ {
      { 1, 2, 3 },
      { 4, 5, 6 }
    } };
    const Tested a{ h1 }, b;
    MatrixTest():b(){}
    
    template<class Container>
    void checkH1Initialization(const Container& a) const{
      ASSERT_EQ(3, a.width());
      ASSERT_EQ(2, a.height());
      EXPECT_EQ(h1, a.constData());
      EXPECT_EQ(2, a.constData()[0][1]);
      EXPECT_EQ(2, (a[{1, 0}]));
      EXPECT_EQ(1, (a[{0, 0}]));
      EXPECT_EQ(6, (a[{2, 1}]));
      EXPECT_EQ(6, accumulate(a.constData()[0], 0));
      EXPECT_EQ(15, accumulate(a.constData()[1], 0));
    }
  };
  
  TEST_F(MatrixTest, InitializationAndIndices){
    EXPECT_EQ(0, b.width());
    EXPECT_EQ(0, b.height());
    this->checkH1Initialization(a);
    Tested c(a);
    this->checkH1Initialization(c);
    Tested d(move(c));
    this->checkH1Initialization(d);
    EXPECT_EQ(0, c.width());
    EXPECT_EQ(0, c.height());
    this->checkH1Initialization(c = d);
    this->checkH1Initialization(d);
    this->checkH1Initialization(c);
    c = b;
    EXPECT_EQ(0, c.width());
    EXPECT_EQ(0, c.height());
    this->checkH1Initialization(c = move(d));
    EXPECT_EQ(0, d.width());
    EXPECT_EQ(0, d.height());
    this->checkH1Initialization(c);
    this->checkH1Initialization(a);
    EXPECT_EQ(0, b.width());
    EXPECT_EQ(0, b.height());    
  }
  
  TEST_F(MatrixTest, InRange){
    EXPECT_TRUE(a.inRange({1,1}));
    EXPECT_FALSE(b.inRange({1,1}));
    EXPECT_TRUE(a.inRange({0, 0}));
    EXPECT_FALSE(b.inRange({0, 0}));
    EXPECT_TRUE(a.inRange({0, 1}));
    EXPECT_FALSE(b.inRange({0, 1}));
    EXPECT_TRUE(a.inRange({2, 1}));
    EXPECT_FALSE(a.inRange({3, 1}));
    EXPECT_FALSE(a.inRange({2, 2}));
    const auto nl = (numeric_limits<size_t>::max)();
    EXPECT_FALSE(a.inRange({nl, 1}));
    EXPECT_FALSE(a.inRange({nl, 0}));
    EXPECT_FALSE(a.inRange({0, nl}));
    EXPECT_FALSE(a.inRange({nl, nl}));
    EXPECT_FALSE(b.inRange({nl, nl}));
    EXPECT_THROW((a[{3, 1}]), out_of_range);
    EXPECT_THROW((a[{1, 2}]), out_of_range);
  }
  
  TEST(outputPathTest, simple){
    const Path a, b;
    const Path c {{1,2},{1,3},{2,3}}, d(c);
    ostringstream r;
    ASSERT_EQ("", r.str());
    outputPath(r, a);
    cout << r.str()<<endl;
    EXPECT_EQ("", r.str());
    r.str("");
    ASSERT_EQ("", r.str());
    outputPath(r, c);
    EXPECT_EQ("(1, 2)(1, 3)(2, 3)", r.str());
    EXPECT_EQ(b, a);
    EXPECT_EQ(d, c);
  }
  
  struct MapTest: ::testing::Test{
    const HardCodedMap h1{ {
      "X X ",
      "X   ",
      "XXX "
    } };
    const Map c, d;
    const Map a{ h1 }, b { h1 };
    MapTest():c(), d(){}
    template<class Container>
    void checkH1Initialization(const Container& e){
      ASSERT_EQ(3, e.height());
      ASSERT_EQ(4, e.width());
      EXPECT_EQ('X', static_cast<char>(e[{2,0}]));
      EXPECT_EQ(' ', static_cast<char>(e[{1, 0}]));
      EXPECT_EQ(' ', static_cast<char>(e[{3, 2}]));
    }
  };
  
  TEST_F(MapTest, Static){
    const auto r = Map::toMapRepository(h1);
    EXPECT_EQ(3, r.size());
    EXPECT_EQ(4, r[0].size());
    for(const auto l : r)
      EXPECT_EQ(4, l.size());
    EXPECT_EQ(Mapel::wall, Map::charToMapItem('X'));
    EXPECT_EQ(Mapel::void_, Map::charToMapItem(' '));
    EXPECT_EQ(Mapel::step, Map::charToMapItem('*'));
    EXPECT_EQ('X', Map::mapItemToChar(Mapel::wall));
    EXPECT_EQ(' ', Map::mapItemToChar(Mapel::void_));
    EXPECT_EQ('*', Map::mapItemToChar(Mapel::step));
  }
  
  TEST_F(MapTest, CtorsAndAssigment){
    ASSERT_EQ(b.constData(), a.constData());
    ASSERT_EQ(d.constData(), c.constData());
    Map e(a);
    Map f(move(a));
    this->checkH1Initialization(f);
    this->checkH1Initialization(e);
    this->checkH1Initialization(a);
    EXPECT_EQ(a.constData(), e.constData());
    EXPECT_EQ(a.constData(), f.constData());
    e = c;
    EXPECT_EQ(0, e.height());
    EXPECT_EQ(0, e.width());
    f = e;
    EXPECT_EQ(0, f.height());
    EXPECT_EQ(0, f.width());
    this->checkH1Initialization(e = a);
    this->checkH1Initialization(e);
    this->checkH1Initialization(f = move(e));
    this->checkH1Initialization(f);
    EXPECT_EQ(0, e.height());
    EXPECT_EQ(0, e.width());
    const Map g(move(f));
    EXPECT_EQ(0, f.height());
    EXPECT_EQ(0, f.width());
    this->checkH1Initialization(g);
    this->checkH1Initialization(a);
    this->checkH1Initialization(b);
    EXPECT_EQ(b.constData(), a.constData());
    EXPECT_EQ(d.constData(), c.constData());
    EXPECT_EQ(f.constData(), c.constData());
    EXPECT_EQ(g.constData(), a.constData());
  }
  
  TEST_F(MapTest, generateMap){
    const Map m(Map::generateMap(5, 8, 0.5, 0));
    EXPECT_EQ(5, m.width());
    EXPECT_EQ(8, m.height());
    const auto & md = m.constData();
    EXPECT_FALSE(md.end()!=find_if(md.begin(), md.end(),
      [](const Map::Dimension& mdd){
        return mdd.end()!=find_if(mdd.begin(), mdd.end(), [](const Mapel el){
          return !(el==Mapel::void_ || el==Mapel::wall);
        }); }));
  }
  
  TEST_F(MapTest, isPassable){
    EXPECT_TRUE(a.isPassable({1,1}));
    EXPECT_FALSE(c.isPassable({1,1}));
    EXPECT_FALSE(a.isPassable({0, 0}));
    EXPECT_FALSE(c.isPassable({0, 0}));
    EXPECT_FALSE(a.isPassable({0, 1}));
    EXPECT_FALSE(c.isPassable({0, 1}));
    EXPECT_TRUE(a.isPassable({2, 1}));
    EXPECT_FALSE(a.isPassable({1, 3}));
    EXPECT_FALSE(a.isPassable({2, 2}));
    EXPECT_TRUE(a.isPassable({3, 2}));
    const auto nl = (numeric_limits<size_t>::max)();
    EXPECT_FALSE(a.isPassable({nl, 1}));
    EXPECT_FALSE(a.isPassable({nl, 0}));
    EXPECT_FALSE(a.isPassable({0, nl}));
    EXPECT_FALSE(a.isPassable({nl, nl}));
    EXPECT_FALSE(c.isPassable({nl, nl}));
  }
  
  TEST_F(MapTest, fillPathOnMap){
    Map m(h1);
    ASSERT_EQ(a.constData(), m.constData());
    m.fillPathOnMap(Path());
    EXPECT_EQ(a.constData(), m.constData());
    m.fillPathOnMap({{1,2}, {2,2}});
    EXPECT_NE(a.constData(), m.constData());
    EXPECT_EQ(Mapel::step, (m[{1,2}]));
    EXPECT_EQ(Mapel::step, (m[{2,2}]));
    EXPECT_NE(Mapel::step, (m[{2,1}]));
    EXPECT_THROW(m.fillPathOnMap({ { 4, 4 } }), ::std::out_of_range);
  }
  
  TEST_F(MapTest, pathFind){
    Map m(a);
    const auto p1 = m.pathFind({ 1, 0 }, { 3, 0 });
    ASSERT_EQ(5, p1.size());
    EXPECT_EQ(Point2D(1, 0), p1[0]);
    EXPECT_EQ(Point2D(1, 1), p1[1]);
    EXPECT_EQ(Point2D(2, 1), p1[2]);
    EXPECT_EQ(Point2D(3, 1), p1[3]);
    EXPECT_EQ(Point2D(3, 0), p1[4]);
    EXPECT_EQ(5, m.pathFind({1, 0}, {3,2}).size());
    EXPECT_EQ(1, m.pathFind({1, 0}, {1,0}).size());
    m[{1,1}]=Mapel::wall;
    EXPECT_TRUE(m.pathFind({1, 0}, {3,0}).empty());
    m[{1,1}]=Mapel::void_;
    m[{0,0}]=Mapel::void_;
    EXPECT_THROW(m.pathFind({ 1, 0 }, { 4, 0 }), ::std::out_of_range);
    EXPECT_THROW(m.pathFind({ 1, 0 }, { 4, 3 }), ::std::out_of_range);
    EXPECT_THROW(m.pathFind({ 1, 0 }, { 4, 4 }), ::std::out_of_range);
    EXPECT_THROW(m.pathFind({ 4, 4 }, { 1, 0 }), ::std::out_of_range);
    const auto nl = (numeric_limits<size_t>::max)();
    EXPECT_THROW(m.pathFind({ 0, 0 }, { nl, 0 }), ::std::out_of_range);
    EXPECT_THROW(m.pathFind({ nl, 0 }, { nl, 0 }), ::std::out_of_range);
    EXPECT_THROW(m.pathFind({ nl, 0 }, { 0, 0 }), ::std::out_of_range);
    EXPECT_EQ(Path(1), m.pathFind({ 0, 0 }, { 0, 0 }));
    EXPECT_EQ(Path(), a.pathFind({ 0, 0 }, { 0, 0 }));
    EXPECT_THROW(c.pathFind({ 0, 0 }, { 0, 0 }), ::std::out_of_range);
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}