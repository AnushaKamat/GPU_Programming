// Add you host code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CL/cl.h"
#include "read_source.h"
#include<malloc.h>
#define Size 25

//Union of float , short and char
union myunion {
	cl_float f;
	cl_short s;
	cl_char c;
};

//My structure aligned to 128
__declspec(align(16)) struct mystruct {
	cl_char ch;
	cl_char4 ch4;
	union myunion u;
	cl_uint2 ui2[4];
};



int main(int argc, char** argv) {
	unsigned int i, j;

	char cBuffer[1024];
	cl_uint platformCount, deviceCount;
	cl_platform_id* platformIDs;
	cl_device_id *deviceIDs;
	cl_int err;
	cl_platform_id selectedPlatformID = NULL;
	cl_device_id selectedDeviceID = NULL;
	size_t global_work_size[3] = { 5,5,0 }, local_work_size[3] = { 5,5,0 };
	struct mystruct stVar[Size];
	


	//Step1: Query for Platform
	err = clGetPlatformIDs(0, NULL, &platformCount);
	if (err != CL_SUCCESS) {
		printf(" Error %d in clGetPlatformIDs Call!\n\n", i, err);
	}
	else {
		printf(" clGetPlatformIDs works\n");
	}

	printf("%d OpenCL Platforms found\n\n", platformCount);

	platformIDs = (cl_platform_id*)malloc(platformCount * sizeof(cl_platform_id));

	// get platform info for each platform
	err = clGetPlatformIDs(platformCount, platformIDs, NULL);
	if (err != CL_SUCCESS) {
		printf(" Error in clGetPlatformIDs Call!\n\n", err);
	}
	else {
		printf(" clGetPlatformIDs works\n");
	}

	for (i = 0; i < platformCount; i++) {
		err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, 1024, &cBuffer, NULL);
		if (err != CL_SUCCESS) {
			printf(" Error in clGetPlatformInfo Call!\n\n", err);
		}
		else {
			printf(" clGetPlatformInfo works\n");
		}

		err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, sizeof(cBuffer), cBuffer, NULL);
		if (err != CL_SUCCESS) {
			printf(" Error in clGetPlatformInfo Call!\n\n", err);
		}
		else {
			printf(" clGetPlatformInfo works\n");
		}

		printf(" CL_PLATFORM_NAME: %d \t%s\n", i, cBuffer);
		//Selecting the Platform
		if (strcmp(cBuffer, "Intel(R) OpenCL") == 0) {
			selectedPlatformID = platformIDs[i];
			//printf(" selected CL_PLATFORM_NAME: %d \t%s\n", i, cBuffer);
		}


	}


	//Query for Devices
	printf("\n OpenCL Devices:\n");
	for (i = 0; i < platformCount; i++) {
		err = clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
		if (err != CL_SUCCESS) {
			printf(" Error  in clGetDeviceIDs Call!\n\n", err);
		}
		else {
			printf(" clGetDeviceIDs works\n");
		}
	}

	printf(" %u deviceIDs is found on: %s\n", deviceCount, cBuffer);

	for (i = 0; i < deviceCount; i++) {
		deviceIDs = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
		err = clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL, deviceCount, deviceIDs, &deviceCount);
		if (err != CL_SUCCESS) {
			printf(" Error %i in clGetDeviceIDs Call!\n\n", err);
		}
		else {
			printf(" clGetDeviceIDs works\n");
		}
	}

	for (i = 0; i < deviceCount; i++) {
		clGetDeviceInfo(deviceIDs[i], CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
		printf(" CL_DEVICE_NAME :  %s\n\n\n", cBuffer);
	}


	//Selecting platform and device


	// Get OpenCL platform name and version
	err = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_NAME, sizeof(cBuffer), cBuffer, NULL);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clGetPlatformIDs Call!\n\n", err);
	}
	else {
		printf(" clGetPlatformInfo works\n");
	}
	printf(" Selected Platform: %d \t%s\n", i, cBuffer);



	err = clGetDeviceIDs(selectedPlatformID, CL_DEVICE_TYPE_GPU, 1, deviceIDs, &deviceCount);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clGetDeviceIDs Call!\n\n", err);
	}
	else {
		printf(" clGetDeviceIDs works\n");
	}

	selectedDeviceID = *deviceIDs;
	clGetDeviceInfo(selectedDeviceID, CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
	printf(" Selected Device: %d \t%s\n\n", i, cBuffer);


	//Create Context 

	cl_context context = clCreateContext(NULL, 1, &selectedDeviceID, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateContext Call!\n\n", err);
	}
	else {
		printf(" clCreateContext works\n");
	}


	size_t file_size = 0;
	char * kernel_source = read_source("helloparallelworld.cl", &file_size);


	printf("reading works");

	//Program Object 
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, &file_size, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateProgramWithSource Call!\n\n", err);
	}
	else {
		printf(" clCreateProgramWithSource works\n");
	}


	//Building executables
	err = clBuildProgram(program, 1, &selectedDeviceID, "–cl-std=CL2.0", NULL, NULL);
	if (err != CL_SUCCESS) {
		printf(" Error %i in BuildProgram  Call!\n\n", err);
	}
	else {
		printf(" clBuildProgram works\n");
	}

	//Create Command Queue

	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, selectedDeviceID, 0, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateCommandQueueWithProperties  Call!\n\n", err);
	}
	else {
		printf(" clCreateCommandQueueWithProperties works\n");
	}

	//Create Kernel Objects
	cl_kernel createdkernel = clCreateKernel(program, "helloparallelworld", &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateKernel  Call!\n\n", err);
	}
	else {
		printf(" clCreateKernel works\n");
	}
	
	//Creating Float3, float4 and float8
	cl_float3 X3[Size];
	cl_float4 Y4[Size];
	cl_float8 Z8[Size];
	
	//Initialising the data of all arguments of kernel
	for (int i = 0; i < Size; i++) {
		X3[i] = { 1.0f,2.0f,3.0f };
		Y4[i] = { 4.0f,5.0f,6.0f,7.0f };
		Z8[i] = { 14.0f, 15.0f, 16.0f, 17.0f, 24.0f, 25.0f, 26.0f, 27.0f };
		stVar[i].ch = 'p';
		stVar[i].ch4 = { 'a', 'b', 'c', 'd' };
		stVar[i].u.f = 55.0f;
		stVar[i].u.s = (cl_short)122;
		stVar[i].u.c = 'z';
		for (int j = 0; j < 4; j++) {
			stVar[i].ui2[j] = {56,57};
		}
	}
	//Creating Buffers
	cl_mem bufX3 = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3)*Size, &X3, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}

	cl_mem bufY4 = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float4)*Size, &Y4, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}

	cl_mem bufZ8 = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float8)*Size, &Z8, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}

	cl_mem bufstVar = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(mystruct)*Size, stVar, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}
	//Set Kernel Arguments
	clSetKernelArg(createdkernel, 0, sizeof(bufX3), &bufX3);
	clSetKernelArg(createdkernel, 1, sizeof(bufY4), &bufY4);
	clSetKernelArg(createdkernel, 2, sizeof(bufZ8), &bufZ8);
	clSetKernelArg(createdkernel, 3, sizeof(bufstVar), &bufstVar);
	printf(" Argument Setting works\n");

	//Execute kernel
	
	err = clEnqueueNDRangeKernel(command_queue, createdkernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf(" Error %s in Enqueue ND range Kernel  Call!\n\n", err);
	}
	else {
		printf(" clEnqueueNDRangeKernel works\n");
	}

	err = clFinish(command_queue);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clFinish  Call!\n\n", err);
	}
	else {
		printf(" clFinish works\n");
	}
		
}