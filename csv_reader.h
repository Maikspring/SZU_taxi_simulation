#pragma once

// TODO: Using standard libraries to take place qt 

#include <QtCore>
#include <fstream>
//#include <iostream>

class CsvReader;
class CsvBinaryReader;

class CsvReader {
public:
	CsvReader(QString filename);
	CsvReader(QString filename, bool is_with_header);
	CsvReader();
	~CsvReader();

public:
	void set_is_with_header(bool is_with_header);
	void SetFile(QString filename);
	void close();

	int get_cursor_line_num();
	bool toNextLine();
	int  get_column_num();
	bool eof();

// reading methods 
public:
	QStringList readLine();
	QString GetCurrentLineElement(int column_num);

private:
	void __SkipHeader();

private:
	bool is_all_set_;
	bool is_with_header_; // is the file has a header row for indicating the columns

	QString filename_;
	QFile infile_;
	QTextStream in_txt_stream_;
	QStringList slist;

	int columns_num_;
	int cursor_line_num_;
};



class CsvBinaryReader {


};
