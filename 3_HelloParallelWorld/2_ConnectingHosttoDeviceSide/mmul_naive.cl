__kernel void mmul_naive(const int N,__global const float *A,__global const float *B, __global float *C)
{
	int i,j,k;
	i= get_global_id(0);
	j = get_global_id(1);

	for(k = 0;k<N;k++){
		C[i*N+j]+=A[i*N+k]*B[k*N+j];
	}
}

