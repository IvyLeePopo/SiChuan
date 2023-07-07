/*
* @Author: 朱亚冰
* @Date:   2017-12-06
* @Last Modified by:   windf
* @Last Modified time: 2017-12-07
*/

#include "IOAdapter.h"
#include <Windows.h>
#include <fstream>

std::map<int, size_t> FilePieceAdapter::threadsFilePieceSize;


std::istream &operator >> (std::istream &input, FilePieceAdapter &dataBlock)
{
    dataBlock.PieceSize = dataBlock.FilePieceSize_Bytes();
    if (dataBlock.PieceSize == 0)
    {
		input.setstate(std::ios_base::eofbit);
		input.setstate(std::ios_base::failbit);
        return input;
    }

    //迭代器中，如果遇到了末尾，不进行处理，但是最后一次可能读到了部分数据
	std::streampos currInputPos = input.tellg();
    input.seekg(0, std::ios_base::end);
	std::streampos endInputPos = input.tellg();
	input.seekg(currInputPos, std::ios_base::beg);
    if (currInputPos == endInputPos)
    {
        input.setstate(std::ios_base::eofbit);
        input.setstate(std::ios_base::failbit);
        return input;
    }
    delete dataBlock.Data;
    dataBlock.Data = NULL;
    dataBlock.DataSize_Bytes = 0;
    dataBlock.Data = new char[dataBlock.PieceSize];
    if (dataBlock.Data == NULL)
    {
        input.setstate(std::ios_base::eofbit);
        input.setstate(std::ios_base::failbit);
        return input;
    }
    if (dataBlock.PieceSize <= (endInputPos - currInputPos))
    {
        input.read(dataBlock.Data, dataBlock.PieceSize);
    }
    else
    {
        input.read(dataBlock.Data, endInputPos - currInputPos);
    }

    dataBlock.DataSize_Bytes = input.gcount();
	
    //dataBlock.DataSize = input.readsome(dataBlock.Data, dataBlock.PieceSize);

    return input;
}

FilePieceAdapter::FilePieceAdapter()
{
    Data = NULL;
    DataSize_Bytes = 0;
    PieceSize = 0;
}
FilePieceAdapter::~FilePieceAdapter()
{
    delete Data;
    Data = NULL;
}

void FilePieceAdapter::DeepClone(const FilePieceAdapter& other)
{
	if(other.DataSize_Bytes==0)
	{
		return ;
	}
    delete Data;
    Data = NULL;
    PieceSize = other.PieceSize;
    DataSize_Bytes = other.DataSize_Bytes;
    Data = new char[DataSize_Bytes];
	
    if (Data)
    {
        memcpy(Data, other.Data, other.DataSize_Bytes);
    }
	
}
FilePieceAdapter::FilePieceAdapter(const FilePieceAdapter& other)
{
	Data = NULL;
    DataSize_Bytes = 0;
    PieceSize = 0;
    if (this != &other)
    {
        DeepClone(other);
    }
}
FilePieceAdapter& FilePieceAdapter::operator = (const FilePieceAdapter& other)
{
    if (this == &other)
    {
        return *this;
    }
    DeepClone(other);

}


void FilePieceAdapter::SetFilePieceSize(size_t pieceSize_Bytes)
{
    threadsFilePieceSize[GetCurrentThreadId()] = pieceSize_Bytes;
}
size_t FilePieceAdapter::FilePieceSize_Bytes()
{
    return threadsFilePieceSize[GetCurrentThreadId()];
}

size_t FileGetSize(const std::string& filePath)
{
	std::ifstream sizeReader(filePath.c_str(), std::ios::binary);
    if (!sizeReader.is_open())
    {
        return 0 ;
    }
    sizeReader.seekg(0, std::ios_base::end);
    size_t fileBytes = sizeReader.tellg();
    sizeReader.close();

    return fileBytes;
}