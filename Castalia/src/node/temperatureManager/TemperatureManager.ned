//********************************************************************************
//*  Copyright:                                         						 *
//*                                                                              *
//*******************************************************************************/

package node.temperatureManager;

simple TemperatureManager {
	parameters:
		
		bool collectTraceInfo = default (false);
		bool exportTemperatureHistory=  default (false);
		string folderPath = default ("");
		string nameFile = default ("");

		double hysteresisFactor = default(0.99);  		// (ºC)
		double bodyTemperatureReference = default(37);  // (ºC)
		double riseTemperatureLimit = default(0.1); 	// (ºC)
		double SAR = default(0.8402);					// (W/kg)
		double specificHeatOfTissue = default(3600); 	// (W/kgºC)

		double massDensity = default(1050);				// (kg/m3)
		double delta_x = default(0.005);				// (m3)
		double delta_t = default(1);					// (ms) one second (maximum)
		
		double metabolicHeating = default(420);			// (W/m3)  for muscle
		double bloodPerfusionConstant = default(2700);	// (J/m3sºC)
		double fixedBloodTemperature = default(37);		// (ºC)
		double termalconductivity = default(0.498);		// (J/m3sºC)
		double volumeOfSensor = default(0.000157);		// (m3) for a cilindric sensor, r=5mm e h=5mm


	gates:
		output toNetwork;
}