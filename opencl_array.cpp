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

void opencl_array::setup_context(void){
  std::cout << "in setup_context" << std::endl;
  if (initialized){
    std::cout << "context already initialized" << std::endl;
    return;
  }

  // query platforms
  int err;
  std::vector<cl::Platform> platforms;
  err = cl::Platform::get(&platforms);
  if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL platforms");
  std::cout << "listing platforms" << std::endl;
  for (std::vector<cl::Platform>::iterator it = platforms.begin(); it != platforms.end(); ++it){
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
  for (std::vector<cl::Device>::iterator it = devices.begin(); it != devices.end(); ++it){
    std::cout << "Device: " << it->getInfo<CL_DEVICE_NAME>(&err) << std::endl;
    if (err != CL_SUCCESS) throw std::runtime_error("could not get OpenCL device name");
  }
  device = devices.at(device_id);
  std::cout << "selecting device " << device_id << ", " << device.getInfo<CL_DEVICE_NAME>(&err) << std::endl;


  // make a program
  // TO DO, CHANGE THIS FILE
  std::string program_name = "/cis/home/dtward/Documents/OpenCL/OpenCL/opencl_array.cl";
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




opencl_array::opencl_array(void){
  setup_context();
  n = 0;
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

opencl_array::opencl_array(int m){
  setup_context();
  n = m;
  setup_buffer();
}


void opencl_array::set(const std::vector<double> & v) {
  n = v.size();
  int err;
  err = queue.enqueueWriteBuffer(buffer,
				 CL_TRUE, // blocking
				 0,	  // offset
				 v.size()*sizeof(double),
				 &v[0],
				 NULL, // event waitlist
				 NULL  // event
				 );
  if (err != CL_SUCCESS) throw std::runtime_error("could not set buffer");
}

opencl_array::opencl_array(int m, double const * const v){
  setup_context();
  n = m;
  setup_buffer();
  std::vector<double> vvec(v,v+m); // need to double check
  set(vvec);
}

opencl_array::opencl_array(const std::vector<double> & v){
  setup_context();
  n = v.size();
  setup_buffer();
  set(v);
}

std::vector<double> opencl_array::get(void) const {
  std::vector<double> out(n);
  int err;
  err = queue.enqueueReadBuffer(buffer,
				CL_TRUE, // blocking
				0,	 //offset
				n*sizeof(double),
				&out[0],
				NULL, // event waitlist
				NULL  // event
				);
  if (err != CL_SUCCESS) throw std::runtime_error("could not get buffer");
  return out;
}

void opencl_array::operator+=(const opencl_array & b){
  int err;
  cl::Kernel kernel(program, "unary_plus", &err);
  if (err != CL_SUCCESS) throw std::runtime_error("could not create unary plus kernel");
  err = kernel.setArg(0, buffer);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 0 for unary plus");
  err = kernel.setArg(1, b.buffer);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 1 for unary plus");
  err = kernel.setArg(2, n);
  if (err != CL_SUCCESS) throw std::runtime_error("could not set argument 2 for unary plus");

  // setup range
  // TO DO, something better
  cl::NDRange offset;
  cl::NDRange local;
  cl::NDRange global;

  // enqueue
  //err = queue.enqueueNDRangeKernel(kernel,
  //				   0,//offset
  //				   );
  if (err != CL_SUCCESS) throw std::runtime_error("could not enqueue ndrange kernel for unary plus");
}

