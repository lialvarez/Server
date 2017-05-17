#include "FileSystem.h"

FileSystem::FileSystem() : lastData(false){}

void FileSystem::openFile(std::string fileName, openMode mode)
{
	if (mode == READ)
	{
		fileStream.open(fileName, std::fstream::in | std::fstream::binary);
	}
	else if(mode == WRITE)
	{
		fileStream.open(fileName, std::fstream::out | std::fstream::binary);
	}
	lastData = false;
}

void FileSystem::saveData(std::vector<char> data)
{
	std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(fileStream));

	//fileStream.write((char *)&data[0], data.size());
}

std::vector<char> FileSystem::readData()
{	
	char aux[512];
	fileStream.read(aux, 512);
	int len = fileStream.gcount();
	if (len < 512)
	{
		lastData = true;
	}
	std::vector<char> ret(aux, aux + len);
	return ret;
}

void FileSystem::closeFile()
{
	if (fileStream.is_open())
	{
		fileStream.close();
	}
}