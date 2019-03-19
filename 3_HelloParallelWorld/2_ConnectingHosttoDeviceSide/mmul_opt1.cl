__kernel void mmul_opt1(const int N,__global const float *A,__global const float *B, __global float *C)
{
	int i,j,k;
	i= get_global_id(0);
	j = get_global_id(1);
	float tmp =0.0f;
	for(k = 0;k<N;k++){
		tmp+=A[i*N+k]*B[k*N+j];
	}
	C[i*N+j]+=tmp;
}

