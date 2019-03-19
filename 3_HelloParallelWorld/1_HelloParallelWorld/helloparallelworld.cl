	union myunion{
	float f;
	short s;
	char c;
	};

	struct mystruct{ 
	char ch;
	char4 ch4 __attribute__ ((packed)) ;
	union myunion u;
	uint2 ui2[4] __attribute__ ((packed)) ;
	}__attribute__ ((aligned (16)));

__kernel void helloparallelworld(__global const float3* X3,__global const float4* Y4,__global const float8* Z8, __global const struct mystruct* stVar )
{


	uint dims = get_work_dim();
	uint nsize = get_global_size(0);
	int gid0,gid1,iloc0,iloc1,nloc;
	if(dims ==1)
	{
		gid0 = get_global_id(0);
		printf("1Dkernel with globalID  %d",gid0);
	}

	if(dims ==2)
	{
		gid0 = get_global_id(0);
		gid1 = get_global_id(1);
		printf("1. 2Dkernel with globalID of x is  %d and y is %d\n\n",gid0,gid1);
	}	
	iloc0 = get_local_id(0);
	iloc1 = get_local_id(1);
	nloc = get_local_size(0);
	printf("2. LocalWorkgroup Size:%d LocalWorkItemID0 = %d LocalWorkItemID1 = %d \n\n",nloc,iloc0,iloc1);
	



	
	printf("3. Contents of float4- Y4 : %f\n",Y4[nsize*gid0+gid1].xyzw);
	printf("4. Reversed Content of Y4 : %f\n",Y4[nsize*gid0+gid1].wzyx);

	printf("5. Swizzled data of float8variable\n");
	printf("High part of float8variable %f \n Low part of float8variable %f \n\n",Z8[nsize*gid1+gid0].hi.xyzw,Z8[nsize*gid1+gid0].lo.xyzw);
	printf("Even part of float8variable %f\n Odd part of float8variable %f\n\n",Z8[nsize*gid1+gid0].even,Z8[nsize*gid1+gid0].odd);

	printf("6. Contents of Struct \n");
	printf("stVar->ch : %c \n",stVar->ch);
	
	printf("stVar->u.f : %2.2f \n",stVar->u.f);
	printf("stVar->u.s : %hd \n",(short)stVar->u.s);
	printf("stVar->u.c : %c \n",stVar->u.c);
	
	printf("stVar->ch4 : %c \n",stVar->ch4);
	for(int i =0;i<4;i++){
	printf("stVar->ui2[4] : %d,%d \n",stVar[nsize*gid1+gid0].ui2[i].x ,stVar[nsize*gid1+gid0].ui2[i].y);
	}
		 
	printf("7. Size of structure %d\n",sizeof(stVar[gid0]));
	printf("8. Size of sum of all individual elements of the structure   : %d\n",sizeof(stVar->ch)+sizeof(stVar->ch4)+sizeof(stVar->u)+sizeof(stVar->ui2));

	printf("9. Size of structure union  U :%d\n",sizeof(stVar->u));
	printf("10. Size of structure union element U.c : %d\n",sizeof(stVar->u.c));
	
	
	
}