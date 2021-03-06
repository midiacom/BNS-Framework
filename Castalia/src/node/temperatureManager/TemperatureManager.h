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


#ifndef _TEMPERATUREMANAGER_H_
#define _TEMPERATUREMANAGER_H_

#include <map>
#include "CastaliaModule.h"
#include "TimerService.h"
#include "Radio.h"
#include "ResourceManager.h"
#include "TemperatureManagerMessage_m.h"

using namespace std;

enum timerIndex {
	PERIODIC_TEMPERATURE_CALCULATION = 0,
};

class TemperatureManager: public CastaliaModule, public TimerService {
 private:
	// parameters and variables

	/*--- The .ned file's parameters ---*/
	double bodyTemperatureReference;
	double riseTemperatureLimit;
	double SAR;
	double specificHeatOfTissue;
	double massDensity;
	double delta_x;
	double delta_t;
	double metabolicHeating;
	double bloodPerfusionConstant;
	double fixedBloodTemperature;
	double termalconductivity;
	double volumeOfSensor;
	double hysteresisFactor;
	bool exportTemperatureHistory;
	const char *folderPath;
	const char *nameFile;

	/*--- Others parameters for temperature estimation ---*/
	double hysteresisTemperatureLimit;
	bool nodeHeatedWithHisteresis;
	bool hysteresisTimeActivated;
	bool nodeHeatedWithoutHisteresis;
	double timeForCheckTemperature;
	double sensorTemperature; 
	double powerConsumption;
	double temperature_last;
	int counter;

	/*--- Custom class parameters ---*/
	Radio *radioModule;
	ResourceManager *resMgrModule;
	int nodenomber;

 protected:
	virtual void initialize();
	void timerFiredCallback(int index);
	double calculateTemperature();
	virtual void handleMessage(cMessage * msg);
	
 public:
	bool isNodeHeatedWithHisteresis();
	bool isNodeHeatedWithoutHisteresis();
	double getSensorTemperature();
	void exportTempHistory();

};

#endif	