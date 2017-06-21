// basic functions
void __kernel addition_assignment(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] += b[i];
}

void __kernel subtraction_assignment(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] -= b[i];
}

void __kernel multiplication_assignment(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] *= b[i];
}

void __kernel division_assignment(__global double * a, __global double * b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] /= b[i];
}
// scalar functions
void __kernel addition_assignment_scalar(__global double * a, __const double b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] += b;
}
void __kernel subtraction_assignment_scalar(__global double * a, __const double b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] -= b;
}

void __kernel multiplication_assignment_scalar(__global double * a, __const double b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] *= b;
}

void __kernel division_assignment_scalar(__global double * a, __const double b, __const int n){
  int i = get_global_id(0);
  if (i > n) return;
  a[i] /= b;
}

// some functions associated with lddmm

