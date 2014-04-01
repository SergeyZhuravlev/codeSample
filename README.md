codeSample
==========

Simple example of my code for paralympic game at some Internet forum. 

Find path on 2D map by simplest breadth-first search algorithm.

I like use BOOST-\STL- algorithms and write pure code with const-correctness and small self-documented functions,
 but can change my code style if required by Code Style Guidelines.
Code written with strong exception safety guarantee.

Code above or project have some disadvantages:
  * Not subdivided into several files;
  * Not satisfy for release compilation;
  * Not configured PCH and compiled code elimination;
  * Hasn't overloaded operators, ::std::swap, ::std::hash, because it's not library for generic use;
  * Not used optimizations;
  * Not configured git:ignore.

Dependencies: BOOST >= 1.55.0, Google Test Framework >= 1.7.0, Microsoft Visual Studio >= 2013

Let see:
  * "codeSample\codeSample.hpp"
  * "codeSample\codeSample.cpp"
  * "test\test.cpp"
  * "codeSample\main.cpp"