// Add you host code
// Add you host code
// Add you host code
// Add you host code
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"

int main(int argc, char** argv) {
	int i, j;
	char cBuffer[1024];
	cl_uint platformCount, deviceCount;
	cl_uint readBuffer;
	size_t rBufWorkDim, rBufWorkSize[3];
	cl_platform_id* platformIDs;
	cl_device_id *devicesIDs;
	cl_int err;

	//Query for Platform
	err = clGetPlatformIDs(0, NULL, &platformCount);
	if (err != CL_SUCCESS) {
		printf(" Error %d in clGetPlatformIDs Call!\n\n", i, err);
	}
	else {
		printf("clGetPlatformIDs works\n");
	}

	printf("%d OpenCL Platforms found\n\n", platformCount);

	platformIDs = (cl_platform_id*)malloc(platformCount * sizeof(cl_platform_id));

	// get platform info for each platform
	err = clGetPlatformIDs(platformCount, platformIDs, NULL);
	if (err != CL_SUCCESS) {
		printf(" Error in clGetPlatformIDs Call!\n\n", err);
	}
	else {
		printf("clGetPlatformIDs works\n");
	}

	for (i = 0; i < platformCount; i++) {
		err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, 1024, &cBuffer, NULL);
		if (err != CL_SUCCESS) {
			printf(" Error in clGetPlatformInfo Call!\n\n", err);
		}


		err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, sizeof(cBuffer), cBuffer, NULL);
		if (err != CL_SUCCESS) {
			printf(" Error in clGetPlatformInfo Call!\n\n", err);
		}

		printf(" CL_PLATFORM_NAME: %d \t%s\n", i, cBuffer);
	}


	//Query for Devices
	printf("OpenCL Devices:\n\n");
	for (i = 0; i < platformCount; i++) {
		err = clGetDeviceIDs(platformIDs[i], CL_DEVICE_AVAILABLE, 0, NULL, &deviceCount);
		if (err != CL_SUCCESS) {
			printf(" Error %i in clGetDeviceIDs Call!\n\n", err);
		}
		else {
			printf("clGetDeviceIDs works\n");
		}
	}

	printf(" %u devicesIDs is found on: %s\n\n", deviceCount, cBuffer);

	for (i = 0; i < platformCount; i++) {
		devicesIDs = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
		err = clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL, deviceCount, devicesIDs, &deviceCount);
		if (err != CL_SUCCESS) {
			printf(" Error %i in clGetDeviceIDs Call!\n\n", err);
		}
		else {
			printf("clGetDeviceIDs works\n");
		}
	}

	for (j = 0; j < deviceCount; j++) {
		printf(" ----------------------------------\n");
		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
		printf(" CL_DEVICE_NAME :  %s\n", cBuffer);
		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cBuffer), &cBuffer, NULL);
		printf(" CL_DEVICE_DOUBLE_FP_CONFIG :  %s\n", cBuffer != 0 ? "YES" : "NO");


		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(readBuffer), &readBuffer, NULL);
		printf(" CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT :  %u\n", readBuffer);


		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(readBuffer), &readBuffer, NULL);
		printf(" CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT :  %u\n", readBuffer);


		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(readBuffer), &readBuffer, NULL);
		printf(" CL_DEVICE_MAX_CLOCK_FREQUENCY :  %u MHz\n", (cl_uint)readBuffer);
		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(readBuffer), &readBuffer, NULL);
		printf(" CL_DEVICE_MAX_COMPUTE_UNITS :  %u\n", readBuffer);

		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(rBufWorkDim), &rBufWorkDim, NULL);
		printf(" CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS :  %u\n", rBufWorkDim);
		clGetDeviceInfo(devicesIDs[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(rBufWorkSize), &rBufWorkSize, NULL);
		printf(" CL_DEVICE_MAX_WORK_ITEM_SIZES :  %u , %u, %u\n", rBufWorkSize[0], rBufWorkSize[1], rBufWorkSize[2]);


	}


}




