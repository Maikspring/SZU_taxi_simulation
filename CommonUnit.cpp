#include "CommonUnit.h"


GoingRoad GetRandomRoadInVector(std::vector<Road>* p_road_vec, int node, int exclude_road_id)
{
	int vec_size = p_road_vec->size();
	if (vec_size == 0)
	{
		std::cout << "node is " << node << std::endl;
		throw "ERROR in RoadCondition: road connection invalid";
	}
	if (vec_size == 1)
		return GoingRoad((*p_road_vec)[0], (*p_road_vec)[0].node_start == node, 0);
	
	int random_road_id = exclude_road_id;
	int road_ind = 0;
	int random_count = 0;
	while (random_road_id == exclude_road_id)
	{
		random_count++;

		road_ind = rand() % vec_size;
		random_road_id = (*p_road_vec)[road_ind].id;
	}
	
	return GoingRoad((*p_road_vec)[road_ind], (*p_road_vec)[road_ind].node_start == node, 0);
}


RoadCondition::RoadCondition()
{
	time_inv_num_ = 0;
	time_inv_len_ = 0;
}

int RoadCondition::GetWhichTimeInv(int current_time)
{
	if (current_time > DAY_LENGTH)
		throw "ERROR in PassengerDistribution: current_time beyond the length of a day!";
	int ind = (double(current_time) / time_inv_len_) + .5;
	return ind >= time_inv_num_ ? time_inv_num_ - 1 : ind;
}

Road RoadCondition::get_road(int road_id)
{
	return id_to_road_map_[road_id];
}

void RoadCondition::set_time_inv_num(int time_inv_num)
{
	if (time_inv_num <= 0)
		throw "ERROR in RoadCondition: time_inv_num is invalid";
	time_inv_num_ = time_inv_num;
	time_inv_len_ = DAY_LENGTH / time_inv_num_;
}

void RoadCondition::LoadRoadConnectionFromFile(const char* road_connection_filename)
{
	id_to_road_map_.clear();
	node_to_road_map_.clear();

	CsvReader reader(road_connection_filename);
	reader.set_is_with_header(true);

	while (reader.toNextLine() != false)
	{
		Road rd;
		rd.id = reader.GetCurrentLineElement(0).toInt();
		rd.node_start = reader.GetCurrentLineElement(1).toInt();
		rd.node_end = reader.GetCurrentLineElement(2).toInt();
		//rd.is_bi_direct = reader.GetCurrentLineElement(3).toInt() == 1;
		rd.is_bi_direct = true;
		rd.length = reader.GetCurrentLineElement(4).toDouble();

		id_to_road_map_.insert(std::pair<int, Road>(rd.id, rd));
	}

	std::map<int, Road>::iterator iter_map = id_to_road_map_.begin();
	while (iter_map != id_to_road_map_.end())
	{
		int node_start = iter_map->second.node_start;
		int node_end = iter_map->second.node_end;
		int road_id = iter_map->first;


		if (node_to_road_map_.find(node_start) == node_to_road_map_.end())
		{
			node_to_road_map_.insert(
				std::pair<int, std::vector<Road>>(node_start, std::vector<Road>()));
		}
		node_to_road_map_[node_start].push_back(iter_map->second);

		if (iter_map->second.is_bi_direct)
		{
			if (node_to_road_map_.find(node_end) == node_to_road_map_.end())
			{
				node_to_road_map_.insert(
					std::pair<int, std::vector<Road>>(node_end, std::vector<Road>()));
			}
			node_to_road_map_[node_end].push_back(iter_map->second);
		}

		iter_map++;
	}

	
	__CheckNodesToRoadMap();
}

