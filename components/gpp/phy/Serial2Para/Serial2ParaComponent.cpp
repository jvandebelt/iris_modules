/**
 * \file components/gpp/phy/Serial2Para/Serial2ParaComponent.cpp
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
 * Serial to parallel (?) component created by Wei
 */

#include "Serial2ParaComponent.h"

#include "irisapi/LibraryDefs.h"
#include "irisapi/Version.h"

using namespace std;

namespace iris
{
namespace phy
{

// export library symbols
IRIS_COMPONENT_EXPORTS(PhyComponent, Serial2ParaComponent);

Serial2ParaComponent::Serial2ParaComponent(std::string name)
  : PhyComponent(name,                      // component name
                "serial2para",              // component type
                "A serial to parallel component", // description
                "Wei Liu",                  // author
                "0.1")                      // version
{
  registerParameter(
      "factor",                             // name
      "the output will be a multiple of the input size",   // description
      "1",                                  // default value
      false,                                // dynamic?
      factor_x );                           // parameter

   registerParameter(
      "inputblocksize",                     // name
      "the input size",   					// description
      "52",                                 // default value
      false,                                // dynamic?
      inputBlocksize_x );                   // parameter                      
}

void Serial2ParaComponent::registerPorts()
{
  registerInputPort("input1", TypeInfo< float >::identifier);
  registerOutputPort("output1", TypeInfo< float >::identifier);
}

void Serial2ParaComponent::calculateOutputTypes(
    std::map<std::string,int>& inputTypes,
    std::map<std::string,int>& outputTypes)
{
  //One output type - always float
  outputTypes["output1"] = TypeInfo< float >::identifier;
}

void Serial2ParaComponent::initialize()
{
	isfirstblock = true;
	block_index = 0; 
	outputBlocksize = factor_x*inputBlocksize_x;
	//float buffer[outputBlocksize];
	data = (float*) malloc (outputBlocksize*sizeof(float));
}

void Serial2ParaComponent::process()
{
  //Get a DataSet from the input DataBuffer
  DataSet<float>* readDataSet = NULL;
  getInputDataSet("input1", readDataSet);
  
  if(!isfirstblock)
  {
		size_t size = readDataSet->data.size();
		
		for(int i=0;i<size;i++)
		{
	   		 data[block_index*inputBlocksize_x+i]=readDataSet->data[i];
       	}
		block_index++;
		//LOG(LINFO) << "block index " << block_index;
		if(block_index == factor_x)
		{
			block_index = 0;
  
			//Get a DataSet from the output DataBuffer
			DataSet<float>* writeDataSet = NULL;
			getOutputDataSet("output1", writeDataSet, outputBlocksize);

  			for(int i=0;i<outputBlocksize;i++)
			{   		 	
				writeDataSet->data[i]=data[i];
       		}

  			//Copy the timestamp and sample rate for the DataSets
  			writeDataSet->timeStamp = readDataSet->timeStamp;
  			writeDataSet->sampleRate = readDataSet->sampleRate;
            releaseOutputDataSet("output1", writeDataSet);
		}
  }

  //Release the DataSets
  releaseInputDataSet("input1", readDataSet);
  
  isfirstblock = false;
}

Serial2ParaComponent::~Serial2ParaComponent()
	{
		free((void*)data);
	}

} // namesapce phy
} // namespace iris
