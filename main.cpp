#include <iostream> // output
#include <algorithm> // EXIT_SUCCESS, etc.
#include "opencl_array.hpp"
#include <vector>
#include <typeinfo>// for typeid below

// convenience function here
template<typename T>
std::ostream & operator<<(std::ostream & os, const std::vector<T>& a){
  int n = a.size();
  os << "std::vector<" << typeid(T).name() << ">(";
  if (n > 6){
    os << a[0] << "," << a[1] << "," << a[2] << ",...[" << n << "total]...," << a[n-3] << "," << a[n-2] << "," << a[n-1];
  } else {
    for (typename std::vector<T>::const_iterator it = a.begin(); it != a.end(); ++it) {
      os << *it;
      if (it != a.end()-1) os << ",";
    }
  }

  os << ")";
}

// some simple tests
int main(int argc, char **argv){

  std::cout << "Hello world!" << std::endl;

  // set up these static members
  // TO DO, make sure you don't change them
  opencl_array::set_platform_id(0);
  opencl_array::set_device_id(0);
  std::vector<double> avec;
  avec.push_back(1);
  avec.push_back(2);
  avec.push_back(3);
  avec.push_back(4);
  avec.push_back(5);
  avec.push_back(6);
  avec.push_back(7);
  std::vector<double> bvec;
  bvec.push_back(3);
  bvec.push_back(4);
  bvec.push_back(5);
  bvec.push_back(6);
  bvec.push_back(7);
  bvec.push_back(8);
  bvec.push_back(9);
  
  opencl_array a(avec);
  std::cout << "a is" << a << std::endl;;
  opencl_array b(bvec);
  std::cout << "b is" << b << std::endl;

  std::cout << " b+= a" << std::endl;
  b+=a;
  std::cout << "b is " << b << std::endl;
  std::cout << "about to get b and assign to cvec" << std::endl;
  std::vector<double> cvec = b.get();
  std::cout << "cvec is " << cvec << std::endl;

  std::cout << "opencl_array d = b - a" << std::endl;
  opencl_array d = b - a;
  std::cout << "d is " << d << std::endl;

  std::cout << "d/=2" << std::endl;
  d /= 2;
  std::cout << "d is " << d << std::endl;

  std::vector<double> xvec;
  xvec.push_back(0);
  xvec.push_back(0);
  xvec.push_back(0);
  xvec.push_back(1);
  xvec.push_back(0);
  xvec.push_back(0);
  // (0,0,0), (1,0,0)  
  opencl_array x(xvec);
  
  a = opencl_array(6,1.0);
  std::cout << a << std::endl;
  
  opencl_array out = apply_gaussian_kernel(x,a,x,1.0);
  std::cout << "after kernel, out is " << out << std::endl;
  return EXIT_SUCCESS;
}
