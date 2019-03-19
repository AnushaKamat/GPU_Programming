__kernel void SAXPY(const float A,__global const float *X,__global const float *Y,__global float *SAXPY)
{
	uint dims = get_work_dim();
	int index,gid0,gid1,gid2,Nx,Ny;

	/*Calculating index for 1D or 2D case*/
	if(dims ==1)
	{
		index = get_global_id(0);
	}

		if(dims ==2)
	{
		gid0 = get_global_id(0);
		gid1 = get_global_id(1);
		Nx = get_global_size(0);
		index = Nx*gid1 +gid0;
	}
	
	/*SAXPY calculation*/
	SAXPY[index] = A*X[index] + Y[index]; // A is set to a value of 2.5
}