#include <iostream> // output
#include <algorithm> // EXIT_SUCCESS, etc.
#include "opencl_array.hpp"
#include <vector>

int main(int argc, char **argv){

  std::cout << "Hello world!" << std::endl;

  // set up these static members
  // TO DO, make sure you don't change them
  opencl_array::platform_id = 0;
  opencl_array::device_id = 0;
  std::vector<double> avec;
  avec.push_back(1);
  avec.push_back(2);
  std::vector<double> bvec;
  bvec.push_back(3);
  bvec.push_back(4);
  
  std::cout << "creating a = 1,2" << std::endl;
  opencl_array a(avec);
  std::cout << "a has size "<< a.n << std::endl;
  std::cout << "creating b = 3,4" << std::endl;
  opencl_array b(bvec);
  std::cout << "b has size "<< b.n << std::endl;


  b+=a;
  std::cout << "about to get b and assign to c" << std::endl;
  std::vector<double> cvec = b.get();
  std::cout << "c has size "<< cvec.size() << std::endl;

  for (std::vector<double>::iterator it = cvec.begin(); it != cvec.end(); ++it){
    std::cout << *it << std::endl;
  }
  
  return EXIT_SUCCESS;
}