void RoadCondition::LoadRoadCruinsingPassingTimeFromFile(const char* road_passing_time_filename)
{
	road_cruising_passing_time_vec_.clear();

	CsvReader reader(road_passing_time_filename);
	reader.set_is_with_header(true);

	if (reader.get_column_num() - 1 != time_inv_num_)
		throw "ERROR in RoadCondition loading cruising passing time: time_inv_num is not equal to the columns input file";

	for (int i = 0; i < time_inv_num_; i++)
	{
		std::map<int, float> temp_map;
		road_cruising_passing_time_vec_.push_back(temp_map);
	}

	while (reader.toNextLine())
	{
		int road_id = reader.GetCurrentLineElement(0).toInt();
		for (int i = 1; i < time_inv_num_ + 1; i++)
		{
			float passing_time = reader.GetCurrentLineElement(i).toDouble();
			road_cruising_passing_time_vec_[i - 1].insert(std::pair<int, float>(road_id, passing_time));
		}
	}
}

void RoadCondition::LoadRoadOccupiedPassingTimeFromFile(const char* road_passing_time_filename)
{
	road_occupied_passing_time_vec_.clear();

	CsvReader reader(road_passing_time_filename);
	reader.set_is_with_header(true);

	if (reader.get_column_num() - 1 != time_inv_num_)
		throw "ERROR in RoadCondition in loading occupied passing time: time_inv_num is not equal to the columns input file";

	for (int i = 0; i < time_inv_num_; i++)
	{
		std::map<int, float> temp_map;
		road_occupied_passing_time_vec_.push_back(temp_map);
	}

	while (reader.toNextLine())
	{
		int road_id = reader.GetCurrentLineElement(0).toInt();
		for (int i = 1; i < time_inv_num_ + 1; i++)
		{
			float passing_time = reader.GetCurrentLineElement(i).toDouble();
			road_occupied_passing_time_vec_[i - 1].insert(std::pair<int, float>(road_id, passing_time));
		}
	}
}

void RoadCondition::__CheckNodesToRoadMap()
{
	// 1. in case of no road to choose
	std::map<int, Road>::iterator iter_map = id_to_road_map_.begin();
	while (iter_map != id_to_road_map_.end())
	{
		int node_start = iter_map->second.node_start;
		int node_end   = iter_map->second.node_end;
		
		if (node_to_road_map_[node_start].size() == 0)
		{
			iter_map->second.is_bi_direct = true;
			node_to_road_map_[node_start].push_back(iter_map->second);
		}

		if (node_to_road_map_[node_end].size() == 0)
		{
			iter_map->second.is_bi_direct = true;
			node_to_road_map_[node_end].push_back(iter_map->second);
		}

		iter_map++;
	}
	


	// 2. in case of island
	

}

GoingRoad RoadCondition::GetRandomNextRoad(GoingRoad going_road)
{
	GoingRoad rd;
	////////////
	if (going_road.road.node_start == -1 || going_road.road.node_end == -1)
		throw "ERROR in RoadCondition: cannot get random going road from invalid nodes";

	if (going_road.directional)  // from node_start to node_end
	{
		std::vector<Road>* p_road_vec = &node_to_road_map_[going_road.road.node_end];
		rd = GetRandomRoadInVector(p_road_vec, going_road.road.node_end, going_road.road.id);
	}
	else
	{
		std::vector<Road>* p_road_vec = &node_to_road_map_[going_road.road.node_start];
		rd = GetRandomRoadInVector(p_road_vec, going_road.road.node_start, going_road.road.id);
	}

	return rd;
}

GoingRoad RoadCondition::GetRandomGoingRoad()
{
	int road_num = id_to_road_map_.size();
	if (road_num == 0)
		throw "ERROR in RoadCondition: road data has not been loaded";

	int random_index = rand() % road_num;
	GoingRoad gr(id_to_road_map_[random_index], rand() % 2 == 0, 0);

	return gr;

}

