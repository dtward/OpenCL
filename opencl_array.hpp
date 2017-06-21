#ifndef OPENCL_ARRAY_HPP
#define OPENCL_ARRAY_HPP

// include opencl
#include "CL/cl.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

class opencl_array {
public:
  // we want a static context (i.e. one per class)
  // declaration, not definition
  static bool initialized;
  static int platform_id;
  static cl::Platform platform;
  static cl::Context context;
  static int device_id;
  static cl::Device device;
  static cl::CommandQueue queue;
  static cl::Program program;

  // set up the context (static means we call from the class, not from an instance)
  static void setup_context(void);

  // range
  cl::NDRange offset;
  cl::NDRange global;
  cl::NDRange local;

  // a buffer
  int n; // how big?
  void set_n(int n);// also update range
  cl::Buffer buffer;
  void setup_buffer(void);
  
  // constructors
  // default
  opencl_array(void);
  // size
  opencl_array(int n);
  // data
  opencl_array(int n, double const * const v);
  // vector
  opencl_array(const std::vector<double> & v);
  // constant
  opencl_array(int n, double a);
  
  // copy, assignment, etc are NOT necessary
  // no destructors necessary, all resources are managed by open cl classes

  // compound assignment
  opencl_array & compound_assignment(const std::string & name, const opencl_array & b);
  opencl_array & operator+=(const opencl_array & b);
  opencl_array & operator-=(const opencl_array & b);
  opencl_array & operator*=(const opencl_array & b);
  opencl_array & operator/=(const opencl_array & b);
  // with scalars
  opencl_array & compound_assignment(const std::string & name, double b);
  opencl_array & operator+=(double b);
  opencl_array & operator-=(double b);
  opencl_array & operator*=(double b);
  opencl_array & operator/=(double b);


  std::vector<double> get(void) const;
  void set(const std::vector<double> & v);
  
};

// binary operators in terms of compound assignment
opencl_array operator+(opencl_array a, const opencl_array & b);
opencl_array operator-(opencl_array a, const opencl_array & b);
opencl_array operator*(opencl_array a, const opencl_array & b);
opencl_array operator/(opencl_array a, const opencl_array & b);

// streams
std::ostream & operator<<(std::ostream & os, const opencl_array & a);

#endif
