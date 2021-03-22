#ifndef SPLINE_H_
#define SPLINE_H_

#define SPLINE_NODES_MIN 3
#define SPLINE_NODES_MAX 20


//float get_spline(float xi, float *X, float *A, uint8_t A_Length);
void spline_poly_get_coeffs(void);
float spline_poly_eval(float xi);

struct _splEE
{
    float X[SPLINE_NODES_MAX];//x
    float A[SPLINE_NODES_MAX];//f(x)
    float B[SPLINE_NODES_MAX];
    float C[SPLINE_NODES_MAX];
    float D[SPLINE_NODES_MAX];
    float Lmin;
    float Lmax;//limits
    int8_t n;//max num node
};


#endif // SPLINE_H_


