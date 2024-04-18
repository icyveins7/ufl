#include "matrix.h"
#include "upfirlerp.h"

#include "mex.h"


/* The gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    /* check for proper number of arguments */
    if(nrhs!=5) {
        mexErrMsgIdAndTxt(
            "MyToolbox:arrayProduct:nrhs",
            "5 inputs required: upsample rate, taps vec, input vec, input period, interpolated time vec.");
    }
    if(nlhs!=1) {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs","One output required.");
    }
    // Check upsample rate
    if ( mxGetNumberOfElements(prhs[0]) != 1 || mxIsComplex(prhs[0]))
    {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notScalar","upsample rate must be a real scalar.");
    }

    // Check taps vector, must be single precision
    if ( !mxIsSingle(prhs[1]) || mxIsComplex(prhs[1]))
    {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notSingle","taps vec must be real single-precision.");
    }

    // Check input vector, must be complex singles
    if ( !mxIsSingle(prhs[2]) || !mxIsComplex(prhs[2]) )
    {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notSingle","input vec must be complex single-precision.");
    }

    // Check input sample period, must be scalar double
    if ( !mxIsDouble(prhs[3]) || mxGetNumberOfElements(prhs[3])!= 1 || mxIsComplex(prhs[3]))
    {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notDouble","input sample period must be a real scalar double.");
    }

    // Check interpolated time vector, must be double precision
    if ( !mxIsDouble(prhs[4]) || mxIsComplex(prhs[4]))
    {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notDouble","interpolated time vec must be real double-precision.");
    }

    // If all okay, instantiate the class
    ufl::UpfirLerp<float> upfirlerp;
    upfirlerp.set_up_rate(
        static_cast<int>(mxGetScalar(prhs[0]))
    );
    upfirlerp.set_up_taps(
        reinterpret_cast<float*>(mxGetSingles(prhs[1])),
        static_cast<size_t>(mxGetNumberOfElements(prhs[1]))
    );


    /* create the output matrix */
    plhs[0] = mxCreateNumericMatrix(
        1, mxGetNumberOfElements(prhs[4]),
        mxSINGLE_CLASS, mxCOMPLEX
    );

    /* get a pointer to the real data in the output matrix */
    mxComplexSingle* out = mxGetComplexSingles(plhs[0]);

    /* call the computational routine */
    upfirlerp.interpolate_array(
        reinterpret_cast<const std::complex<float>*>(mxGetData(prhs[2])), 
        static_cast<size_t>(mxGetNumberOfElements(prhs[2])),
        static_cast<double>(mxGetScalar(prhs[3])),
        reinterpret_cast<const double*>(mxGetData(prhs[4])), 
        static_cast<size_t>(mxGetNumberOfElements(prhs[4])),
        reinterpret_cast<std::complex<float>*>(out)
    );
}

