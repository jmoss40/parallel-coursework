// Note: this file was originally serial code provided by my instructor, and I was
// the one who made it parallel using OpenMP.

/* File:    trap.c
 * Purpose: Calculate definite integral using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -o trap_omp_parallelfor trap_omp_parallelfor.c -fopenmp
 * Usage:   ./trap_omp_parallelfor {thread_count}
 *
 * Note:    The function f(x) is hardwired.
 *
 * IPP:     Section 3.2.1 (pp. 94 and ff.) and 5.2 (p. 216)
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int thread_count;
double f(double x);    /* Function we're integrating */
double Trap(double a, double b, int n, double h);

int main(int argc, char* argv[]) {
    double  integral;   /* Store result in integral   */
    double  a, b;       /* Left and right endpoints   */
    int     n;          /* Number of trapezoids       */
    double  h;          /* Height of trapezoids       */

    printf("Enter a, b, and n\n");
    scanf("%lf", &a);
    scanf("%lf", &b);
    scanf("%d", &n);

    thread_count = strtol(argv[1], NULL, 10);

    h = (b-a)/n;
    integral = Trap(a, b, n, h);
   
    printf("With n = %d trapezoids, our estimate ", n);
    printf("of the integral from %.2f to %.2f = %.2f\n",
      a, b, integral);

    return 0;
} /* main */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids
 * Input args:  a, b, n, h
 * Return val:  Estimate of the integral 
 */
double Trap(double a, double b, int n, double h) {
    double integral;
    int k;
    
    integral = (f(a) + f(b))/2.0;
    # pragma omp parallel for num_threads(thread_count) \
	reduction(+: integral)
    for (k = 1; k <= n-1; k++)
	integral += f(a+k*h);

    integral = integral*h;

    return integral;
} /* Trap */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
    double return_val;

    return_val = x*x;
    return return_val;
} /* f */
