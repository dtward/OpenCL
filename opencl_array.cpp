#include "opencl_array.hpp"
#include <exception>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <streambuf>

// define static members (definition not declaration)
bool opencl_array::initialized = false;
int opencl_array::platform_id = 0;
cl::Platform opencl_array::platform;
int opencl_array::device_id = 0;
cl::Context opencl_array::context;
cl::Device opencl_array::device;
cl::CommandQueue opencl_array::queue;
cl::Program opencl_array::program;


int opencl_array::get_platform_id(void) {
  return platform_id;
}
void opencl_array::set_platform_id(int id){
  if (initialized){
    throw std::runtime_error("You cannot change opencl context variables after your arrays are initialized");
  }
  platform_id = id;
}
const cl::Platform & opencl_array::get_platform(void){
  return platform;
}

int opencl_array::get_device_id(void){
  return device_id;
}
void opencl_array::set_device_id(int id){
  if (initialized){
    throw std::runtime_error("You cannot change opencl context variables after your arrays are initialized");
  }
  device_id = id;
}
const cl::Device & opencl_array::get_device(void){
  return device;
}

const cl::CommandQueue & opencl_array::get_queue(void){
  return queue;
}

const cl::Program & opencl_array::get_program(void){
  return program;
}


int opencl_array::get_n(void) const {
  return n;
}



