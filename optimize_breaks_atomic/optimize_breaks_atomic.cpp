#include <cppad/cppad.hpp>
#include "atomic_toy_class.h"

// This file uses the toy atomic in a tape.
// The atomic gives y = f(x), where both y and x have length 2.
// After calling the atomic, the dependent variable of the tape is
// simply y[1]*y[1].
// Hence, y[0] is ultimately ignored and in more specialized code
// could be omitted.  But an atomic may be used in general cases
// and should not break even when used trivially.
//
// In the example below, optimizing the tape breaks the atomic because
// taylor_x[0] is not put in place for use by reverse.
// Mathematically it is correct that taylor_x[0] should not be needed,
// but in the code this propagates a nan.

using CppAD::AD;
using CppAD::ADFun;
using std::cout;
using std::endl;

void record_tape(double x0, double x1, ADFun<double> &adtape, bool optimize) {
  // This records the operation:
  // y = toy(x)
  // z = y[1] * y[1]
  // z is the dependent variable of the tape
  // n = 2
  // m = 1
  std::vector<AD<double> > indVars(2);
  std::vector<AD<double> > atomOut(2);
  std::vector<AD<double> > depVars(1);

  static atomic_toy_class atomic_toy("atomic toy");
  indVars[0] = x0;
  indVars[1] = x1;
  CppAD::Independent(indVars);
  atomic_toy(indVars, atomOut);
  depVars[0] = atomOut[1] * atomOut[1];
  adtape.Dependent(indVars, depVars);
  if(optimize) adtape.optimize(); // If not optimized, it works correctly
}

void AD_toy_test(double x0_rec,
		 double x1_rec,
		 double x0_test,
		 double x1_test,
		 std::vector<double> &y,
		 std::vector<double> &dy_dx,
		 bool optimize) {
  ADFun<double> adtape;
  record_tape(x0_rec, x1_rec, adtape, optimize);
  std::vector<double> newInd(2);
  newInd[0] = x0_test;
  newInd[1] = x1_test;
  y = adtape.Forward(0, newInd);
  std::vector<double> w(1,1);
  dy_dx = adtape.Reverse(1, w);
}

void correct_ans(double x0,
		 double x1,
		 std::vector<double> &y,
		 std::vector<double> &dy_dx) {
  y.resize(1);
  dy_dx.resize(2);
  y[0] = exp(x1) * exp(x1);
  dy_dx[0] = 0;
  dy_dx[1] = 2 * exp(x1) * exp(x1);
}

void show(std::vector<double> &y,
	  std::vector<double> &dy_dx) {
  cout<<"(y[0]) = "<<y[0]<<endl;
  cout<<"(dy[0] / dx[0], dy[0] / dx[1])  = ("<<dy_dx[0]<<", "<<dy_dx[1]<<")"<<endl;
  cout<<endl;
}

int main(void) {
  double x0_rec = 1.2;
  double x1_rec = 2.3;
  double x0_test = 3.4;
  double x1_test = 4.5;
  std::vector<double> y, dy_dx;

  printf("Correct:\n");
  correct_ans(x0_test, x1_test, y, dy_dx);
  show(y, dy_dx);

  printf("CppAD without optimizing (gives correct answer):\n");
  AD_toy_test(x0_rec, x1_rec, x0_test, x1_test, y, dy_dx, false);
  printf("ANSWER:\n");
  show(y, dy_dx);

  printf("CppAD with optimizing (abort from error trap happens in this case, wrong answers without abort in less trivial cases:\n");
  AD_toy_test(x0_rec, x1_rec, x0_test, x1_test, y, dy_dx, true);
  show(y, dy_dx);
}
