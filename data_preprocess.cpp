#include "data_preprocess.h"

using namespace std;

struct Trajectory {
	int road_id;
	int unixtimestamp;
	int speed;
	bool is_occupied;
	bool is_stop;
};


bool CountMapValue(QMap<int, QList<double>> &passing_time_map, QMap<int, QList<int>> &passing_count_map, QVector<Trajectory> traj_list)
{
	QVector<Trajectory>::iterator trj_iter = traj_list.begin();
	QVector<Trajectory>::iterator trj_iter_end = traj_list.end();

	QList<double> temp_list_time;
	QList<int> temp_list_count;

	int time_inv_len = 24 * 60 * 60 / 48;

	for (int i = 0; i < 48; i++)
	{
		temp_list_time.append(.0);
		temp_list_count.append(0);
	}

	int speed_zero_count = 0;
	while (trj_iter != trj_iter_end)
	{		
		if (trj_iter->speed > 0)
			speed_zero_count++;

		trj_iter++;
	}

	if (speed_zero_count == 0)
		return false;

	trj_iter = traj_list.begin();

	while (trj_iter != trj_iter_end)
	{
		if (trj_iter->is_stop )
		{
			trj_iter++;
			continue;
		}

		int road_id = trj_iter->road_id;
		if (!passing_time_map.contains(road_id))
		{
			passing_time_map.insert(road_id, temp_list_time);
			passing_count_map.insert(road_id, temp_list_count);
		}

		int time_inv_ind = double(QDateTime().fromSecsSinceEpoch(trj_iter->unixtimestamp).time().msecsSinceStartOfDay() / 1000) / time_inv_len + 0.5;
		if (time_inv_ind == 48)
			time_inv_ind--;

		passing_time_map[road_id][time_inv_ind] += trj_iter->speed;
		passing_count_map[road_id][time_inv_ind] ++;

		trj_iter++;
	}

	return true;
}


void OutputPassingSpeed(const char* infiledirname, const char* outfilename_cruising, const char* outfilename_occupied)
{
	QDir infiledir(infiledirname);
	QStringList namefileter;
	namefileter << "*.csv";
	infiledir.setNameFilters(namefileter);
	QFileInfoList infolist = infiledir.entryInfoList();

	int nInvNum = 48;

	QMap<int, QList<double>> passing_time_crs_map;
	QMap<int, QList<double>> passing_time_occ_map;

	QMap<int, QList<int>> passing_count_crs_map;
	QMap<int, QList<int>> passing_count_occ_map;
	
	QList<double> templist;
	for (int i = 0; i < nInvNum; i++)
		templist.append(.0);

	QVector<Trajectory> trj_list;
	for (int i = 0; i < infolist.size(); i++)
	{
		cout << "No. " << i << " of " << infolist.size() << endl;

		QFile infile(infolist[i].absoluteFilePath());
		if (infile.open(QIODevice::ReadOnly) == false)
		{
			throw "cannot open file";
		}

		bool is_occ = false;
		int last_time = -1;

		QTextStream in_str(&infile);
		QString smsg = in_str.readLine();
		while (in_str.atEnd() == false)
		{
			smsg = in_str.readLine();
			QStringList slist = smsg.split(',');
			
			Trajectory trj;
			trj.unixtimestamp = slist[1].toInt();
			trj.speed = slist[5].toInt();
			trj.is_occupied = slist[7].toInt() == 1;
			trj.is_stop = slist[8].toInt() == 1;
			trj.road_id = slist[10].toInt();

			if (last_time != -1 && (trj.unixtimestamp - last_time > 5 * 60 || is_occ != trj.is_occupied))
			{
				if (is_occ)
				{
					CountMapValue(passing_time_occ_map, passing_count_occ_map, trj_list);
				}
				else
				{
					CountMapValue(passing_time_crs_map, passing_count_crs_map, trj_list);
				}

				trj_list.clear();
			}

			is_occ = trj.is_occupied;
			last_time = trj.unixtimestamp;

			trj_list.append(trj);
		}

		infile.close();
	}

	cout << "outputting" << endl;

	foreach(int key, passing_time_occ_map.keys())
	{
		for (int i = 0; i < nInvNum; i++)
		{
			if (passing_count_occ_map[key][i] == 0)
				passing_time_occ_map[key][i] = 0;
			else
				passing_time_occ_map[key][i] /= passing_count_occ_map[key][i];
		}
	}

	foreach(int key, passing_time_crs_map.keys())
	{
		for (int i = 0; i < nInvNum; i++)
		{
			if (passing_count_crs_map[key][i] == 0)
				passing_time_crs_map[key][i] = 0;
			else
				passing_time_crs_map[key][i] /= passing_count_crs_map[key][i];
		}
	}


	// output
	QFile outfile_crs(outfilename_cruising);
	if (outfile_crs.open(QIODevice::WriteOnly) == false)
		throw "cannot open output file";
	
	QTextStream out_crs(&outfile_crs);
	out_crs << "FID";
	for (int i = 0; i < nInvNum; i++)
		out_crs << ",H" << i;
	out_crs << "\r\n";

	foreach(int key, passing_time_crs_map.keys())
	{
		out_crs << key;
		for (int i = 0; i < nInvNum; i++)
			out_crs << ',' << QString::number(passing_time_crs_map[key][i], 'g', 12);
		out_crs << "\r\n";
	}

	outfile_crs.close();


	QFile outfile_occ(outfilename_occupied);
	if (outfile_occ.open(QIODevice::WriteOnly) == false)
		throw "cannot open output file";

	QTextStream out_occ(&outfile_occ);
	out_occ << "FID";
	for (int i = 0; i < nInvNum; i++)
		out_occ << ",H" << i;
	out_occ << "\r\n";

	foreach(int key, passing_time_occ_map.keys())
	{
		out_occ << key;
		for (int i = 0; i < nInvNum; i++)
			out_occ << ',' << QString::number(passing_time_occ_map[key][i], 'g', 12);
		out_occ << "\r\n";
	}

	outfile_occ.close();

}