void opencl_array::setup_context(void){
  //std::cout << "in setup_context" << std::endl;
  if (initialized){
    //std::cout << "context already initialized" << std::endl;
    return;
  }
  std::cout << "setting up context" << std::endl;

  // query platforms
  int err;
  std::vector<cl::Platform> platforms;
  err = cl::Platform::get(&platforms);
  if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL platforms");
  std::cout << "listing platforms" << std::endl;
  for (std::vector<cl::Platform>::const_iterator it = platforms.begin(); it != platforms.end(); ++it){
    std::cout << "Platform: " << it->getInfo<CL_PLATFORM_NAME>(&err) << std::endl;
    if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL platform name");

  }
  platform = platforms.at(platform_id);// [] does not throw
  std::cout << "selecting platform " << platform_id << ", " << platform.getInfo<CL_PLATFORM_NAME>(&err) << std::endl;

  // now set up a context
  cl_context_properties context_properties[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0};
  context = cl::Context(CL_DEVICE_TYPE_ALL, context_properties);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create OpenCL context");
    

  // query devices
  std::vector<cl::Device> devices;
  err = context.getInfo(CL_CONTEXT_DEVICES, &devices);
  if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL devices");
  std::cout << "listing devices" << std::endl;
  for (std::vector<cl::Device>::const_iterator it = devices.begin(); it != devices.end(); ++it){
    std::cout << "Device: " << it->getInfo<CL_DEVICE_NAME>(&err) << std::endl;
    if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL device name");
  }
  device = devices.at(device_id);
  std::cout << "selecting device " << device_id << ", " << device.getInfo<CL_DEVICE_NAME>(&err) << std::endl;


  // make a program
  // TO DO, CHANGE THIS FILE
  std::string program_name;
  program_name = "/cis/home/dtward/Documents/OpenCL/OpenCL/opencl_array.cl";
  program_name = "/home/dtward/Documents/Coding/OpenCL/OpenCL/opencl_array.cl";
  program_name = "../OpenCL/opencl_array.cl"; // this will be better, but still needs to be fixed
  std::cout << "building program " << program_name << std::endl;
  std::ifstream program_file(program_name.c_str());
  if (!program_file.is_open()) throw std::runtime_error("could not open source file");
  std::string program_string((std::istreambuf_iterator<char>(program_file)), std::istreambuf_iterator<char>());
  
  cl::Program::Sources source(1, std::make_pair(program_string.c_str(), strlen(program_string.c_str())));
  program = cl::Program(context, source, &err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create OpenCL program");
  std::vector<cl::Device>devices_;
  devices_.push_back(device);
  err = program.build(devices);

  if (err != CL_SUCCESS) {
    std::string build_log;
    build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
    std::cout << build_log << std::endl;
    throw std::runtime_error("could not build OpenCL program");
  }

  // command queue
  // for properties could do CL_QUEUE_PROFILING_ENABLE, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
  queue = cl::CommandQueue(context, device,0,&err);
  
  // finish
  initialized = true;
}


void opencl_array::set_n(int n_in){
  n = n_in;
  offset = cl::NDRange(0);
  global = cl::NDRange(n);
  local = cl::NullRange;
}

// default constructor
opencl_array::opencl_array(void){
  setup_context();
  set_n(0);
}

void opencl_array::setup_buffer(void){
  // set up buffer
  int err;
  buffer = cl::Buffer(context,
		      CL_MEM_READ_WRITE,
		      n*sizeof(double),
		      NULL,
		      &err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create buffer");
}

const cl::Buffer & opencl_array::get_buffer(void) const {
  return buffer;
}

// size constructor, allcate but don't set
opencl_array::opencl_array(int m){
  setup_context();
  set_n(m);
  setup_buffer();
}


void opencl_array::set(const std::vector<double> & v, bool blocking) {
  set_n(v.size());
  int err;
  err = queue.enqueueWriteBuffer(buffer,
				 blocking, // blocking
				 0,	  // offset
				 v.size()*sizeof(double),
				 &v[0],
				 NULL, // event waitlist
				 NULL  // event
				 );
  if (err != CL_SUCCESS) throw std::runtime_error("could not set buffer");
}

// pointer constructor
opencl_array::opencl_array(int m, double const * const v){
  setup_context();
  set_n(m);
  setup_buffer();
  std::vector<double> vvec(v,v+m); // need to double check
  set(vvec);
}

// vector constructor
opencl_array::opencl_array(const std::vector<double> & v){
  setup_context();
  set_n(v.size());
  setup_buffer();
  set(v);
}

// constant constructor
opencl_array::opencl_array(int n, double a){
  setup_context();
  set_n(n);
  setup_buffer();
  set(std::vector<double>(n,a));
}

std::vector<double> opencl_array::get(bool blocking) const {
  std::vector<double> out(n);
  int err;
  err = queue.enqueueReadBuffer(buffer,
				blocking, // blocking
				0,	 //offset
				n*sizeof(double),
				&out[0],
				NULL, // event waitlist
				NULL  // event
				);
  if (err != CL_SUCCESS) throw std::runtime_error("could not get buffer");
  return out;
}

opencl_array & opencl_array::compound_assignment(const std::string & name, const opencl_array & b){
  int err;
  cl::Kernel kernel(program, name.c_str(), &err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create " + name + " kernel");
  err = kernel.setArg(0, buffer);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 0 for " + name);
  err = kernel.setArg(1, b.buffer);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 1 for " + name);
  err = kernel.setArg(2, n);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 2 for " + name);

  // enqueue
  err = queue.enqueueNDRangeKernel(kernel,
  				   offset,//offset
				   global,
				   local,
				   NULL, // events waitlist
				   NULL // event
  				   );
  if (err != CL_SUCCESS) throw std::runtime_error("could not enqueue ndrange kernel for " + name);
  return *this;
}

opencl_array & opencl_array::operator+=(const opencl_array & b){
  return compound_assignment("addition_assignment", b);
}
opencl_array &opencl_array::operator-=(const opencl_array & b){
  return compound_assignment("subtraction_assignment", b);
}
opencl_array &opencl_array::operator*=(const opencl_array & b){
  return compound_assignment("multiplication_assignment", b);
}
opencl_array &opencl_array::operator/=(const opencl_array & b){
  return compound_assignment("division_assignment", b);
}

// with scalars
opencl_array & opencl_array::compound_assignment(const std::string & name, double b){
  int err;
  cl::Kernel kernel(program, name.c_str(), &err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create " + name + " kernel");
  err = kernel.setArg(0, buffer);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 0 for " + name);
  err = kernel.setArg(1, b);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 1 for " + name);
  err = kernel.setArg(2, n);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 2 for " + name);

  // enqueue kernel operators
  err = queue.enqueueNDRangeKernel(kernel,
  				   offset,//offset
				   global,
				   local,
				   NULL, // events waitlist
				   NULL // event
  				   );
  if (err != CL_SUCCESS) throw std::runtime_error("could not enqueue ndrange kernel for " + name);
  return *this;
}
opencl_array & opencl_array::operator+=(double b){
  return compound_assignment("addition_assignment_scalar", b);
}
opencl_array &opencl_array::operator-=(double b){
  return compound_assignment("subtraction_assignment_scalar", b);
}
opencl_array &opencl_array::operator*=(double b){
  return compound_assignment("multiplication_assignment_scalar", b);
}
opencl_array &opencl_array::operator/=(double b){
  return compound_assignment("division_assignment_scalar", b);
}


// binary operators in terms of compound assignment
// we need to make a copy, so do it in the argument list
opencl_array operator+(opencl_array a, const opencl_array & b){
  a+=b;
  return a;
}
opencl_array operator-(opencl_array a, const opencl_array & b){
  a-=b;
  return a;
}
opencl_array operator*(opencl_array a, const opencl_array & b){
  a*=b;
  return a;
}
opencl_array operator/(opencl_array a, const opencl_array & b){
  a/=b;
  return a;
}

opencl_array operator+(opencl_array a, double b){
  a += b;
  return a;
}
opencl_array operator-(opencl_array a, double b){
  a -= b;
  return a;
}
opencl_array operator*(opencl_array a, double b){
  a *= b;
  return a;
}
opencl_array operator/(opencl_array a, double b){
  a /= b;
  return a;
}

opencl_array operator+(double a, opencl_array b){
  b += a;
  return b;
}
opencl_array operator-(double a, opencl_array b){
  b *= (-1.0);
  b += a;
  return b;
}
opencl_array operator*(double a, opencl_array b){
  b *= a;
  return b;
}
opencl_array operator/(double a, const opencl_array & b){
  opencl_array a_array(b.get_n(),a);
  a_array /= b;
  return a_array; // is there a better way?
}


// streams
std::ostream & operator<<(std::ostream & os, const opencl_array & a){
  std::vector<double> avec = a.get();
  int n = avec.size();
  os << "opencl_array(";
  if (n > 6){
    os << avec[0] << "," << avec[1] << "," << avec[2] << ",... [" << n << "total]...," << avec[n-3] << "," << avec[n-2] << "," << avec[n-1];
  } else {
    for (std::vector<double>::const_iterator it = avec.begin(); it != avec.end(); ++it) {
      os << *it;
      if (it != avec.end()-1) os << ",";
    }
  }
  os << ")";
  return os;
}



// open cl kernels
opencl_array apply_gaussian_kernel(const opencl_array & x, const opencl_array & p, const opencl_array & y, double sigma){
  // negative one over 2 sigma squared
  double noo2sigma2 = -1.0/(2.0*sigma*sigma);
  // initialize output variable
  opencl_array v(x.get_n()); // recall that this creates the buffer but doesn't send any data to it
  int err;
  std::string name = "apply_gaussian_kernel";
  cl::Kernel kernel(opencl_array::get_program(), name.c_str(),&err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create " + name + "kernel");
  err = kernel.setArg(0,x.get_buffer());
  err |= kernel.setArg(1,p.get_buffer());
  err |= kernel.setArg(2,x.get_n()/3);
  err |= kernel.setArg(3,3);
  err |= kernel.setArg(4,y.get_buffer());
  err |= kernel.setArg(5,v.get_buffer());
  err |= kernel.setArg(6,y.get_n()/3);
  err |= kernel.setArg(7,noo2sigma2);

  if (err != CL_SUCCESS) throw std::runtime_error("could not set args for " + name);


  // set up range
  // this is not the usual range
  cl::NDRange offset = cl::NDRange(0);
  cl::NDRange global = cl::NDRange(y.get_n()/3); // parallelize over output
  cl::NDRange local = cl::NullRange;

  // enqueue kernel operators
  err = opencl_array::get_queue().enqueueNDRangeKernel(kernel,
  				   offset,//offset
				   global,
				   local,
				   NULL, // events waitlist
				   NULL // event
  				   );
  if (err != CL_SUCCESS) throw std::runtime_error("could not enqueue ndrange kernel for " + name);


  
  // return
  return v;
}
