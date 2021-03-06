/**
 * \file components/gpp/phy/Serial2Para/Serial2ParaComponent.h
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

#ifndef PHY_SERIAL2PARACOMPONENT_H_
#define PHY_SERIAL2PARACOMPONENT_H_

#include "irisapi/PhyComponent.h"

namespace iris
{
namespace phy
{

/** Serial to paralel (?) component created by Wei
 *
 * 
 */
class Serial2ParaComponent
  : public PhyComponent
{
 public:
	/** Construct this component.
	 *
	 * Call the constructor on PhyComponent and pass in all details
	 * about the component.	Register all parameters and events.
	 * \param name the name given to this component in the radio config
	 */
  Serial2ParaComponent(std::string name);
  
  ~Serial2ParaComponent();

  /** Calculate the output types generated by this component.
   *
	 * Given the data-types associated with each input port, provide
	 * the data-types which will be produced on each output port.
	 * \param inputTypes  The map of input port names and data-type identifiers
	 * \param outputTypes Map of output port names and data-type identifiers
	 */
  virtual void calculateOutputTypes(
      std::map<std::string, int>& inputTypes,
      std::map<std::string, int>& outputTypes);

  /** Register the ports of this component.
   *
	 * Register ports by name, declare them as input or output and
	 * provide a list of valid data types.
	 */
  virtual void registerPorts();

	/// Do any initialization required by this component.
  virtual void initialize();

  /** Process input data and generate output data
   *
	 * This is where the work of this component gets done.
	 * Typically components will take DataSets from their input
	 * ports, process them and write DataSets to their output ports.
	 */
  virtual void process();

 private:

	uint32_t factor_x;
	uint32_t inputBlocksize_x;
	uint32_t block_index;
	uint32_t outputBlocksize;
	bool isfirstblock;
	float *data;
};

} // namespace phy
} // namespace iris

#endif // PHY_SERIAL2PARACOMPONENT_H_