float RoadCondition::GetRoadCruisingPassingTime(int road_id, int current_time)
{
	int time_inv = GetWhichTimeInv(current_time);
	
	float passing_time = road_cruising_passing_time_vec_[time_inv][road_id];
	if (passing_time < 1e-1)	// if the passing time is miss, return a passing time at speed 40km/h
	{
		float road_len = id_to_road_map_[road_id].length;	// meters
		passing_time = road_len / (40.0 / 3.6);
	}

	if (passing_time < 1.0)
		return 1.0;
	return passing_time;
}

float RoadCondition::GetRoadOccupiedPassingTime(int road_id, int current_time)
{
	int time_inv = GetWhichTimeInv(current_time);
	return road_occupied_passing_time_vec_[time_inv][road_id];
}

PassengersDistribution::PassengersDistribution(	const char* pro_filename)
{
	LoadProDisFromFile(pro_filename);
}

PassengersDistribution::PassengersDistribution()
{
	time_inv_num_pro_ = -1;
	time_inv_len_pro_ = -1;
}

PassengersDistribution::~PassengersDistribution()
{
	for (int i = 0; i < time_inv_num_pro_; i++)
		probability_distribution_time_map_[i].clear();
	probability_distribution_time_map_.clear();
}

// UNFINISHED
bool PassengersDistribution::FindPassenger(int road_id, int current_time, ODTuple &od_tuple)
{
	double pro = GetProbabilityOfOccupiedInTimeOfRoad(road_id, current_time);
	double pro_get = double(rand()) / RAND_MAX;

	if (pro_get > pro)
	{
		int time_inv_od = GetWhichTimeInvInOD(current_time);

		std::vector<ODTuple>* p_od_tuples_vec = GetAllODTuple(road_id, time_inv_num_od_);
		if (p_od_tuples_vec == NULL)
			return false;

		int num_od_tuples = p_od_tuples_vec->size();

		if(num_od_tuples != 0) // no od tuple in current time slice
		{
			int od_ind = rand() % num_od_tuples;
			od_tuple = (*p_od_tuples_vec)[od_ind];
			return true;
		}
	}

	return false;
}

void PassengersDistribution::set_time_inv_num_pro(int time_inv_num)
{
	if (time_inv_num <= 0)
		throw "ERROR in PassengerDistribution: time_inv_num is invalid";
	time_inv_num_pro_ = time_inv_num;
	time_inv_len_pro_ = DAY_LENGTH / time_inv_num_pro_;
}

void PassengersDistribution::set_time_inv_num_od(int time_inv_num)
{
	if (time_inv_num <= 0)
		throw "ERROR in PassengerDistribution: time_inv_num is invalid";
	time_inv_num_od_ = time_inv_num;
	time_inv_len_od_ = DAY_LENGTH / time_inv_num_od_;
}


void PassengersDistribution::LoadODTuplesFromFile(const char* od_tuples_filename)
{
	QFile infile(od_tuples_filename);
	if (infile.open(QIODevice::ReadOnly) == false)
	{
		throw "ERROR in PassengerDistribution: Cannot open od tuples file";
	}

	QTextStream in_strm(&infile);
	QString smsg = in_strm.readLine();
	QStringList slist = smsg.split(',');

	set_time_inv_num_od(slist.size() - 1);

	std::vector<std::vector<ODTuple>> templist;
	std::vector<ODTuple> templist_od;
	for (int i = 0; i < time_inv_num_od_ ; i++)
		templist.push_back(templist_od);


	while (in_strm.atEnd() == false)
	{
		smsg = in_strm.readLine();
		slist = smsg.split(',');

		ODTuple od_tuple;
		od_tuple.road_id_o = slist[0].toInt();
		od_tuple.road_id_d = slist[1].toInt();
		od_tuple.occur_time = slist[2].toInt();
		od_tuple.time_duration = slist[3].toDouble() * 60;
		od_tuple.path_length = slist[4].toDouble();

		if(od_tuple.time_duration < 1.0 || od_tuple.time_duration > 3*60*60)
			continue;

		if(od_tuple.path_length < 100.0 || od_tuple.path_length > 300000)
			continue;

		if (od_tuple.path_length / od_tuple.time_duration > 160 / 3.6)
			continue;


		int time_inv_ind = GetWhichTimeInvInOD(od_tuple.occur_time);
		
		if (o_road_to_od_tuples_map_.find(od_tuple.road_id_o) == o_road_to_od_tuples_map_.end())
			o_road_to_od_tuples_map_.insert(
				std::pair<int, std::vector<std::vector<ODTuple>>>(od_tuple.road_id_o, templist));

		o_road_to_od_tuples_map_[od_tuple.road_id_o][time_inv_ind].push_back(od_tuple);
	}

	infile.close();
}

