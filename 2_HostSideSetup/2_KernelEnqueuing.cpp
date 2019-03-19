// Add you host code

// Add you host code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CL/cl.h"
#include "read_source.h"
#include<malloc.h>
#define Size 8192
bool verifyResults(float* bufIN, float* bufOUT, const int size_dim) {
	bool flag = 0;
	float outputcheck[Size];
	for (unsigned int i = 0; i < size_dim; i++) {
		outputcheck[i]= *bufIN * 2;
		if (*bufOUT== outputcheck[i]) {
			flag = 1;
			bufOUT++;
		}
		else {
			printf("Verification failed\n");
			printf("Element with difference %d with values %f and %f\n", i, outputcheck[i], *bufOUT);
			return CL_FALSE;
		}
		bufIN++;
	}
	if (flag == 1) {
		printf("Verification succeeded");
	}
	return CL_TRUE;
}

int main(int argc, char** argv) {
	unsigned int i, j;
	
	char cBuffer[1024];
	cl_uint platformCount, deviceCount;
	cl_platform_id* platformIDs;
	cl_device_id *deviceIDs;
	cl_int err;
	cl_platform_id selectedPlatformID = NULL;
	cl_device_id selectedDeviceID = NULL;
	size_t global_work_size[3] = { Size,0,0 }, local_work_size[3] = { 16,0,0 };



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
	char * kernel_source = read_source("vecadd_anyD.cl", &file_size);


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
	cl_kernel createdkernel = clCreateKernel(program, "vecadd_anyD", &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateKernel  Call!\n\n", err);
	}
	else {
		printf(" clCreateKernel works\n");
	}
	
	//int ssize = 8192;
	//Creating Buffer Objects
	float *a = (float *)_aligned_malloc(sizeof(float)*Size, 4096);
	float *c = (float *)_aligned_malloc(sizeof(float)*Size, 4096);

	for (i = 0; i < Size; i++) {
		a[i] = i + 1;
	}
	
	cl_mem bufIN = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size, a, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}
	
	cl_mem bufOUT = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size, c, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in Create Buffer  Call!\n\n", err);
	}
	else {
		printf(" clCreateBuffer works\n");
	}
	

	//Set Kernel Arguments
	clSetKernelArg(createdkernel, 0, sizeof(bufIN), &bufIN);
	clSetKernelArg(createdkernel, 1, sizeof(bufIN), &bufIN);
	clSetKernelArg(createdkernel, 2, sizeof(bufOUT), &bufOUT);
	printf(" Argument Setting works\n");
	
	//Execute kernel
	printf("");
	err = clEnqueueNDRangeKernel(command_queue, createdkernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
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
	
	//read results back to Host
	float * inp = NULL;
	inp = (float *)clEnqueueMapBuffer(command_queue, bufOUT, CL_FALSE, CL_MAP_READ, 0, sizeof(bufOUT), 0, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in EnqueueMapBuffer  Call!\n\n", err);
	}
	else {
		printf(" clEnqueueMapBuffer works\n");
	}
	//Verifying the Results 
	bool errcond = verifyResults(a,inp,Size);
	if (errcond == CL_TRUE) {
		printf(" Worked!\n\n", err);
	}
	else {
		printf(" Not  Worked!\n\n", err);
	}
	
	err = clEnqueueUnmapMemObject(command_queue,bufOUT, inp, NULL,NULL,NULL);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clEnqueueUnmapMemObject  Call!\n\n", err);
	}
	else {
		printf(" clEnqueueUnmapMemObject works\n");
	}
	_aligned_free(a);
	_aligned_free(c);
}




