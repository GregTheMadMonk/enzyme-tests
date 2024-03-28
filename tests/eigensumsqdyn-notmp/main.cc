/*
 * This file is a modified version of
 * https://github.com/EnzymeAD/Enzyme/blob/main/enzyme/test/Integration/ReverseMode/eigensumsqdyn-notmp.cpp
 */

#define EIGEN_NO_AUTOMATIC_RESIZING 1
#define EIGEN_DONT_ALIGN 1
#define EIGEN_NO_DEBUG 1
#define EIGEN_UNROLLING_LIMIT 0
#define EIGEN_DONT_VECTORIZE 1

// <---- test_utils.h ---->
#if defined(__cplusplus) || defined(__APPLE__)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#else
struct _IO_FILE;
extern struct _IO_FILE* stderr;
extern int fprintf(struct _IO_FILE *, const char*, ...);
extern int fflush(struct _IO_FILE *stream);
extern int printf(const char*, ...);
extern void abort();
extern void free(void *);
extern void* malloc(unsigned long);
extern void *realloc( void *ptr, unsigned long new_size );
extern void* memcpy( void* dest, const void* src, unsigned long count );
extern void* memset( void* dest, int, unsigned long count );
#endif

extern
#ifdef __cplusplus
"C"
#endif
int enzyme_allocated, enzyme_const, enzyme_dup, enzyme_dupnoneed, enzyme_out,
    enzyme_tape;

/*
#ifdef __cplusplus
extern "C" {
#endif
static inline bool approx_fp_equality_float(float f1, float f2, double threshold) {
  if (fabs(f1-f2) > threshold) return false;
  return true;
}

static inline bool approx_fp_equality_double(double f1, double f2, double threshold) {
  if (fabs(f1-f2) > threshold) return false;
  return true;
}
#ifdef __cplusplus
}
#endif
*/

#define APPROX_EQ(LHS, RHS, THRES)                                    \
    {                                                                \
      if (__builtin_fabs((LHS) - (RHS)) > THRES) {                                               \
        fprintf(stderr, "Assertion Failed: fabs( [%s = %g] - [%s = %g] ) > %g at %s:%d (%s)\n", #LHS, (double)(LHS), #RHS, (double)(RHS), THRES, \
                __FILE__, __LINE__, __PRETTY_FUNCTION__);               \
        abort();                                                        \
      }                                                                 \
    };
// END <---- test_utils.h ---->
#include <eigen3/Eigen/Dense>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

using Eigen::MatrixXd;
using Eigen::Matrix;
using Eigen::VectorXd;

constexpr size_t IN = 4, OUT = 4, NUM = 5;

extern "C" {
    extern double __enzyme_autodiff(void*, const MatrixXd* __restrict W, const MatrixXd* __restrict Wp, const MatrixXd* __restrict M, const MatrixXd* __restrict Mp);
}

MatrixXd square(const MatrixXd* M) {
    return *M * *M;
}

__attribute__((noinline))
static double matvec(const MatrixXd* __restrict W, const MatrixXd* __restrict M) {
  return ((*W - *M) * (*W - *M)).sum();
  /*
  MatrixXd diff = *W-*M;
  auto sq = square(&diff);
  return sq.sum();
  */
}

int main(int argc, char** argv) {

    //size_t IN = 40, OUT = 30, NUM = 50;
    MatrixXd W = Eigen::MatrixXd::Constant(IN, OUT, 1.0);
    MatrixXd M = Eigen::MatrixXd::Constant(IN, OUT, 2.0);
    
    MatrixXd Wp = Eigen::MatrixXd::Constant(IN, OUT, 0.0);
    MatrixXd Mp = Eigen::MatrixXd::Constant(IN, OUT, 0.0);
    
    __enzyme_autodiff((void*)matvec, &W, &Wp, &M, &Mp);
    

    for(int o=0; o<OUT; o++)
    for(int i=0; i<IN; i++) {
        APPROX_EQ( Wp(i, o), -8., 1e-10);
        fprintf(stderr, "Wp(o=%d, i=%d)=%f\n", i, o, Wp(i, o));
    }
     
    for(int o=0; o<OUT; o++)
    for(int i=0; i<IN; i++) {
        APPROX_EQ( Mp(i, o), 8., 1e-10);
        fprintf(stderr, "Mp(o=%d, i=%d)=%f\n", i, o, Mp(i, o));
    }
}
