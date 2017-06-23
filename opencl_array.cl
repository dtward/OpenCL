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
void __kernel apply_gaussian_kernel(__global double * x, __global double * p, __const int np, __const int d, __global double * y, __global double * v, __const int nv, __const double noo2sigma2){
  // here n is the number of points, d is the dimension
  // parallelize over output
  // note maybe I should say add gaussian kernel, and not initialize to zero
  int i = get_global_id(0);
  if (i > nv) return;
  int di = d*i;
  for (int j = 0; j < d; ++j) v[di + j] = 0.0;
  double3 yi = (double3)(y[di],y[di+1],y[di+2]);
  double3 xj;
  int dj;
  double d2;
  // sum over xs
  for (int j = 0; j < np; ++j){
    // get displacement
    dj = d*j;
    xj = (double3)(x[dj],x[dj+1],x[dj+2]);
    // get distance squared
    d2 = length(yi-xj);
    d2 *= d2;
    d2 *= noo2sigma2;
    for (int k = 0; k < d; ++k){
      v[di + k] += exp(d2)*p[dj + k];
    }
  }
  
}
