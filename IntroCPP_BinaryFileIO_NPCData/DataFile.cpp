#include "DataFile.h"
#include <fstream>
using namespace std;

DataFile::DataFile()
{
	recordCount = 0;
	fileName = "";
}

DataFile::~DataFile()
{
	Clear();
}

void DataFile::AddRecord(string imageFilename, string name, int age)
{
	Image i = LoadImage(imageFilename.c_str());

	Record* r = new Record;
	r->image = i;
	r->name = name;
	r->age = age;

	records.push_back(r);
	recordCount++;
}

DataFile::Record* DataFile::GetRecord(int index)
{
	//check if file exists
	if (fileName == "") { throw "File not loaded"; }
	//check if index is valid for the loaded file
	if (index > recordCount) { throw "Index out of bounds"; }
	//load until index is reached
	while (index >= records.size()) {
		LoadRecord(records.size());
	}
	return records[index];
}

void DataFile::Save(string filename)
{
	ofstream outfile(filename, ios::binary);

	int recordCount = records.size();
	outfile.write((char*)&recordCount, sizeof(int));

	for (int i = 0; i < recordCount; i++)
	{		
		Color* imgdata = GetImageData(records[i]->image);
				
		int imageSize = sizeof(Color) * records[i]->image.width * records[i]->image.height;
		int nameSize = records[i]->name.length();
		int ageSize = sizeof(int);

		outfile.write((char*)&records[i]->image.width, sizeof(int));
		outfile.write((char*)&records[i]->image.height, sizeof(int));
		
		outfile.write((char*)&nameSize, sizeof(int));
		outfile.write((char*)&ageSize, sizeof(int));

		outfile.write((char*)imgdata, imageSize);
		outfile.write((char*)records[i]->name.c_str(), nameSize);
		outfile.write((char*)&records[i]->age, ageSize);
	}

	outfile.close();
}

void DataFile::Load(string filename)
{
	fileName = filename;
}

void DataFile::LoadRecord(int currentIndex)
{
	ifstream infile(fileName, ios::binary);

	recordCount = 0;
	infile.read((char*)&recordCount, sizeof(int));

	if (currentIndex < recordCount && currentIndex > -1) { //if valid index
		//check if file exists
		if (fileName == "") { throw "File not loaded"; }

		//init variables
		int ind = 0;
		int nameSize = 0;
		int ageSize = 0;
		int width = 0, height = 0, format = 0, imageSize = 0;
		while (ind < currentIndex) {
			//read sizes
			infile.read((char*)&width, sizeof(int));
			infile.read((char*)&height, sizeof(int));
			imageSize = sizeof(Color) * width * height;
			infile.read((char*)&nameSize, sizeof(int));
			infile.read((char*)&ageSize, sizeof(int));
			//skip ahead
			streampos current = infile.tellg();
			streamsize dataSize = imageSize + (nameSize * sizeof(char)) + ageSize;
			streampos next = current + dataSize;
			infile.seekg(next);

			ind++;
		}
		//run through of selected file
		infile.read((char*)&width, sizeof(int));
		infile.read((char*)&height, sizeof(int));
		imageSize = sizeof(Color) * width * height;
		infile.read((char*)&nameSize, sizeof(int));
		infile.read((char*)&ageSize, sizeof(int));

		char* imgdata = new char[imageSize];
		infile.read(imgdata, imageSize);

		Image img = LoadImageEx((Color*)imgdata, width, height);
		char* name = new char[nameSize];
		int age = 0;

		infile.read((char*)name, nameSize);
		infile.read((char*)&age, ageSize); 
		//record selected file
		Record* r = new Record();
		r->image = img;
		r->name = string(name);
		r->name.resize(nameSize); //strings initialize with a set size
		r->age = age;
		records.push_back(r);

		delete[] imgdata;
		delete[] name;
	}
	else{ throw "Index out of bounds"; }
	infile.close();
}

void DataFile::Clear()
{
	for (int i = 0; i < records.size(); i++)
	{
		delete records[i];
	}
	records.clear();
	recordCount = 0;
}