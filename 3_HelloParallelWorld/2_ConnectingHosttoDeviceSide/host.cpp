//HOST CODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CL/cl.h"
#include "read_source.h"
#include <Windows.h>
#include<math.h>

#define Size 512 //NXN matrix with N=512

/*Function for calculating average Run Time*/
double sumRuntime(double *acc_time, int N) {
	double sumtime = 0.0;
	for (int i = 0; i < N; i++) {
		sumtime += acc_time[i];
	}
	return (sumtime / N);
}

/*Function for calculating Standard Deviation*/
double sdcl(double *acc_time, int N, double mean) {
	double sd = 0.0;
	for (int i = 0; i < N; i++) {
		sd += pow(acc_time[i] - mean, 2);
	}
	sd = sqrt(sd / N);
	return sd;
}

int main(int argc, char** argv) {

	/*All Variables*/
	unsigned int i, j;
	//Platform and Device variables
	char cBuffer[1024];
	cl_uint platformCount, deviceCount;
	cl_platform_id* platformIDs;
	cl_device_id *deviceIDs;
	cl_int err;
	cl_platform_id selectedPlatformID = NULL;
	cl_device_id selectedDeviceID = NULL;

	//WorkSizes
	size_t global_work_size[3] = { Size,Size,0 }, local_work_size[3] = { 1,64,0 };//change local size as { 2,128,0} for OPt1 calc

	//Profiling Variables
	/*
	  Switch to operate either open CL profiling or Windows Profiling
	  WIN_ON_OCL_OFF =1 --> Windows Profiling
	  WIN_ON_OCL_OFF =0 --> Open CL Profiling
	*/
	bool WIN_ON_OCL_OFF = 0;
	const int iterations = 500; //Number of iterations over which average run time and standard deviation is taken

	//OpenCl profiling elements
	double acc_time_start_end_cl[iterations];
	double acc_time_submit_end_cl[iterations];
	double run_time_start_end_cl, run_time_submit_end_cl;
	int num_itern = 0;
	cl_event prof_event;
	cl_ulong submit_time = (cl_ulong)0, start_time = (cl_ulong)0, end_time = (cl_ulong)0;
	size_t return_bytes;
	double avg_time_cl_start_end, sd_cl_start_end, avg_time_cl_submit_end, sd_cl_submit_end;

	//Windows profiling variables
	LARGE_INTEGER perfFreq, perfCountNDRangeStart, perfCountNDRangeStop, perfCountClFinishStop;
	double  run_time_startndr_endndr_win, run_time_startndr_endclf_win;
	double acc_time_startndr_endndr_win[iterations];
	double acc_time_startndr_endclf_win[iterations];
	double avg_time_win_startndr_endndr, sd_win_startndr_endndr, avg_time_win_startndr_endclf, sd_win_startndr_endclf;

	//===================================================================================================================================
	// Basic Setup
	//===================================================================================================================================


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



	err = clGetDeviceIDs(selectedPlatformID, CL_DEVICE_TYPE_GPU, 1, deviceIDs, &deviceCount); // GPU is selected as the device
	if (err != CL_SUCCESS) {
		printf(" Error %i in clGetDeviceIDs Call!\n\n", err);
	}
	else {
		printf(" clGetDeviceIDs works\n");
	}

	selectedDeviceID = *deviceIDs;
	clGetDeviceInfo(selectedDeviceID, CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
	printf(" Selected Device: %d \t%s\n\n", i, cBuffer);

	//===============================================================================================================================================
	//End of Basic Code
	//Create Context.Used a Kernel Source, Build the program
	//===============================================================================================================================================


	//Create Context 

	cl_context context = clCreateContext(NULL, 1, &selectedDeviceID, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		printf(" Error %i in clCreateContext Call!\n\n", err);
	}
	else {
		printf(" clCreateContext works\n");
	}


	size_t file_size = 0;
	char * kernel_source = read_source("mmul_naive.cl", &file_size); // change the "mmul_naive.cl" to "mmul_opt1.cl" for optimised 1 calculations


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
	//=============================================================================================================================================
	// if WIN_ON_OCL_OFF =1 -> Windows Profiling 
	// else OpenCL Profiling
	//=============================================================================================================================================




	if (WIN_ON_OCL_OFF)
	{
		while (num_itern < iterations)
		{

			//Create Command Queue
			cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, selectedDeviceID, 0, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in clCreateCommandQueueWithProperties  Call!\n\n", err);
			}
			else {
				printf(" clCreateCommandQueueWithProperties works\n");
			}

			//Create Kernel Objects
			cl_kernel createdkernel = clCreateKernel(program, "mmul_naive", &err); // change the "mmul_naive" to "mmul_opt1" for optimised 1 calculations
			if (err != CL_SUCCESS) {
				printf(" Error %i in clCreateKernel  Call!\n\n", err);
			}
			else {
				printf(" clCreateKernel works\n");
			}
			//=============================================================================================================================================
			//Create buffer objects and assign them
			//=============================================================================================================================================

			//Creating Buffer Objects

			float *a = (float *)malloc(sizeof(float)*Size*Size);
			float *c = (float *)malloc(sizeof(float)*Size*Size);
			printf("malloc works");
			/*Assigning values of matrix as 1-512 in each row*/
			for (int i = 0; i < Size*Size; i++) {
				a[i] = (i + 1) % Size;
			}

			printf("assignment  works");

			//Creation of Data Buffers

			cl_mem bufA = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size*Size, a, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in Create Buffer  Call!\n\n", err);
			}
			else {
				printf(" clCreateBuffer works\n");
			}

			cl_mem bufC = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size*Size, c, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in Create Buffer  Call!\n\n", err);
			}
			else {
				printf(" clCreateBuffer works\n");
			}

			int N = Size; //N=512
			//Set Kernel Arguments
			clSetKernelArg(createdkernel, 0, sizeof(int), &N);
			clSetKernelArg(createdkernel, 1, sizeof(bufA), &bufA);
			clSetKernelArg(createdkernel, 2, sizeof(bufA), &bufA);
			clSetKernelArg(createdkernel, 3, sizeof(bufC), &bufC);

			printf(" Argument Setting works\n");

			//Start Windows Profiling
			QueryPerformanceCounter(&perfCountNDRangeStart);

			//Execute kernel
			err = clEnqueueNDRangeKernel(command_queue, createdkernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
			if (err != CL_SUCCESS) {
				printf(" Error %s in Enqueue ND range Kernel  Call!\n\n", err);
			}
			else {
				printf(" clEnqueueNDRangeKernel works\n");
			}

			//End NDRange profiling
			QueryPerformanceCounter(&perfCountNDRangeStop);

			err = clFinish(command_queue);
			if (err != CL_SUCCESS) {
				printf(" Error %i in clFinish  Call!\n\n", err);
			}
			else
			{
				printf(" clFinish works\n");
			}

			//Subcase : 2 End of total Profiling : clEnqueueNDRangeKernel and clFinish 
			QueryPerformanceCounter(&perfCountClFinishStop);
			QueryPerformanceFrequency(&perfFreq);

			// compute execution time in milliseconds (ms)
			run_time_startndr_endndr_win = (double)1000.0f*(float)(perfCountNDRangeStop.QuadPart - perfCountNDRangeStart.QuadPart) / (float)perfFreq.QuadPart;
			run_time_startndr_endclf_win = (double)1000.0f*(float)(perfCountClFinishStop.QuadPart - perfCountNDRangeStart.QuadPart) / (float)perfFreq.QuadPart;
			acc_time_startndr_endndr_win[num_itern] = run_time_startndr_endndr_win;
			acc_time_startndr_endclf_win[num_itern] = run_time_startndr_endclf_win;
			num_itern++;
			free(a);
			free(c);
		}
		avg_time_win_startndr_endndr = sumRuntime(acc_time_startndr_endndr_win, iterations);
		sd_win_startndr_endndr = sdcl(acc_time_startndr_endndr_win, iterations, avg_time_win_startndr_endndr);
		avg_time_win_startndr_endclf = sumRuntime(acc_time_startndr_endclf_win, iterations);
		sd_win_startndr_endclf = sdcl(acc_time_startndr_endclf_win, iterations, avg_time_win_startndr_endclf);
		printf("\n\n 3b.i.Windows Performance Counter(WPC)API on a GPU\n");
		printf("The Program is Naive Matrix Multiplication kernel : mmul_naive\n");
		printf("Global work size : %dX%d \n Local work size : 1X64\n", Size, Size);
		printf("a. WPC-Case-1 : Performance of clEnqueueNDRangeKernel --> Time taken %Lf milli seconds Standard Deviation %Lf \n\n", avg_time_win_startndr_endndr, sd_win_startndr_endndr);
		printf("b. WPC-Case-2 : Performance of clEnqueueNDRangeKernel and clFinish --> Time taken %Lf milli seconds Standard Deviation %Lf \n ", avg_time_win_startndr_endclf, sd_win_startndr_endclf);

	}
	/*OpenCl profiling*/
	else
	{
		while (num_itern < iterations) {
			start_time = (cl_ulong)0;
			end_time = (cl_ulong)0;
			submit_time = (cl_ulong)0;
			//Creating Command Queue with Profiling enabled
			cl_command_queue command_queue = clCreateCommandQueue(context, selectedDeviceID, CL_QUEUE_PROFILING_ENABLE, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in clCreateCommandQueueWithProperties  Call!\n\n", err);
			}
			else {
				printf(" clCreateCommandQueueWithProperties works\n");
			}

			//Create Kernel Objects
			cl_kernel createdkernel = clCreateKernel(program, "mmul_naive", &err); // change the "mmul_naive" to "mmul_opt1" for optimised 1 calculations
			if (err != CL_SUCCESS) {
				printf(" Error %i in clCreateKernel  Call!\n\n", err);
			}
			else {
				printf(" clCreateKernel works\n");
			}
			//=============================================================================================================================================
			//Create buffer objects and assign them
			//=============================================================================================================================================

			//Creating Buffer Objects

			float *a = (float *)malloc(sizeof(float)*Size*Size);
			float *c = (float *)malloc(sizeof(float)*Size*Size);

			for (i = 0; i < Size*Size; i++) {
				a[i] = (i + 1) % Size;
			}



			cl_mem bufA = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size*Size, a, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in Create Buffer  Call!\n\n", err);
			}
			else {
				printf(" clCreateBuffer works\n");
			}

			cl_mem bufC = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(float)*Size*Size, c, &err);
			if (err != CL_SUCCESS) {
				printf(" Error %i in Create Buffer  Call!\n\n", err);
			}
			else {
				printf(" clCreateBuffer works\n");
			}

			int N = Size;
			//Set Kernel Arguments
			clSetKernelArg(createdkernel, 0, sizeof(int), &N);
			clSetKernelArg(createdkernel, 1, sizeof(bufA), &bufA);
			clSetKernelArg(createdkernel, 2, sizeof(bufA), &bufA);
			clSetKernelArg(createdkernel, 3, sizeof(bufC), &bufC);

			printf(" Argument Setting works\n");

			//Execute kernel

			err = clEnqueueNDRangeKernel(command_queue, createdkernel, 1, NULL, global_work_size, local_work_size, 0, NULL, &prof_event);
			if (err != CL_SUCCESS) {
				printf(" Error %s in Enqueue ND range Kernel  Call!\n\n", err);
			}
			else {
				printf(" clEnqueueNDRangeKernel works\n");
			}

			//Wait for finish and for all events to end
			err = clFinish(command_queue);
			if (err != CL_SUCCESS) {
				printf(" Error %i in clFinish  Call!\n\n", err);
			}
			else {
				printf(" clFinish works\n");
			}
			err = clWaitForEvents(1, &prof_event);
			if (err != CL_SUCCESS) {
				printf(" Error %d in clWaitForEvents  Call!\n\n", err);
			}
			else {
				printf(" clWaitForEvents works\n");
			}


			//Profiling Info 
			err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, &return_bytes);
			if (err != CL_SUCCESS) {
				printf(" Error %d in START clGetEventProfilingInfo  Call!\n\n", err);
			}
			else {
				printf(" START clGetEventProfilingInfo works\n");
			}
			err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit_time, &return_bytes);
			if (err != CL_SUCCESS) {
				printf(" Error %d in START clGetEventProfilingInfo  Call!\n\n", err);
			}
			else {
				printf(" START clGetEventProfilingInfo works\n");
			}
			err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, &return_bytes);
			if (err != CL_SUCCESS) {
				printf(" Error %s in END clGetEventProfilingInfo  Call!\n\n", err);
			}
			else {
				printf(" END clGetEventProfilingInfo works\n");
			}

			run_time_start_end_cl = (double)(end_time - start_time) * 1e-9 *1000.0; //Profiling info is given out in nano seconds,converting to milli seconds
			printf("Time taken %Lf seconds", run_time_start_end_cl);
			acc_time_start_end_cl[num_itern] = run_time_start_end_cl;
			run_time_submit_end_cl = (double)(end_time - submit_time) * 1e-9*1000.0;
			acc_time_submit_end_cl[num_itern] = run_time_submit_end_cl;
			num_itern++;
			free(a);
			free(c);

		}
		double avg_time_cl_start_end = sumRuntime(acc_time_start_end_cl, iterations);
		double sd_cl_start_end = sdcl(acc_time_start_end_cl, iterations, avg_time_cl_start_end);
		double avg_time_cl_submit_end = sumRuntime(acc_time_submit_end_cl, iterations);
		double sd_cl_submit_end = sdcl(acc_time_submit_end_cl, iterations, avg_time_cl_submit_end);
		printf("\n\n 3b.ii.OpenCL (OCL) event-based command profiling on a GPU\n");
		printf(" The Program is Naive Matrix Multiplication kernel : mmul_naive\n");
		printf(" Global work size : %dX%d \n Local work size : 1X64\n", Size, Size);
		printf(" a. OCL-Set-1: SUBMIT : END --> Time taken %Lf milliseconds Standard Deviation %Lf \n\n", avg_time_cl_submit_end, sd_cl_submit_end);
		printf(" b. OCL-Set-2: START : END --> Time taken %Lf milliseconds Standard Deviation %Lf \n ", avg_time_cl_start_end, sd_cl_start_end);
	}

}