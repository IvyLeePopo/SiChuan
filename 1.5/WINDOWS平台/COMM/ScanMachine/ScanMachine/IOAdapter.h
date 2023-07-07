#ifndef IO_ADAPTER_H
#define IO_ADAPTER_H
//输入输出处理器
#include <map>
#include <io.h>

class FilePieceAdapter
{
public:
    FilePieceAdapter();
    ~FilePieceAdapter();
    size_t PieceSize;
    char* Data;
    size_t DataSize_Bytes;
    void DeepClone(const FilePieceAdapter& other);
    FilePieceAdapter(const FilePieceAdapter& other);
    FilePieceAdapter& operator = (const FilePieceAdapter& other);

public:
    /**
     * 下面内容的作用是保持多线程下的访问正确性
     * 由于文件的分片大小不是固定的，所以需要在每次访问前设置分片的大小
     * 在多线程环境下， 线程之间的分片大小参数会相互干扰，因此分离
     */
    static void SetFilePieceSize(size_t pieceSize_Bytes);
    static size_t FilePieceSize_Bytes();

private:
    static std::map<int, size_t> threadsFilePieceSize;


};

std::istream &operator >> (std::istream &input, FilePieceAdapter &dataBlock);//输入流
// 单位，Byte
size_t FileGetSize(const std::string& filePath);

#endif // IO_ADAPTER_H
