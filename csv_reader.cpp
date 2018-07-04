#include "csv_reader.h"

CsvReader::CsvReader(QString filename)
{
	SetFile(filename);
}

CsvReader::CsvReader(QString filename, bool is_with_header)
{
	SetFile(filename);
	is_with_header_ = is_with_header;
}

CsvReader::CsvReader()
{
	is_all_set_ = false;
	is_with_header_ = true;
	columns_num_ = 0;
	cursor_line_num_ = 0;
}

CsvReader::~CsvReader()
{
	close();
}

void CsvReader::set_is_with_header(bool is_with_header)
{
	is_with_header_ = is_with_header;
}

void CsvReader::SetFile(QString filename)
{
	if (is_all_set_ == true)
		infile_.close();

	filename_ = filename;

	infile_.setFileName(filename_);
	if (infile_.open(QIODevice::ReadOnly) == false)
		throw "ERROR in CsvReader: cannot open given file in QIODevice::ReadOnly mode!";

	in_txt_stream_.setDevice(&infile_);

	QString smsg = in_txt_stream_.readLine();
	slist = smsg.split(',', QString::KeepEmptyParts);
	
	columns_num_ = slist.size();

	in_txt_stream_.seek(0);
	cursor_line_num_ = 0;

	is_all_set_ = true;
}

void CsvReader::close()
{
	if (!is_all_set_)
	{
		return;
	}

	infile_.close();
	columns_num_ = 0;

	is_all_set_ = false;
	is_with_header_ = true;
}

int CsvReader::get_cursor_line_num()
{
	return cursor_line_num_;
}

bool CsvReader::toNextLine()
{
	if (!is_all_set_)
		throw "ERROR in CsvReader: target file not set";

	if (is_with_header_)
		__SkipHeader();

	if (eof())
		return false;

	QString smsg = in_txt_stream_.readLine();
	slist = smsg.split(',', QString::KeepEmptyParts);

	if (slist.size() != columns_num_)
	{
		throw "ERROR in CsvReader: the size of a line is not equal to the number of columns";
	}

	cursor_line_num_++;
	return true;
}

int CsvReader::get_column_num()
{
	if (!is_all_set_)
		throw "ERROR in CsvReader: target file not set";

	return columns_num_;
}

bool CsvReader::eof()
{
	if (!is_all_set_)
		throw "ERROR in CsvReader: target file not set";

	return in_txt_stream_.atEnd();
}

QStringList CsvReader::readLine()
{
	if (!is_all_set_)
		throw "ERROR in CsvReader: target file not set";

	if (is_with_header_)
		__SkipHeader();

	if (eof())
		throw "ERROR in CsvReader: eof"; 

	QString smsg = in_txt_stream_.readLine();
	slist = smsg.split(',', QString::SkipEmptyParts);

	if (slist.size() != columns_num_)
	{
		throw "ERROR in CsvReader: the size of a line is not equal to the number of columns";
	}

	cursor_line_num_++;

	return slist;
}

QString CsvReader::GetCurrentLineElement(int column_num)
{
	if (!is_all_set_)
		throw "ERROR in CsvReader: target file not set";

	if (column_num < 0 || column_num >= columns_num_)
		throw "ERROR in CsvReader: index out of range";

	return slist[column_num];
}

void CsvReader::__SkipHeader()
{
	if(cursor_line_num_ == 0)
		in_txt_stream_.readLine();
}
