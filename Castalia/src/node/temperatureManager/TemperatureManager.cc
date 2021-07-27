/**********************************************************************************************
 *  Copyright (c) Federal Fluminence University (UFF), RJ-Brazil - 2020                       *
 *  Developed at the Multimedia Data Communication Research Laboratory (MidiaCom)			  *
 *  All rights reserved                                                                       *
 *                                                                                            *
 *  Permission to use, copy, modify, and distribute this protocol and its documentation for   *
 *  any purpose, without fee, and without written agreement is hereby granted, provided that  *
 *  the above copyright notice, and the author appear in all copies of this protocol.         *
 *                                                                                            *
 *  Author(s): E. Caballero                                                                   *
 *  Implementation: Egberto Caballero <egbertocr@midiacom.uff.br> 							  *
 **********************************************************************************************/


#include "TemperatureManager.h"

Define_Module(TemperatureManager);

void TemperatureManager::initialize()
{
	/*--- Configurables parameters ---*/

	/* These parameters are initialized at the values indicated in the simulation configuration file.
	 * If not specified in the configuration file, they are given the default value indicated in the corresponding .ned file */
	exportTemperatureHistory = par("exportTemperatureHistory");

	/* To export the temperature history, you must specify the path of the file where you want to export and,
	 * the name with which the file will be saved. If this information is not indicated in the simulation configuration file,
	 * the temperature history will not be generated. */
	folderPath = par("folderPath");
	nameFile = par("nameFile");

    bodyTemperatureReference = par("bodyTemperatureReference");  // (ºC)
    riseTemperatureLimit = par("riseTemperatureLimit");
    SAR = par("SAR");
    specificHeatOfTissue = par("specificHeatOfTissue");
	massDensity =  par("massDensity");
	delta_x = par("delta_x");
	delta_t = (double)par("delta_t")/1000.0;
	metabolicHeating = par("metabolicHeating");
	bloodPerfusionConstant = par("bloodPerfusionConstant");
	fixedBloodTemperature = par("fixedBloodTemperature");
	termalconductivity = par("termalconductivity");
	volumeOfSensor = par("volumeOfSensor");

	/* hysteresisFactor is a value that defines the limit value of the temperature that
	 * is considered for the node to return to the "NOT HEATED" state after having reached
	 * the "HEATED" state (temperature hysteresis value).
	 * This factor is multiplied by the maximum allowed temperature. In this way,
	 * the hysteresis value of the temperature is defined as a function of x percent of 
	 * the maximum allowed temperature.
	 * For hysteresisFactor, values between 0.95 and 0.99 are recommended.*/
	hysteresisFactor = par("hysteresisFactor");

	/*--- Others parameters for temperature estimation ---*/

	hysteresisTemperatureLimit = hysteresisFactor * riseTemperatureLimit;
	timeForCheckTemperature = delta_t;
	powerConsumption = 0; // inicial consumption is zero.
	temperature_last = bodyTemperatureReference;
    nodeHeatedWithHisteresis = false;
    nodeHeatedWithHisteresis = false;
	hysteresisTimeActivated = false;
	nodeHeatedWithoutHisteresis = false;
	sensorTemperature =0;
	counter = 0;
	

	/*--- Custom class parameters ---*/

	/* Get a valid references to the Resources Manager module and the
	 * Radio module, so that we can make direct calls to their public methods.*/
    radioModule = check_and_cast <Radio*>(getParentModule()->getSubmodule("Communication")->getSubmodule("Radio"));
    resMgrModule = check_and_cast <ResourceManager*>(getParentModule()->getSubmodule("ResourceManager"));

	nodenomber = getParentModule()->getIndex();
	setTimerDrift(resMgrModule->getCPUClockDrift());

    /* Starting Timer */
    setTimer(PERIODIC_TEMPERATURE_CALCULATION, timeForCheckTemperature);

}


void TemperatureManager::timerFiredCallback(int index){
	switch(index){
        case PERIODIC_TEMPERATURE_CALCULATION:
        {
            // Restarting the Timer.
            setTimer(PERIODIC_TEMPERATURE_CALCULATION, timeForCheckTemperature); 
            double Temperature_var = calculateTemperature();

			if(Temperature_var >= (riseTemperatureLimit + bodyTemperatureReference)){
                nodeHeatedWithHisteresis = true;
				hysteresisTimeActivated = true;
				nodeHeatedWithoutHisteresis = true;
				sensorTemperature = Temperature_var;
            }else if(hysteresisTimeActivated == true && Temperature_var < (riseTemperatureLimit + bodyTemperatureReference) && Temperature_var >= (hysteresisTemperatureLimit + bodyTemperatureReference)){
				nodeHeatedWithHisteresis = true;
				hysteresisTimeActivated = true;
				nodeHeatedWithoutHisteresis = false;
				sensorTemperature = Temperature_var;
			}else if(hysteresisTimeActivated == true && Temperature_var < (hysteresisTemperatureLimit + bodyTemperatureReference)){
                nodeHeatedWithHisteresis = false;
				hysteresisTimeActivated = false;
				sensorTemperature = Temperature_var;
            }else{
				sensorTemperature = Temperature_var;
			}
			exportTempHistory();
        }
            break;

        default:
            return;
	}
		return;
}


