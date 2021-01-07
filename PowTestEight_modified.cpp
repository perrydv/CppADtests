bool PowTestEight(void)
{   bool ok = true;

    using std::cout;
    using CppAD::AD;
    using CppAD::vector;
    using CppAD::NearEqual;
    double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
    //
    vector< double>      x(2), y(1), dx(2), dy(1), w(1), dw(1);
    vector< AD<double> > ax(2), ay(1);
    //
    x[0]  = -1.5; // works if x[0] > 0
    ax[0] = x[0];
    x[1] = 2.0;
    ax[1] = x[1];
    //
    CppAD::Independent(ax);
    ay[0] = pow(ax[0],  ax[1]); // variable-variable case
    CppAD::ADFun<double> f(ax, ay);
    f.check_for_nan(true);
    //
    double check;
    y     = f.Forward(0, x);
    cout<<y[0]<<"\n";
    check = pow(x[0], x[1]);
    ok   &= NearEqual(y[0], check, eps99, eps99);
    //
    dx[0] = 1.0;
    dx[1] = 0.0;
    dy    = f.Forward(1, dx); // execution aborts here
    check = x[1] * pow(x[0], x[1]-1);
    cout<<dy[0]<<"\n";
    ok   &= NearEqual(dy[0], check, eps99, eps99);

    return ok;
}