void OutputPickupProbabilities(const char* cruising_count_filename, const char* pickup_count_filename, const char* outfilename)
{
	QFile cruising_file(cruising_count_filename);
	if (cruising_file.open(QIODevice::ReadOnly) == false)
		throw "cannot open cruising count file";

	QFile pickup_file(pickup_count_filename);
	if (pickup_file.open(QIODevice::ReadOnly) == false)
		throw "cannot open pick up count file";

	QFile outfile(outfilename);
	if (outfile.open(QIODevice::WriteOnly) == false)
		throw "cannot open output file";

	QMap<int, QList<double>> cruising_count_map;
	QMap<int, QList<double>> pickup_count_map;
	QMap<int, QList<double>> pickup_pro_map;
	
	QList<double> temp_list;
	for (int i = 0; i < 48; i++)
		temp_list.push_back(.0);

	QTextStream in_cruising(&cruising_file);
	QString smsg = in_cruising.readLine();

	while (in_cruising.atEnd() == false)
	{
		smsg = in_cruising.readLine();
		QStringList slist = smsg.split(',');

		int road_id = slist[0].toInt();

		if (cruising_count_map.find(road_id) == cruising_count_map.end())
			cruising_count_map.insert(road_id, temp_list);

		for (int i = 1; i < 49; i++)
			cruising_count_map[road_id][i - 1] = slist[i].toDouble();
	}

	QTextStream in_pick(&pickup_file);
	smsg = in_pick.readLine();
	while (in_pick.atEnd() == false)
	{
		smsg = in_pick.readLine();
		QStringList slist = smsg.split(',');

		int road_id = slist[0].toInt();

		if (pickup_count_map.find(road_id) == pickup_count_map.end())
			pickup_count_map.insert(road_id, temp_list);

		for (int i = 1; i < 49; i++)
			pickup_count_map[road_id][i - 1] = slist[i].toDouble();
	}

	foreach(int road_id, pickup_count_map.keys())
	{
		if (pickup_pro_map.find(road_id) == pickup_pro_map.end())
			pickup_pro_map.insert(road_id, temp_list);

		if (cruising_count_map.find(road_id) == cruising_count_map.end())
		{
			std::cout << "A cruising count is NULL" << std::endl;
			continue;
		}

		for (int i = 0; i < 48; i++)
		{
			if (pickup_count_map[road_id][i] < 1e-3)
				pickup_pro_map[road_id][i] = 0;
			else if (cruising_count_map[road_id][i] < 1e-3)
			{
				std::cout << "zero divided at " << road_id << " time: " << i << std::endl;
				pickup_pro_map[road_id][i] = 0.9;
			}
			else
				pickup_pro_map[road_id][i] = pickup_count_map[road_id][i] / cruising_count_map[road_id][i];

			if (pickup_pro_map[road_id][i] >= 1.0)
			{
				std::cout << "invalid pro" << std::endl;
				pickup_pro_map[road_id][i] = .95;
			}

		}
	}

	QTextStream out_str(&outfile);
	out_str << "FID";
	for (int i = 0; i < 48; i++)
		out_str << ",H" << i;
	out_str << "\r\n";

	foreach(int key, pickup_pro_map.keys())
	{
		out_str << key;
		for (int i = 0; i < 48; i++)
			out_str << ',' << QString::number(pickup_pro_map[key][i], 'g', 12);
		out_str << "\r\n";
		out_str.flush();
	}

	outfile.close();
	pickup_file.close();
	cruising_file.close();
}