/* Function that calculates the sensor temperature using Pennes equation.*/
double TemperatureManager::calculateTemperature(){

	double a = 1;
	double b = (delta_t* bloodPerfusionConstant)/(massDensity* specificHeatOfTissue);
	double c = (4* delta_t* termalconductivity)/(massDensity* specificHeatOfTissue* (pow(delta_x,2)));
	double d = (delta_t* termalconductivity)/ (massDensity* specificHeatOfTissue* (pow(delta_x,2)));

	// ECR => T_Tb
	double T_Tb = b* fixedBloodTemperature; 

	// ECR => T_Qm
	double T_Qm = (delta_t* metabolicHeating)/(massDensity* specificHeatOfTissue);

	// ECR => T_SAR
	double T_SAR;
	double tx_Time = radioModule->getTxTimeInLastSlotTime(timeForCheckTemperature);	// Get Trasmission time in the last timeslot (timeslot-> 1 millisecond)
	if(tx_Time>0){
		T_SAR = delta_t * SAR/specificHeatOfTissue;
	} else{
		T_SAR = 0;
	}

	// ECR => T_Pc
	double T_Pc;
	double power_var = resMgrModule->getSpentEnergy();	// Get spent energy from ResourceManager module
	double deltaEnergy = power_var - powerConsumption;
	powerConsumption = power_var;
	double powerDissipationDensity = deltaEnergy/volumeOfSensor;
	T_Pc = (delta_x * powerDissipationDensity)/(massDensity*specificHeatOfTissue);
		
	// ECR => Pennes Equation
	double Temperature = a*temperature_last - b*temperature_last -c*temperature_last+ T_SAR + T_Tb + T_Pc + d*4*temperature_last;	// metabolicHeating (T_Qm) effect is not considered in this implementation 
	// double Temperature = a*temperature_last - b*temperature_last -c*temperature_last+ T_SAR + T_Tb + T_Pc + d*4*temperature_last +T_Qm; // Considering the metabolicHeating (T_Qm) effect

	temperature_last = Temperature;
	return Temperature;
}


void TemperatureManager::handleMessage(cMessage * msg) {
	int msgKind = msg->getKind();
	switch (msgKind) {

		case TIMER_SERVICE:{
			handleTimerMessage(msg);
			break;
		}

		default:{
			opp_error("Temperature module recieved unexpected message: [%s]", msg->getName());
		}
	}
	delete msg;
}


/* Function that returns the heat status of the sensor considering a histeresis process. 
 * "true" when the temperature is greater than or equal the allowed limit,
 * "false" when the temperature is less than the allowed limit.*/
bool TemperatureManager::isNodeHeatedWithHisteresis(){
	if(nodeHeatedWithHisteresis==true){
		return true;
	}else{
		return false;
	}
}

/* Function that returns the heat status of the sensor without considering the hysteresis process.
 * "true" when the temperature is greater than or equal the allowed limit,
 * "false" when the temperature is less than the temperature hysteresis value allowed limit.*/
bool TemperatureManager::isNodeHeatedWithoutHisteresis(){
	if(nodeHeatedWithoutHisteresis==true){
		return true;
	}else{
		return false;
	}
}



/* Function that returns the updated sensor temperature value.*/
double TemperatureManager::getSensorTemperature(){
	return sensorTemperature;
}


/* Function to generate e txt file with the temperature history.
 * Temperature will be printed each one second.
 * To export the temperature history it must be enabled by setting the variable
 * exportTemperatureHistory in the simulation configuration file (omnetpp.ini);
 * In addition the path file, in which it will be exported, must be specified. */
void TemperatureManager::exportTempHistory(){
	if(counter == (int)1/timeForCheckTemperature){
		if(exportTemperatureHistory==true){
			double time_var = simTime().dbl();
			double temp_rise_var = sensorTemperature - bodyTemperatureReference;
			string pathFile = string(folderPath)+"/"+string(nameFile)+"_node_"+std::to_string(nodenomber)+".txt";
			ofstream file;
			file.open(pathFile, std::ofstream::out | std::ofstream::app);
			file << time_var << "\t " << temp_rise_var << "\n";
			file.close();
		}
		
		if(nodeHeatedWithHisteresis==true){
			trace() << "NODE HEATED";
		}else{
			trace() << "NODE NOT HEATED";
		}
		
		counter=0;

	}else{
		counter ++;
	}
}