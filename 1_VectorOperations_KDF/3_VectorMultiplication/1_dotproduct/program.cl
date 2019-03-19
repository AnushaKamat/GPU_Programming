__kernel void vecdotprod(__global const int4* input1,__global const int4* input2,__global int* product)
{
	int gid0 = get_global_id(0);
	int4 A,B;
	A.xyzw = input1[gid0]; //Accessing i, j , k components of vectors
	B.xyzw = input2[gid0];
	product[gid0] = (A.x*B.x+A.y*B.y+A.z*B.z); //Dot product of 2 vectors
}