bool OutputODTuples(const char* infilefilename, const char* ref_shape_filename, const char* outfilename)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	CPLSetConfigOption("SHAPE_ENCODING", "");

	/* Load road network from shape file*/
	// open road net shape file
	cout << "Loading road net file" << endl;
	GDALDataset* poRNDataset = static_cast<GDALDataset*>(GDALOpenEx(ref_shape_filename, GDAL_OF_VECTOR, NULL, NULL, NULL));
	if (poRNDataset == NULL)
	{
		cout << "Open RoadNet \'" << ref_shape_filename << "\' failed !" << endl;
		return false;
	} // end if

	  // get layer of road , polyline feature
	int nLayerCount = poRNDataset->GetLayerCount();
	//cout << "The num of layer in roadnet shape file is " << nLayerCount << endl;
	OGRLayer *pRNLayer = poRNDataset->GetLayer(0);  /// get layer 
													//cout << "Road Net layer name is: \'" << pRNLayer->GetName() << "\'" << endl;

	pRNLayer->ResetReading();	/// ensure starting at the beginning of the layer

	int nRNCount = pRNLayer->GetFeatureCount();
	cout << "Number of roads is: " << nRNCount << endl;

	OGRSpatialReference* pSpatialRef = pRNLayer->GetSpatialRef();
	if (pSpatialRef->IsProjected() == false)
	{
		cout << "the spatial ref of input road net shape file is not projected coordinate system" << endl;
		GDALClose(poRNDataset);
		return false;
	}

	// create coordinate transformation
	OGRSpatialReference wgsRef;
	if (wgsRef.SetWellKnownGeogCS("WGS84") != OGRERR_NONE)
	{
		cout << "Set Wgs84 geo coor failed" << endl;
		GDALClose(poRNDataset);
		return false;
	}
	OGRCoordinateTransformation *pCorTransf = OGRCreateCoordinateTransformation(&wgsRef, pSpatialRef);
	if (pCorTransf == NULL)
	{
		cout << "creation of coordinate transformation failed" << endl;
		GDALClose(poRNDataset);
		return false;
	}


	QFile infile(infilefilename);
	if (infile.open(QIODevice::ReadOnly) == false)
	{
		cout << "cannot open od tuple file" << endl;
		return false;
	}
	
	QFile outfile(outfilename);
	if (outfile.open(QIODevice::WriteOnly) == false)
	{
		cout << "cannot open outfile" << endl;
		return false;
	}

	QTextStream _out(&outfile);
	_out << "road_id_o, road_id_d, occur_time(s), time_duration(min), length(m)\r\n";

	QTextStream _in(&infile);
	QString smsg = _in.readLine();

	while (!_in.atEnd())
	{
		smsg = _in.readLine();
		QStringList slist = smsg.split(',');

		int unixtime = slist[3].toInt();

		if ((unixtime < 1457971200 || unixtime > 1458316800) && (unixtime < 1458576000 || unixtime > 1458835200))
			continue;

		double y_o = slist[5].toDouble();
		double x_o = slist[6].toDouble();
		double y_d = slist[7].toDouble();
		double x_d = slist[8].toDouble();

		if (pCorTransf->Transform(1, &x_o, &y_o, NULL) == FALSE || pCorTransf->Transform(1, &x_d, &y_d, NULL) == FALSE)
			continue;

		OGRPoint point_o(x_o, y_o);
		OGRPoint point_d(x_d, y_d);

		point_o.assignSpatialReference(pSpatialRef);
		point_d.assignSpatialReference(pSpatialRef);
		OGRGeometry *pBuf_o = point_o.Buffer(130);
		OGRGeometry *pBuf_d = point_d.Buffer(130);

		if (pBuf_o == NULL)
		{
			delete pBuf_d;
			continue;
		}

		if (pBuf_d == NULL)
		{
			delete pBuf_o;
			continue;
		}

		pRNLayer->SetSpatialFilter(pBuf_o);
		if (pRNLayer->GetFeatureCount() == 0)
		{
			delete pBuf_d;
			delete pBuf_o;
			continue;
		}

		int road_id_o = -1;
		int min_distance = 99999;
		OGRFeature *p_road = NULL;
		while ((p_road = pRNLayer->GetNextFeature()) != NULL)
		{
			double distance_to_road = point_o.Distance(p_road->GetGeometryRef());
			if (distance_to_road < min_distance)
			{
				min_distance = distance_to_road;
				road_id_o = p_road->GetFID();
//				cout << road_id_o << endl;
			}

			OGRFeature::DestroyFeature(p_road);
		}

		pRNLayer->ResetReading();
		pRNLayer->SetSpatialFilter(pBuf_d);
		if (pRNLayer->GetFeatureCount() == 0)
		{
			delete pBuf_d;
			delete pBuf_o;
			continue;
		}

		
		int road_id_d = -1;
		min_distance = 99999;
		while ((p_road = pRNLayer->GetNextFeature()) != NULL)
		{
			double distance_to_road = point_d.Distance(p_road->GetGeometryRef());
			if (distance_to_road < min_distance)
			{
				min_distance = distance_to_road;
				road_id_d = p_road->GetFID();
			}

			OGRFeature::DestroyFeature(p_road);
		}


		delete pBuf_o;
		delete pBuf_d;

		_out << road_id_o << ',' 
			<< road_id_d << ',' 
			<< QDateTime().fromSecsSinceEpoch(unixtime).time().msecsSinceStartOfDay() / 1000 << ',' 
			<< slist[9] << ',' 
			<< slist[10] << "\r\n";
		_out.flush();
	}


	infile.close();
	outfile.close();
	
	// destroy coordinate transformation
	OGRCoordinateTransformation::DestroyCT(pCorTransf);

	GDALClose(poRNDataset);

	return true;
}
