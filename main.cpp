#include <iostream>

#include "data_preprocess.h"
#include "Simulator.h"

using namespace std;


void TestSimulation();
void TestCsvReader();

int main()
{
	cout << "** Taxi Simulation **" << endl;
	cout << "** Author: Mai Ke" << endl;
	cout << "** E-Mail: maikspring@foxmail.com" << endl;
	
	srand((unsigned)time(NULL));

	try
	{
// 		OutputPassingTime("J:\\test_can_de",
// 			"J:\\Workspace\\taxi_simulation\\PlanA\\passing_speed_cruising.csv", 
// 			"J:\\Workspace\\taxi_simulation\\PlanA\\passing_speed_occupied.csv");

//  		OutputODTuples("J:\\Workspace\\shenzhen_taxi\\2016\\OD_tuples_filted.csv",
//  			"J:\\Workspace\\shenzhen_fundamentalchuzu\\road\\OSM_Roads_final_UTM_copy.shp",
//  			"J:\\Workspace\\taxi_simulation\\PlanA\\od_tuples_filted.csv");
		//TestCsvReader();
		TestSimulation();
	}
	catch (const char* excep_smsg)
	{
		cout << excep_smsg << endl;
		system("pause");
		return -1;
	}

	system("pause");
	return 0;
}

void TestSimulation()
{
	try
	{
		SimulationControler simulator;
		
		simulator.set_road_condition_time_inv_num(48);
		simulator.set_start_time(32400);		// 10:00, seconds since the start of day(00:00)
		simulator.set_time_inv_length(15);	// 15 seconds
		simulator.addOrdTaxies(15000);		// 100 ordinary taxies

		cout << "loading roads" << endl;
 		 simulator.LoadRoadConnetion("J:\\Workspace\\taxi_simulation\\PlanA\\road_condition.csv");
		cout << "loading cruising passing time" << endl;
		 simulator.LoadCruisingPassingTime("J:\\Workspace\\taxi_simulation\\PlanA\\passing_time_cruising_adjusted.csv");
		cout << "loading od tuples" << endl;
		 simulator.LoadODTuples("J:\\Workspace\\taxi_simulation\\PlanA\\od_tuples_filted.csv");
		cout << "loading pick up probabilities" << endl;
		 simulator.LoadPickupProbabilites("J:\\Workspace\\taxi_simulation\\PlanA\\pickup_probabilities.csv");

		simulator.setOrdTaxiLogStrm("J:\\Workspace\\taxi_simulation\\PlanA\\output\\taxi_log_no_sigle_way_10000_3.csv");	

		cout << "initializing" << endl;
		simulator.initialization();

		cout << "begin simulation: " << endl;
		int one_day_inv_num = 2400;//24 * 60 * 60 / 15;	// how much intervals in 1 day
		for (int i = 0; i < one_day_inv_num; i++)
		{
			cout << "no. " << i << " of " << one_day_inv_num << endl;
			simulator.toNextTime();
		}
		
		cout << "outputting statistics" << endl;
		simulator.OutputFullStatisticsOrdTaxi("J:\\Workspace\\taxi_simulation\\PlanA\\output\\statistic_no_single_way_10000_3.csv");
	}
	catch (const char* excep_smsg)
	{
		cout << excep_smsg << endl;
		system("pause");
	}
}

void TestCsvReader()
{
	CsvReader reader("J:\\Workspace\\taxi_simulation\\PlanA\\road_condition.csv");
	reader.set_is_with_header(true);

	cout << "num of cols is " << reader.get_column_num() << endl;
	
	QMap<int, Road> id_road_map;

	while (reader.toNextLine() != false)
	{
		Road rd;
		rd.id = reader.GetCurrentLineElement(0).toInt();
		rd.node_start = reader.GetCurrentLineElement(1).toInt();
		rd.node_start = reader.GetCurrentLineElement(2).toInt();
		rd.is_bi_direct = reader.GetCurrentLineElement(3).toInt() == 1;
		rd.length = reader.GetCurrentLineElement(4).toDouble();

		id_road_map.insert(rd.id, rd);
	}


	reader.SetFile("J:\\Workspace\\taxi_simulation\\PlanA\\passing_speed_cruising_km_h.csv");
	reader.set_is_with_header(true);
	int time_inv_num = reader.get_column_num() - 1;
	
	QFile outfile("J:\\Workspace\\taxi_simulation\\PlanA\\passing_time_cruising_adjusted.csv");
	if (outfile.open(QIODevice::WriteOnly) == false)
	{
		cout << "open outfile failed" << endl;
		return;
	}
	QTextStream _out(&outfile);

	_out << "road_id";
	for (int i = 0; i < time_inv_num; i++)
		_out <<",H" << i;
	_out << "\r\n";

	while (reader.toNextLine() != false)
	{
		int road_id = reader.GetCurrentLineElement(0).toInt();
		_out << road_id;
		for (int i = 0; i < time_inv_num; i++)
		{
			double speed = reader.GetCurrentLineElement(i + 1).toDouble();
			if (speed < 1e-10)
				speed = 3.6;
//  			if (speed < 15)
//  				speed = 15;

			double passing_time = id_road_map[road_id].length *3.6 / speed;
			if (passing_time > 1200)
				passing_time = 1200;

			_out << ',' << QString::number(passing_time, 'g', 12);
		}
		_out << "\r\n";
		_out.flush();
	}

	
	outfile.close();
	reader.close();
}