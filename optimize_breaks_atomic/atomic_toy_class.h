#ifndef _ATOMIC_TOY
#define _ATOMIC_TOY

/*
This defines a derived atomic_three class for the function from R^2 --> R^2:
Y[0] = exp(X[0]) + exp(X[1])
Y[1] = exp(X[1])
This is a toy for debugging.  Correctness of math is not important.
Only forward 0 and reverse 1 are needed.
Only Base type of double is needed.
*/


#include <cppad/cppad.hpp>
#include <cppad/utility/nan.hpp>

using std::cout;
using std::endl;

class atomic_toy_class :  public CppAD::atomic_three< double > {
 public:
 atomic_toy_class(const std::string& name) : CppAD::atomic_three<double>(name) {};
 private:

  virtual bool for_type(
			const CppAD::vector<double>&               parameter_x ,
			const CppAD::vector<CppAD::ad_type_enum>&  type_x      ,
			CppAD::vector<CppAD::ad_type_enum>&        type_y      )
  {
    type_y[0] = type_x[0] > type_x[1] ? type_x[0] : type_x[1];
    type_y[1] = type_x[1];
    return true;
  }
  virtual bool rev_depend(
			  const CppAD::vector<double>&          parameter_x ,
			  const CppAD::vector<CppAD::ad_type_enum>&  type_x      ,
			  CppAD::vector<bool>&                depend_x    ,
			  const CppAD::vector<bool>&          depend_y
			  ) {
    depend_x[0] = depend_y[0];
    depend_x[1] = depend_y[0] || depend_y[1];
    return true;
  }

  virtual bool forward(
		       const CppAD::vector<double>&               parameter_x  ,
		       const CppAD::vector<CppAD::ad_type_enum>&  type_x       ,
		       size_t                              need_y       ,
		       size_t                              order_low    ,
		       size_t                              order_up     ,
		       const CppAD::vector<double>&               taylor_x     ,
		       CppAD::vector<double>&                     taylor_y     ) {
    int nrow = order_up + 1;
    int i;
    if(order_low <= 0 & order_up >= 0) { // value
      // 0s are in place to remind that the format is i + j*nrow
      taylor_y[0 + 0*nrow] = exp(taylor_x[0 + 0*nrow]) + exp(taylor_x[0 + 1*nrow]);
      taylor_y[0 + 1*nrow] = exp(taylor_x[0 + 1*nrow]);
    }
    if(order_up >= 1) {
      printf("not supported in this toy\n");
      return false;
    }
    return true;
  }
  
  virtual bool reverse(
		       const CppAD::vector<double>&               parameter_x ,
		       const CppAD::vector<CppAD::ad_type_enum>&  type_x      ,
		       size_t                              order_up    ,
		       const CppAD::vector<double>&               taylor_x    ,
		       const CppAD::vector<double>&               taylor_y    ,
		       CppAD::vector<double>&                     partial_x   ,
		       const CppAD::vector<double>&               partial_y   )
  {
    //reverse mode
    cout<<"In reverse"<<endl;
    int nrow = order_up + 1;
    if(order_up >= 0) {
      // obtain exp(x0) and exp(x1) from 0th order forward result.
      double exp_x0 = taylor_y[0 + 0*nrow] - taylor_y[0 + 1*nrow];
      double exp_x1 = taylor_y[0 + 1*nrow];
      cout<<"taylor_y[0] = "<<taylor_y[0 + 0*nrow]<<" taylor_y[1] = "<<taylor_y[0 + 1*nrow]<<endl;
      cout<<"taylor_x[0] = "<<taylor_x[0 + 0*nrow]<<" taylor_x[1]1 = "<<taylor_x[0 + 1*nrow]<<endl;
      cout<<"type_x[0] = "<<type_x[0 + 0*nrow]<<" type_x[1] = "<<type_x[0 + 1*nrow]<<endl;
      cout<<"parameter_x[0] = "<<parameter_x[0 + 0*nrow]<<" parameter_x[1] = "<<parameter_x[0 + 1*nrow]<<endl;
      cout<<"partial_y[0] = "<<partial_y[0 + 0*nrow]<<" partial_y[1] = "<<partial_y[0 + 1*nrow]<<endl;
      // adjoint of x[0] = (adjoint of y[0]) * exp(x[0])
      partial_x[0 + 0*nrow] = partial_y[0 + 0*nrow] * exp_x0;
      // adjoint of x[1] = (adjoint of y[0] + adjoing of y[1]) * exp(x[1])
      partial_x[0 + 1*nrow] = (partial_y[0 + 0*nrow] + partial_y[0 + 1*nrow]) * exp_x1;
    }
    if(order_up >= 1) {
      printf("not supported in this toy\n");
      return false;
    }
    return true;
  }
};
#endif
