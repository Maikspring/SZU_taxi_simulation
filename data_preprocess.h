#pragma once

#include "csv_reader.h"

#include <iostream>

#include "ogrsf_frmts.h"
#include "gdal_priv.h"

// void OutputRoadCondition(const char* infilename, const char* outfilename);

void OutputPassingSpeed(const char* infiledirname, 
						const char* outfilename_cruising, 
						const char* outfilename_occupied);

void OutputPickupProbabilities(const char* cruising_count_filename,
								const char* pickup_count_filename,
								const char* outfilename);

// Plan A
bool OutputODTuples(const char* infilename, const char* ref_shape_filename, const char* outfilename);

void OutputCruisingPath(const char* infilename, const char* outfilename);