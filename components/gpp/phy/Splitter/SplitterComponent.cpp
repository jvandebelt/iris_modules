/**
 * \file components/gpp/phy/Splitter/SplitterComponent.cpp
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
 * Implementation of a splitter PhyComponent that splits data into two or more outputs
 */

#include "SplitterComponent.h"

#include "irisapi/LibraryDefs.h"
#include "irisapi/Version.h"
#include "irisapi/TypeVectors.h"

using namespace std;

namespace iris
{
namespace phy
{

// export library symbols
IRIS_COMPONENT_EXPORTS(PhyComponent, SplitterComponent);

	template <class T>
    inline std::string toString (const T& t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

SplitterComponent::SplitterComponent(std::string name)
  : PhyComponent(name,                      // component name
                "splitter",                 // component type
                "Splits data into two or more outputs", // description
                "Paul Sutton",              // author
                "0.1")                      // version
{
  registerParameter(
      "numoutputs",                   		// name
      "Number of outputs to split across",               // description
      "2",                                  // default value
      false,                                // dynamic?
      numOutputs_x,                         // parameter
      Interval<unsigned>(1, 10));           // allowed values

}

void SplitterComponent::registerPorts()
{

  //This component supports all types
  vector<int> validTypes = convertToTypeIdVector<IrisDataTypes>();

  registerInputPort("input1", validTypes);

   for(unsigned i=1; i<=numOutputs_x; i++)
    {
        string portName = "output" + toString(i);
        registerOutputPort(portName, validTypes);
    }
}

void SplitterComponent::calculateOutputTypes(
    std::map<std::string,int>& inputTypes,
    std::map<std::string,int>& outputTypes)
{
  //Output type is the same as the input type
  for(unsigned i=1; i<=numOutputs_x; i++)
  {
      string portName = "output" + toString(i);
      outputTypes[portName] = inputTypes["input1"];
  }
}

void SplitterComponent::initialize()
{
}

void SplitterComponent::process()
{
  if( outputBuffers.size() != numOutputs_x || inputBuffers.size() != 1)
    {
        //Need to throw an exception here
    }

    switch(inputBuffers[0]->getTypeIdentifier())
    {
    case 0:
        writeOutput<uint8_t>();
        break;
    case 1:
        writeOutput<uint16_t>();
        break;
    case 2:
        writeOutput<uint32_t>();
        break;
    case 3:
        writeOutput<uint64_t>();
        break;
    case 4:
        writeOutput<int8_t>();
        break;
    case 5:
        writeOutput<int16_t>();
        break;
    case 6:
        writeOutput<int32_t>();
        break;
    case 7:
        writeOutput<int64_t>();
        break;
    case 8:
        writeOutput<float>();
        break;
    case 9:
        writeOutput<double>();
        break;
    case 10:
        writeOutput<long double>();
        break;
    case 11:
        writeOutput< complex<float> >();
        break;
    case 12:
        writeOutput< complex<double> >();
        break;
    case 13:
        writeOutput<complex< long double> >();
        break;
    default:
        break;
    }
}

	template<typename T>
    void SplitterComponent::writeOutput()
    {
       DataSet<T>* readDataSet = NULL;
       DataSet<T>* writeDataSet = NULL;

       getInputDataSet("input1", readDataSet);
       size_t size = readDataSet->data.size();

       for(unsigned i=1;i<=numOutputs_x;i++)
       {
		   std::string portName = "output" + toString(i);

           getOutputDataSet(portName, writeDataSet, size);
           copy(readDataSet->data.begin(), readDataSet->data.end(), writeDataSet->data.begin());
           releaseOutputDataSet(portName, writeDataSet);
       }

       releaseInputDataSet("input1", readDataSet);
    }

} // namesapce phy
} // namespace iris
