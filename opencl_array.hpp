#ifndef OPENCL_ARRAY_HPP
#define OPENCL_ARRAY_HPP
// TO DO
// include some lddmm functions
// like applying a kernel


// include opencl
#include "CL/cl.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

class opencl_array {
private:
  // ideally these variables should be private
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

  

public:


  static int get_platform_id(void); // note static member has no this pointer, so no const this pointer
  static void set_platform_id(int id);
  static const cl::Platform& get_platform(void);

  static int get_device_id(void);
  static void set_device_id(int id);
  static const cl::Device & get_device(void);

  static const cl::CommandQueue & get_queue(void);

  static const cl::Program & get_program(void);
  

  
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

  // get and set
  std::vector<double> get(bool blocking = true) const;
  void set(const std::vector<double> & v, bool blocking = false);

  int get_n(void) const;
  const cl::Buffer & get_buffer(void) const;  
};

// binary operators in terms of compound assignment
opencl_array operator+(opencl_array a, const opencl_array & b);
opencl_array operator-(opencl_array a, const opencl_array & b);
opencl_array operator*(opencl_array a, const opencl_array & b);
opencl_array operator/(opencl_array a, const opencl_array & b);

opencl_array operator+(opencl_array a, double b);
opencl_array operator-(opencl_array a, double b);
opencl_array operator*(opencl_array a, double b);
opencl_array operator/(opencl_array a, double b);

// other order
opencl_array operator+(double a, opencl_array b);
opencl_array operator-(double a, opencl_array b);
opencl_array operator*(double a, opencl_array b);
opencl_array operator/(double a, const opencl_array & b);

// streams
std::ostream & operator<<(std::ostream & os, const opencl_array & a);


// now some more exotic functions
// apply gaussian kernel, centered at x, evaluate at y
// points are stored like x[3*i + j] for j \in \{0,...,2\}
opencl_array apply_gaussian_kernel(const opencl_array & x, const opencl_array & p, const opencl_array & y, double sigma);

#endif
