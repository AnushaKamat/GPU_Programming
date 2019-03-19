__kernel void veccrossprod(__global const int4* input1,__global const int4* input2,__global int4* product)
{
	int gid0 = get_global_id(0);
	int4 A,B;
	A.xyzw = input1[gid0];//Accessing i, j , k components of vectors
	B.xyzw = input2[gid0];

	/*Calculating i,j,k components of cross product*/
	product[gid0] = (int4)((A.y*B.z - A.z*B.y),(A.z*B.x - A.x*B.z),(A.x*B.y - A.y*B.x),0);
}