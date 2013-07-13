//-----------------------------------------------------------------------------
// SoPackageFile
// (C) oil
// 2013-07-13
//-----------------------------------------------------------------------------
#ifndef _SoPackageFile_h_
#define _SoPackageFile_h_
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <map>
//-----------------------------------------------------------------------------
#define SoPackageFileFlag "SOPACKAG"
#define SoPackageFileFlagLength 8
#define SoPackageFileVersion 1
#define SoPackageFileMAX_PATH 260
//-----------------------------------------------------------------------------
namespace GGUI
{
	class SoPackageFile
	{
	public:
		enum FileMode
		{
			Mode_None,
			Mode_Read,
			Mode_Write,
		};
		enum SeekOrigin
		{
			Seek_Set = 0,
			Seek_Cur = 1,
			Seek_End = 2,
		};
		enum OperationResult
		{
			Result_OK, //顺利完成
			Result_InvalidParam, //存在无效的参数
			Result_PackageFileHaveNotOpen, //资源包尚未打开，不能执行操作。
			Result_PackageFileAlreadyOpen, //资源包已经打开，必须先关闭才能打开下一个资源包。
			Result_IsNotPackageFile, //不是SoPackageFile文件
			Result_OpenFileFail, //打开磁盘文件失败
			Result_CreateFileFail, //创建磁盘文件失败
			Result_FileOperationError, //C标准库中的文件操作函数返回了失败。
			Result_MemoryIsEmpty, //申请内存失败，内存不足。
		};
		//资源包文件头。
		struct stPackageHead
		{
			//文件标志。
			char szFileFlag[SoPackageFileFlagLength];
			//版本号。
			int nVersion;
			//一共有多少个SingleFile。
			int nFileCount;
			//SingleFile信息集合中，第一个stSingleFileInfo距离文件开始处的偏移量。
			int nOffsetForFirstSingleFileInfo;
		};
		//SingleFile的信息。
		struct stSingleFileInfo
		{
			//文件名
			char szFileName[SoPackageFileMAX_PATH];
			//文件原始大小
			int nOriginalFileSize;
			//文件在资源包内的大小（有可能经过了压缩）
			int nEmbededFileSize;
			//该文件距离资源包文件开始处的偏移量。
			int nOffset;
		};
		struct stReadSingleFile
		{
			//资源包内每个文件都有一个文件ID。
			int nFileID;
			//文件原始大小。
			int nFileSize;
			//文件指针。
			int nFilePointer;
			//存储文件原始的完整内容。
			char* pFileBuff;
		};

	public:
		SoPackageFile();
		~SoPackageFile();
		OperationResult InitPackageFile(const char* pszPackageFile, FileMode theFileMode);
		OperationResult ReleasePackageFile();

		//<<<<<<<<<<<<<<<< 从资源包内读取一个文件 <<<<<<<<<<<<<<<<<<<<<<<<<
		OperationResult Open(const char* pszFileName, stReadSingleFile& theFile);
		OperationResult Close(stReadSingleFile& theFile);
		OperationResult Read(void* pBuff, int nBuffSize, int nReadSize, int& nActuallyReadSize, stReadSingleFile& theFile);
		OperationResult Tell(int& nFilePos, stReadSingleFile& theFile);
		OperationResult Seek(int nOffset, SeekOrigin theOrigin, stReadSingleFile& theFile);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//<<<<<<<<<<<<<<<< 把一个磁盘文件写入资源包 <<<<<<<<<<<<<<<<<<<<<<<
		OperationResult InsertSingleFile();
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	private:
		//写入资源包文件头。
		OperationResult WritePackageHead();
		//把一个原始的SingleFile写入到资源包中。
		OperationResult WriteSingleFile(FILE* pSingleFile);
		//把stSingleFileInfo信息集合写入到资源包中。
		OperationResult WriteAllSingleFileInfo();
		//解析资源包，即提取资源包已有的文件结构信息。
		OperationResult ParsePackageFile();

		void ReCreateSingleFileInfoList(int nCapacity);
		void ReleaseSingleFileInfoList();
		//把文件名格式化成如下格式：
		//1，把'\\'修改成'/'；
		//2，把大写字母修改成小写字母；
		void FormatFileFullName(std::string& strOut, const char* pszIn);

	private:
		FileMode m_theFileMode;
		FILE* m_pFile;
		//文件头。
		stPackageHead m_stPackageHead;
		//SingleFile信息列表。
		stSingleFileInfo* m_pSingleFileInfoList;
		//m_pSingleFileInfoList中可以容纳多少个stSingleFileInfo对象。
		int m_nSingleFileInfoListCapacity;
		//m_pSingleFileInfoList中有效stSingleFileInfo对象的个数。
		int m_nSingleFileInfoListSize;
	};
}
//-----------------------------------------------------------------------------
#endif //_SoPackageFile_h_
//-----------------------------------------------------------------------------
