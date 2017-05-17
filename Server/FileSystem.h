#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <fstream>

#define SAVE_DIR	"Download/"

typedef enum { READ, WRITE }openMode;

class FileSystem
{
public:
	FileSystem();
	void closeFile();
	void openFile(std::string fileName, openMode mode);
	void saveData(std::vector<char> data);
	std::vector<char> readData();
	bool lastData;
private:
	
	std::fstream fileStream;
	std::streamsize dataSize;
};

#endif // !FILESYSTEM_H
