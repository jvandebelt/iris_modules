/**
 * \file components/gpp/phy/Example/ExampleComponent.cpp
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * 
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * A Periodogram component.
 */

#include "PeriodogramComponent.h"

#include "irisapi/LibraryDefs.h"
#include "irisapi/Version.h"

using namespace std;

namespace iris
{
namespace phy
{

// export library symbols
IRIS_COMPONENT_EXPORTS(PhyComponent, PeriodogramComponent);

PeriodogramComponent::PeriodogramComponent(std::string name)
  : PhyComponent(name,                      // component name
                "Periodogram",              // component type
                "A Periodogram component",  // description
                "Wei Liu",                  // author
                "0.1")                      // version
{
  registerParameter(
      "number_of_complex_samples",          // name
      "incoming complex samples",           // description
      "16384",                              // default value
      true,                                 // dynamic?
      x_number_of_complex_samples);			// parameter

  registerParameter(
	 "blocksize",
 	 "output block size by periodogram", 
 	 "1024", 
	  true, 
	  x_blocksize);
  registerParameter(
	  "shift",
	  "if fftshift is requested, 1 means yes",
	  "1",
	  false, 
	  x_shift);	
  registerParameter(
	  "window",
	  "if han window is applied, 1 means yes",
	  "0",
	  false, 
	  x_window);

}

void PeriodogramComponent::registerPorts()
{
  registerInputPort("input1", TypeInfo< complex<float> >::identifier);
  registerOutputPort("output1", TypeInfo< float >::identifier);
}

void PeriodogramComponent::calculateOutputTypes(
    std::map<std::string,int>& inputTypes,
    std::map<std::string,int>& outputTypes)
{
  
  outputTypes["output1"] = TypeInfo< float >::identifier;
}

void PeriodogramComponent::initialize()
{
}

void PeriodogramComponent::process()
{
  //Get a DataSet from the input DataBuffer
  DataSet< complex<float> >* readDataSet = NULL;
  getInputDataSet("input1", readDataSet);
  std::size_t size = readDataSet->data.size();

  //Get a DataSet from the output DataBuffer
  DataSet< float >* writeDataSet = NULL;
  getOutputDataSet("output1", writeDataSet, x_blocksize);

  //FFT process
		
	//float *data =(float*) malloc (size*sizeof(float));
	float data[2*size],PSDresult[x_blocksize];	
        for(int i=0;i<size;i++)
        {
            data[2*i+1]=readDataSet->data[i].real();
            data[2*i]=readDataSet->data[i].imag();
        }
	periodogram(size,x_blocksize,data,PSDresult);
	log10Array(PSDresult,x_blocksize);
	for(int i=0;i<x_blocksize;i++)
	{
	    writeDataSet->data[i] = PSDresult[i];
	}

  //Copy the timestamp and sample rate for the DataSets
  writeDataSet->timeStamp = readDataSet->timeStamp;
  writeDataSet->sampleRate = readDataSet->sampleRate;

  //Release the DataSets
  releaseInputDataSet("input1", readDataSet);
  releaseOutputDataSet("output1", writeDataSet);
}

float PeriodogramComponent::FFT (float data[], unsigned long number_of_complex_samples, int isign)
	{
	    //variables for trigonometric recurrences

	    unsigned long n,mmax,m,j,istep,i;
	    double wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	    //the complex array is real+complex so the array 

	    // has a size n = 2* number of complex samples

	    // real part is the data[index] and the complex part is the data[index+1]

	    n=number_of_complex_samples * 2; 

	    //binary inversion (note that 

	    //the indexes start from 1 which means that the

	    //real part of the complex is on the odd-indexes

	    //and the complex part is on the even-indexes

	     j=0;
	    for (i=0;i<n/2;i+=2) {
		if (j > i) {
		    //swap the real part

		    SWAP(data[j],data[i]);
		    //swap the complex part

		    SWAP(data[j+1],data[i+1]);
		    // checks if the changes occurs in the first half

		    // and use the mirrored effect on the second half

		    if((j/2)<(n/4)){
		        //swap the real part

		        SWAP(data[(n-(i+2))],data[(n-(j+2))]);
		        //swap the complex part

		        SWAP(data[(n-(i+2))+1],data[(n-(j+2))+1]);
		    }
		}
		m=n/2;
		while (m >= 2 && j >= m) {
		    j -= m;
		    m = m/2;
		}
		j += m;
	    }
		//Danielson-Lanzcos routine 

	    mmax=2;
	    //external loop

	    while (n > mmax)
	    {
		istep = mmax<<  1;
		theta=(2*M_PI/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		//internal loops

		for (m=1;m<mmax;m+=2) {
		    for (i= m;i<=n;i+=istep) {
		        j=i+mmax;
		        tempr=wr*data[j-1]-wi*data[j];
		        tempi=wr*data[j]+wi*data[j-1];
		        data[j-1]=data[i-1]-tempr;
		        data[j]=data[i]-tempi;
		        data[i-1] += tempr;
		        data[i] += tempi;
		    }
		    wr=(wtemp=wr)*wpr-wi*wpi+wr;
		    wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	    }
		return data[0];
	}

	int PeriodogramComponent::PSDlog (float data[], float dataOut[],int N)
	{
		int i;
		for(i=0;i<2*N;i+=2)
		{ 
			dataOut[i/2]=data[i]*data[i]+data[i+1]*data[i+1];
			dataOut[i/2]=10*log10f(dataOut[i/2]);
		}
		return 0;

	}
	int PeriodogramComponent::PSD (float data[], float dataOut[],int N)
	{
		int i;
		for(i=0;i<2*N;i+=2)
		{ 
			dataOut[i/2]=data[i]*data[i]+data[i+1]*data[i+1];
		}
		return 0;

	}
		// Shift the center frequency to the middle and mirror the left part
	void PeriodogramComponent::PSDshift(float data[],float dataOut[],unsigned long number_of_total_samples)
	{
		int i;
		for(i = 0;i<number_of_total_samples/2;i++)
		{
			dataOut[i] = data[i+number_of_total_samples/2];
		}
		for(i = number_of_total_samples/2;i<number_of_total_samples;i++)
		{
			dataOut[i] = data[i-number_of_total_samples/2];
		}

	}
	// the same as han window in matlab and octave
	int PeriodogramComponent::winHan(int N,float windowedData[])
	{
		float *window = (float*) malloc (N*sizeof(float));	
		int i;
		for(i=0;i<N;i++)
		{
			window[i]=0.5*( 1-cos( 2*pi*(i+1)/(N+1) ) );
		}
		for(i=0;i<2*N;i+=2)
		{
			windowedData[i]*=window[i/2];
			windowedData[i+1]*=window[i/2];
		}
		free((void*)window);
		return 0;
	}
	// totallength and N are number of complex samples
	void PeriodogramComponent::periodogram(int totallength,int N,float data[],float result[])
	{
		 int num_of_block = 2*totallength/N-1;
		 float datablock[2*N],PSDresult[N],PSDresultshift[N];
		 int offset;
		 clearArray(result,N);
		 for( int i= 1;i<=num_of_block;i++)
		 {
		     offset = N/2*(i-1);
		     copyblock(offset,N,data,datablock);
		     winHan(N,datablock);
		     FFT (datablock, N, 1);
		     PSD(datablock,PSDresult,N);
		     PSDshift(PSDresult,PSDresultshift,N);
		     addArray(result,PSDresultshift,N);
		 }
		 avgArray(result,N,num_of_block);
	}
	void PeriodogramComponent::copyblock(int start,int N,float data[],float dataOut[])
	{
		for(int i = 0;i<2*N;i++){
			dataOut[i] = data[start+i];
		}
	}
	void PeriodogramComponent::addArray(float arrayA[],float arrayB[],int length)
	{
		for(int i=0;i<length;i++)
		{
			arrayA[i] = arrayA[i] + arrayB[i];
		}
		
	}
	void PeriodogramComponent::clearArray(float arrayA[],int length)
	{
		for(int i=0;i<length;i++)
		{
			arrayA[i] = 0.0;
		}
	}
	void PeriodogramComponent::avgArray(float arrayA[],int length,int dev)
	{
		for(int i=0;i<length;i++)
		{
			arrayA[i] /= dev ;
		}
		
	}
	void PeriodogramComponent::log10Array(float arrayA[],int length)
	{
		for(int i=0;i<length;i++){
			arrayA[i] = 10*log10f(arrayA[i]);
		}		
	}


} // namesapce phy
} // namespace iris
