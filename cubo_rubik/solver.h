#ifndef SOLVER_H
#define SOLVER_H
#include "Cube.hpp"
#include "Cross.hpp"
#include "Corners.hpp"
#include "Edges.hpp"
#include "OLL.hpp"
#include "PLL.hpp"
#include <iostream>
#include <vector>

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

std::string format(std::string s) {
  std::string formatted;

  for (int i=0; i<s.length(); ++i) {
    if (s[i] == '\'') {
      formatted += s[i-1];
      formatted += s[i-1];
    }
    else if (s[i] == '2') {
      formatted += s[i-1];
    }
    else if (s[i] == ' ') {

    }
    else {
      formatted += s[i];
    }
  }

  return formatted;

}


std::vector<std::string> process(std::string argString){
  TCube myCube(false);
  Cross cross;
  Corners corners;
  Edges edges;
  OLL oll;
  PLL pll;
  std::string scramble = format(argString);
  myCube.moves(scramble);
  cross.solveCross(myCube);
  std::string ga = cross.cross_moves;
  corners.solveCorners(myCube);
  ga += corners.corners_moves;  
  edges.solveEdges(myCube);
  ga += edges.edges_moves;
  oll.solveOLL(myCube);
  ga += oll.oll_moves;
  pll.solvePLL(myCube);
  ga += pll.pll_moves;
  std::cout << "FINAL SEQUENCE: "<< ga << std::endl;

  std::vector<std::string> in;
  split(ga, in, ' ');
  return in;
}

/*
int main(int argc, char **argv) {
    std::string argString = "F R B";
    std::vector<std::string> result = process(argString);
    for(int i = 0; i < result.size(); i++){
      std::cout<<result[i]<<std::endl;
    }
    
    
    

    // myCube.output();
  // }
  return 0;
}
*/
#endif