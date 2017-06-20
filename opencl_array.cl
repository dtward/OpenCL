
void __kernel unary_plus(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] += b[i];
}

void __kernel unary_minus(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] -= b[i];
}

void __kernel unary_times(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] *= b[i];
}

void __kernel unary_divide(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] /= b[i];
}




  
