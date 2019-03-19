__kernel void vecadd(__global const float *a,__global const float *b,__global float *c)
{
	uint dims = get_work_dim();	//Get Dimensions of Input to find if its 1D,2D or 3D
	int index,gid0,gid1,gid2,Nx,Ny;
	/*Calculating indices in different scenarios*/
	if(dims ==1)
	{
		index = get_global_id(0);
	}

	else if(dims ==2)
	{
		gid0 = get_global_id(0);
		gid1 = get_global_id(1);
		Nx = get_global_size(0);
		index = Nx*gid1 +gid0;	
	}
	
	else 
	{
		gid0 = get_global_id(0);
		gid1 = get_global_id(1);
		gid2 = get_global_id(2);
		Nx = get_global_size(0);
		Ny = get_global_size(1);
		index = Nx*Ny*gid2+Nx*gid1 +gid0;
	}
	
	c[index] = a[index] + b[index];
}