/**
 * \file components/gpp/phy/SampleSelector/SampleSelectorComponent.cpp
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
 * Sample Selector Component written by Wei
 */

#include "SampleSelectorComponent.h"

#include "irisapi/LibraryDefs.h"
#include "irisapi/Version.h"


using namespace std;

namespace iris
{
namespace phy
{

// export library symbols
IRIS_COMPONENT_EXPORTS(PhyComponent, SampleSelectorComponent);

SampleSelectorComponent::SampleSelectorComponent(std::string name)
  : PhyComponent(name,                      // component name
                "sampleselector",           // component type
                "A sample selector component", // description
                "Wei Liu",                  // author
                "0.1")                      // version
{
  registerParameter(
      "sampleoffset",                       // name
      "the position of first sample to be selected",               // description
      "0",                                  // default value
      true,                                 // dynamic?
      sampleOffset_x);                      // parameter

  registerParameter(
	  "sampletotal", 
	  "the total number of samples to be selected", 
	  "0", 
	  true, 
	  totalNumberOfSamples_x);
}

void SampleSelectorComponent::registerPorts()
{
  registerInputPort("input1", TypeInfo< float >::identifier);
  registerOutputPort("output1", TypeInfo< float >::identifier);
}

void SampleSelectorComponent::calculateOutputTypes(
    std::map<std::string,int>& inputTypes,
    std::map<std::string,int>& outputTypes)
{
  //One output type - always float
  outputTypes["output1"] = TypeInfo< float >::identifier;
}

void SampleSelectorComponent::initialize()
{
}

void SampleSelectorComponent::process()
{
  //Get a DataSet from the input DataBuffer
  DataSet<float>* readDataSet = NULL;
  getInputDataSet("input1", readDataSet);
  std::size_t size = readDataSet->data.size();
  
  if(size < totalNumberOfSamples_x) {
			LOG(LINFO) << "not enough samples " << totalNumberOfSamples_x;
		}

  //Get a DataSet from the output DataBuffer
  DataSet<float>* writeDataSet = NULL;
  getOutputDataSet("output1", writeDataSet, totalNumberOfSamples_x);

  //Copy the input DataSet to the output DataSet
		for(int i=0;i<totalNumberOfSamples_x;i++)
		{
		    writeDataSet->data[i] =  readDataSet->data[i+sampleOffset_x];
		}

  //Copy the timestamp and sample rate for the DataSets
  writeDataSet->timeStamp = readDataSet->timeStamp;
  writeDataSet->sampleRate = readDataSet->sampleRate;

  //Release the DataSets
  releaseInputDataSet("input1", readDataSet);
  releaseOutputDataSet("output1", writeDataSet);
}

} // namesapce phy
} // namespace iris