float PassengersDistribution::GetProbabilityOfOccupiedInTimeOfRoad(int road_id, int current_time)
{
	int which_time_inv = GetWhichTimeInvInPro(current_time);
	if (probability_distribution_time_map_[which_time_inv].find(road_id) == probability_distribution_time_map_[which_time_inv].end())
		return 0.05;
	return probability_distribution_time_map_[which_time_inv][road_id];
}

int PassengersDistribution::GetWhichTimeInvInPro(int current_time)
{
	if (current_time > 24 * 60 * 60)
		throw "ERROR in PassengerDistribution: current_time beyond the length of a day!";
	int ind = (double(current_time) / time_inv_len_pro_) + .5;
	return ind >= time_inv_num_pro_ ? time_inv_num_pro_ - 1 : ind;
}

int PassengersDistribution::GetWhichTimeInvInOD(int current_time)
{
	if (current_time > 24 * 60 * 60)
		throw "ERROR in PassengerDistribution: current_time beyond the length of a day!";
	int ind = (double(current_time) / time_inv_len_od_) + .5;
	return ind >= time_inv_num_od_ ? time_inv_num_od_ - 1 : ind;
}

// UNFINISHED
std::vector<ODTuple>* PassengersDistribution::GetAllODTuple(int road_id, int time_inv_ind)
{
	if(o_road_to_od_tuples_map_.find(road_id) == o_road_to_od_tuples_map_.end())
		return NULL;

	for (int i = 0; i < time_inv_num_od_; i++)
	{
		if (o_road_to_od_tuples_map_[road_id][(time_inv_ind+i)%time_inv_num_od_].size() != 0)
		{
			return &o_road_to_od_tuples_map_[road_id][(time_inv_ind + i) % time_inv_num_od_];
		}
	}

	return NULL;
}

void PassengersDistribution::LoadProDisFromFile(const char* pro_dis_filename)
{
	probability_distribution_time_map_.clear();
	
	CsvReader reader(pro_dis_filename);
	reader.set_is_with_header(true);

	set_time_inv_num_pro(reader.get_column_num() - 1);

	for (int i = 0; i < time_inv_num_pro_; i++)
	{
		std::map<int, float> temp_map;
		probability_distribution_time_map_.push_back(temp_map);
	}

	while (reader.toNextLine())
	{
		int road_id = reader.GetCurrentLineElement(0).toInt();
		for (int i = 1; i < time_inv_num_pro_ + 1; i++)
		{
			float pro = reader.GetCurrentLineElement(i).toDouble();
			probability_distribution_time_map_[i - 1].insert(std::pair<int, float>(road_id, pro));
		}
	}
}

// UNFINISHED
TaxiPathExperience::TaxiPathExperience()
{

}

// UNFINISHED
TaxiPathExperience::~TaxiPathExperience()
{

}

// UNFINISHEDw
bool TaxiPathExperience::GetCruisingPath(GoingRoad &current_going_road, std::list<GoingRoad> &going_road_list, int current_time)
{
	///////////////////
	// Plan A: return false
	return false;
}

// Plan A implement
bool TaxiPathExperience::GetOccupiedPath(GoingRoad &current_going_road, ODTuple od_tuple, std::list<GoingRoad> &going_road_list, int current_time)
{


	return true;
}
