#ifndef IO_ADAPTER_H
#define IO_ADAPTER_H
//�������������
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
     * �������ݵ������Ǳ��ֶ��߳��µķ�����ȷ��
     * �����ļ��ķ�Ƭ��С���ǹ̶��ģ�������Ҫ��ÿ�η���ǰ���÷�Ƭ�Ĵ�С
     * �ڶ��̻߳����£� �߳�֮��ķ�Ƭ��С�������໥���ţ���˷���
     */
    static void SetFilePieceSize(size_t pieceSize_Bytes);
    static size_t FilePieceSize_Bytes();

private:
    static std::map<int, size_t> threadsFilePieceSize;


};

std::istream &operator >> (std::istream &input, FilePieceAdapter &dataBlock);//������
// ��λ��Byte
size_t FileGetSize(const std::string& filePath);

#endif // IO_ADAPTER_H
