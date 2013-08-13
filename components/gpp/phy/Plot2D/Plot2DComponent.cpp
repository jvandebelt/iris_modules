/**
 * \file components/gpp/phy/Plot2D/Plot2DComponent.cpp
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
 * A 2D Plot PhyComponents.
 */

#include "Plot2DComponent.h"

#include "irisapi/LibraryDefs.h"
#include "irisapi/Version.h"

using namespace std;

namespace iris
{
namespace phy
{

// export library symbols
IRIS_COMPONENT_EXPORTS(PhyComponent, Plot2DComponent);

Plot2DComponent::Plot2DComponent(std::string name)
  : PhyComponent(name,                      // component name
                "plot2d",                   // component type
                "A 2D plot component",      // description
                "Wei Liu",                  // author
                "0.1")                      // version
{
  registerParameter(
      "ymin",                   			// name
      "minimum value on the graph on vertical axis",               // description
      "-1",                                 // default value
      true,                                 // dynamic?
      ymin_x);                              // parameter

  registerParameter(
      "ymax",                   			// name
      "maximum value on the graph on vertical axis",               // description
      "0",                                 // default value
      true,                                 // dynamic?
      ymax_x);                              // parameter

  registerParameter(
      "xmin",                   			// name
      "minimum value on the graph on horizontal axis",               // description
      "-1",                                 // default value
      true,                                 // dynamic?
      xmin_x);                              // parameter

  registerParameter(
      "xmax",                   			// name
      "maximum value on the graph on horizontal axis",               // description
      "0",                                 // default value
      true,                                 // dynamic?
      xmax_x);                              // parameter

 registerParameter(
	  "numberofsamples",
	  "number of samples",
	  "0", 
	  true,
	  numofsamples_x);
 registerParameter(
	  "autoscale",
	  "set autoscale on/off",
	  "false",
	  true, 
	  auto_x);

}

void Plot2DComponent::registerPorts()
{
  registerInputPort("input1", TypeInfo< float >::identifier);
}

void Plot2DComponent::calculateOutputTypes(
    std::map<std::string,int>& inputTypes,
    std::map<std::string,int>& outputTypes)
{

}

void Plot2DComponent::initialize()
{
		x_yaverage = (ymax_x-ymin_x)/2; 
		float step = (xmax_x - xmin_x)/numofsamples_x;
		xarray = (float*) malloc(numofsamples_x*sizeof(float));
		for(int i = 0; i< numofsamples_x;i++){
			xarray[i] = xmin_x + i*step;
		}
		LOG(LINFO) << "x step " << step << " xmin " << xarray[0] << "xmax " << xarray	[numofsamples_x-1];
		FILE *pipetemp = popen("gnuplot -persist","w");
		pipe = pipetemp;
		fprintf(pipe, "set samples %d\n",numofsamples_x);
		fprintf(pipe,"set yrange [%f:%f]\n",ymin_x,ymax_x);
		fprintf(pipe, "set xrange [%f:%f]\n",xmin_x,xmax_x);
    
}

void Plot2DComponent::process()
{
  //Get a DataSet from the input DataBuffer
  DataSet<float>* readDataSet = NULL;
  getInputDataSet("input1", readDataSet);
  std::size_t size = readDataSet->data.size();
  if(size < numofsamples_x) LOG(LINFO) << "not enough samples " << size;

  //Get a DataSet from the output DataBuffer
  //DataSet<uint32_t>* writeDataSet = NULL;
  //getOutputDataSet("output1", writeDataSet, size);

  float data[size];
  for(int i=0;i<size;i++)
  {
      data[i]=readDataSet->data[i];
  }
  gnuPlot2DPersist(pipe,size,xarray,data);

  //Copy the timestamp and sample rate for the DataSets
  //writeDataSet->timeStamp = readDataSet->timeStamp;
  //writeDataSet->sampleRate = readDataSet->sampleRate;

  //Release the DataSets
  releaseInputDataSet("input1", readDataSet);
  //releaseOutputDataSet("output1", writeDataSet);
}
void Plot2DComponent::wait ( float seconds )
{
	  clock_t endwait;
	  endwait = clock () + seconds * CLOCKS_PER_SEC ;
	  while (clock() < endwait) {}
}
	
int Plot2DComponent::gnuPlot2DPersist(FILE *pipe,int num_of_samples,float x[],float y[])
{
	int i;	
	float t = 0;
	fprintf(pipe, "plot '-' with lines\n");
	for(i=0;i<num_of_samples;i++)
	{
		
		
		if(y[i] < ymin_x)
			ymin_x = y[i];
			           
		if(y[i] > ymax_x)
			ymax_x = y[i];
			
		t += y[i];

		fprintf(pipe,"%f %f\n",x[i],y[i]);
		
	}
	fprintf(pipe,"e\n");
	
	//Autoscale
	if(auto_x)
	{
		//x_yaverage = t/num_of_samples;
		//if(ymin_x/x_yaverage > 3)
			//ymin_x = ymin_x/2;
		
		//if( ymax_x/x_yaverage > 5)
			//ymax_x = ymax_x/3;

	fprintf(pipe,"set yrange [%f:%f]\n",ymin_x,ymax_x);
	}

	return 0;
}

Plot2DComponent::~Plot2DComponent()
{
	pclose(pipe);
}


} // namesapce phy
} // namespace iris
