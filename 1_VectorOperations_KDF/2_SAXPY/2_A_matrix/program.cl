/* In this SAXPY calculation, A is a M*M matrix , X and Y are M*N matrix*/
/* For ease of computation M=N=2, all matrices are square*/

__kernel void matrixmulSAXPY(const int M, const int N,__global const float *A,__global const float *X,__global const float *Y,__global float *SAXPY)
{
	int gid0 = get_global_id(0);
	int gid1 = get_global_id(1);

	/*Matrix Multiplication : 
	Accessing first matrix A as Row Major
	Accessing second matrix X as Column major
	and performing this actions over rows of A(i.e M number of times)*/

	float AX_val =0.0f;
	for(int i = 0; i <M;i++)
	{
		AX_val += A[gid1*M+i]*X[i*M+gid0]; 
	}
	SAXPY[gid1*M+gid0] = AX_val;
	
	SAXPY[gid0] += Y[gid0];
}