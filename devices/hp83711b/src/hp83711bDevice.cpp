/*! \file hp83711bDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class hp83711bDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "hp83711bDevice.h"

hp83711bDevice::hp83711bDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; // normally 16
	secondaryAddress = 0;
	outputOn = false; // default to power off
	frequency = 3.36; // in GHz
	frequencyIncrement = 0.001; // in GHz
	outputPower = -7.0; // in dBm
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
}

hp83711bDevice::~hp83711bDevice()
{
}


void hp83711bDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("Frequency (GHz)", frequency);
	addAttribute("Frequency Increment (GHz)", frequencyIncrement);
	addAttribute("Output", "Off", "Off, On");
	addAttribute("Output Power (dBm)", outputPower);
}

void hp83711bDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //response to the IDN? query
	setAttribute("Frequency (GHz)", frequency);
	setAttribute("Frequency Increment (GHz)", frequencyIncrement);
	setAttribute("Output", (outputOn ? "On" : "Off"));
	setAttribute("Output Power (dBm)", outputPower);
}

bool hp83711bDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;
	string result;

	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("*idn?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("Frequency (GHz)") == 0)
	{
		bool successFrequency = stringToValue(value, newFrequency);
		if(successFrequency && newFrequency <= 20 && newFrequency >= 1) 
		{
			
			std::string frequencyCommand = "FREQ:CW " + value + " GHZ";
			std::cerr << "frequency_command_str: " << frequencyCommand << std::endl;
			commandSuccess = commandDevice(frequencyCommand);
			if(commandSuccess)
			{
				result = queryDevice("FREQ:CW?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successFrequency = stringToValue(result, frequency);
					std::cerr << "device successfully commanded"<< std::endl;
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired frequency is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Frequency Increment (GHz)") == 0)
	{
		bool successFrequencyIncrement = stringToValue(value, newFrequencyIncrement);
		if(successFrequencyIncrement && newFrequencyIncrement <= 20 && newFrequencyIncrement > 0) 
		{
			
			std::string frequencyIncrementCommand = "FREQ:STEP:INCR " + value + " GHZ";
			std::cerr << "frequencyIncrementCommand_str: " << frequencyIncrementCommand << std::endl;
			commandSuccess = commandDevice(frequencyIncrementCommand);
			if(commandSuccess)
			{
				result = queryDevice("FREQ:CW:STEP:INCR?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successFrequencyIncrement = stringToValue(result, frequencyIncrement);
					std::cerr << "device successfully commanded"<< std::endl;
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired frequency increment is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Output Power (dBm)") == 0)
	{
		bool successOutputPower = stringToValue(value, newOutputPower);
		if(successOutputPower && newOutputPower <= 16 && newOutputPower >= -10) 
		{
			
			std::string outputPowerCommand = ":POW:LEV " + value + " dBm";
			std::cerr << "outputPowerCommand: " << outputPowerCommand << std::endl;
			commandSuccess = commandDevice(outputPowerCommand);
			std::cerr << "device successfully commanded"<< std::endl;
			if(commandSuccess)
			{
				result = queryDevice("POW:LEV?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successOutputPower = stringToValue(result, outputPower);
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired output power is outside of the allowed range." << std::endl;
			success = false;
		}
	}
	else if(key.compare("Output") == 0)
	{
		if(value.compare("On") == 0)
		{
			commandSuccess = commandDevice("OUTP ON");
			outputOn = true;
		}
		else
		{
			commandSuccess = commandDevice("OUTP OFF");
			outputOn = false;
		}
		if(commandSuccess)
		{
			result = queryDevice("OUTP?");
			success = stringToValue(result, outputOn);
		}
		else
			success = false;
	}


	return success;
}

void hp83711bDevice::defineChannels()
{
}

bool hp83711bDevice::writeChannel(const RawEvent& Event)
{
	return false;
}

bool hp83711bDevice::readChannel(DataMeasurement& Measurement)
{
	return false;
}

void hp83711bDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void hp83711bDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "li-gpib.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void hp83711bDevice::stopEventPlayback()
{
}

std::string hp83711bDevice::execute(int argc, char **argv)
{
	std::vector<std::string> argvOutput;
	convertArgs(argc, argv, argvOutput);
	bool success;

	if(argvOutput.size() == 3)
		success = setAttribute(argvOutput.at(1), argvOutput.at(2)); // expect key value
	else
		success = false;
	
	return (success ? "1" : "0");
}
bool hp83711bDevice::deviceMain(int argc, char **argv)
{
	return false;
}
std::string hp83711bDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool hp83711bDevice::commandDevice(std::string command)
{
	std::string commandString;
	std::string result;
	commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 0";

	result = partnerDevice("gpibController").execute(commandString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	if(result.compare("1")==0)
		return true;
	else
		return false;
}