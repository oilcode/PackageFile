//-----------------------------------------------------------------------------
// SoPackageFile
// (C) oil
// 2013-07-13
//
// 1，与文件长度大小相关的变量都使用64位整数，支持无限大的资源包文件。
// 2，最大限度满足跨平台需求。
// 3，用户自己定义版本号规则。
//-----------------------------------------------------------------------------
#include "SoPackageFile.h"
//-----------------------------------------------------------------------------
namespace GGUI
{
	//-----------------------------------------------------------------------------
	SoPackageFile::SoPackageFile()
	:m_theFileMode(Mode_None)
	,m_pFile(0)
	,m_pSingleFileInfoList(0)
	,m_nSingleFileInfoListCapacity(0)
	,m_nSingleFileInfoListSize(0)
	{

	}
	//-----------------------------------------------------------------------------
	SoPackageFile::~SoPackageFile()
	{
		ReleasePackageFile();
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::InitPackageFile(const char* pszPackageFile, FileMode theFileMode)
	{
		if (pszPackageFile == 0 //空指针
			|| pszPackageFile[0] == 0 //空字符串
			|| theFileMode == Mode_None) //无效的文件操作模式
		{
			return Result_InvalidParam;
		}
		if (m_pFile || m_theFileMode != Mode_None)
		{
			//资源包已经打开，必须先关闭才能打开下一个资源包。
			return Result_PackageFileAlreadyOpen;
		}
		FILE* pFile = 0;
		//记录是否需要解析资源包，即提取资源包已有的文件结构信息。
		bool bParsePackageFile = false;
		if (theFileMode == Mode_Read)
		{
			//读模式，资源包文件必须存在。
			pFile = fopen(pszPackageFile, "rb");
			if (pFile == 0)
			{
				//打开失败。
				return Result_OpenFileFail;
			}
			else
			{
				//资源包存在，需要解析资源包。
				bParsePackageFile = true;
			}
		}
		else if (theFileMode == Mode_Write)
		{
			//写模式，如果资源包不存在则创建。
			pFile = fopen(pszPackageFile, "r+");
			if (pFile == 0)
			{
				//资源包不存在，则创建。
				pFile = fopen(pszPackageFile, "w+");
				if (pFile == 0)
				{
					return Result_CreateFileFail;
				}
				else
				{
					bParsePackageFile = false;
				}
			}
			else
			{
				//资源包存在，需要解析资源包。
				bParsePackageFile = true;
			}
		}
		m_theFileMode = theFileMode;
		m_pFile = pFile;
		if (bParsePackageFile)
		{
			//解析资源包。
			OperationResult eResult = ParsePackageFile();
			if (eResult != Result_OK)
			{
				ReleasePackageFile();
				return eResult;
			}
		}
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::ReleasePackageFile()
	{
		m_theFileMode = Mode_None;
		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile = 0;
		}
		m_stPackageHead.Clear();
		ReleaseSingleFileInfoList();
		m_mapFileName2FileID.clear();
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Open(const char* pszFileName, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Close(stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Read(void* pBuff, __int64 nBuffSize, __int64 nReadSize, __int64& nActuallyReadSize, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Tell(__int64& nFilePos, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Seek(__int64 nOffset, SeekOrigin theOrigin, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::InsertSingleFile()
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::WritePackageHead()
	{
		//判断当前文件状态。
		if (m_pFile == 0)
		{
			return Result_PackageFileHaveNotOpen;
		}
		__int64 nSeekResult = fseek(m_pFile, 0, SEEK_SET);
		if (nSeekResult != 0)
		{
			return Result_FileOperationError;
		}
		//生成合法的文件头。
		const char* pszFileFlag = SoPackageFileFlag;
		for (__int64 i=0; i<SoPackageFileFlagLength; ++i)
		{
			m_stPackageHead.szFileFlag[i] = pszFileFlag[i];
		}
		m_stPackageHead.nVersion = SoPackageFileVersion;
		//写入。
		const size_t sizePackageHead = sizeof(m_stPackageHead);
		size_t nActuallyWrite = fwrite(&m_stPackageHead, sizePackageHead, 1, m_pFile);
		if (nActuallyWrite != sizePackageHead)
		{
			//文件头没有写入完整。
			return Result_FileOperationError;
		}
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::WriteSingleFile(FILE* pSingleFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::WriteAllSingleFileInfo()
	{
		//判断当前文件状态。
		if (m_pFile == 0)
		{
			return Result_PackageFileHaveNotOpen;
		}
		__int64 nSeekResult = fseek(m_pFile, m_stPackageHead.nOffsetForFirstSingleFileInfo, SEEK_SET);
		if (nSeekResult != 0)
		{
			return Result_FileOperationError;
		}
		//写入。
		const size_t sizeAllSingleFileInfo = m_nSingleFileInfoListSize * sizeof(stSingleFileInfo);
		size_t nActuallyWrite = fwrite(m_pSingleFileInfoList, sizeAllSingleFileInfo, 1, m_pFile);
		if (nActuallyWrite != sizeAllSingleFileInfo)
		{
			//SingleFile信息列表没有写入完整。
			return Result_FileOperationError;
		}
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::ParsePackageFile()
	{
		if (m_pFile == 0)
		{
			return Result_PackageFileHaveNotOpen;
		}
		__int64 nSeekResult = fseek(m_pFile, 0, SEEK_SET);
		if (nSeekResult != 0)
		{
			return Result_FileOperationError;
		}
		//读取文件头。
		const size_t sizePackageHead = sizeof(m_stPackageHead);
		size_t nActuallyReadSize = fread(&m_stPackageHead, sizePackageHead, sizePackageHead, m_pFile);
		if (nActuallyReadSize != sizePackageHead)
		{
			//文件头没有读取完整。
			return Result_FileOperationError;
		}
		//判断文件头。
		if (!CheckValid_PackageHead(m_stPackageHead))
		{
			return Result_IsNotPackageFile;
		}
		//获取SingleFile信息列表。
		nSeekResult = fseek(m_pFile, m_stPackageHead.nOffsetForFirstSingleFileInfo, SEEK_SET);
		if (nSeekResult != 0)
		{
			return Result_FileOperationError;
		}
		ReleaseSingleFileInfoList();
		ReCreateSingleFileInfoList(m_stPackageHead.nFileCount);
		if (m_pSingleFileInfoList == 0)
		{
			return Result_MemoryIsEmpty;
		}
		const size_t sizeSingleFileInfoList = m_stPackageHead.nFileCount * sizeof(stSingleFileInfo);
		size_t nActuallyReadInfoListSize = fread(m_pSingleFileInfoList, sizeSingleFileInfoList, sizeSingleFileInfoList, m_pFile);
		if (nActuallyReadInfoListSize != sizeSingleFileInfoList)
		{
			//SingleFile信息列表没有读取完整。
			return Result_FileOperationError;
		}
		//SingleFile信息列表读取成功。
		//下面生成文件名到文件ID的映射。
		m_mapFileName2FileID.clear();
		for (__int64 i=0; i<m_stPackageHead.nFileCount; ++i)
		{
			if (CheckValid_SingleFileInfo(m_pSingleFileInfoList[i]))
			{
				m_mapFileName2FileID.insert(make_pair(std::string(m_pSingleFileInfoList[i].szFileName), i));
			}
			else
			{
				//出现了无效的文件信息，很可能整个资源包都被破坏了。
				m_pSingleFileInfoList[i].Clear();
			}
		}
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	void SoPackageFile::ReCreateSingleFileInfoList(__int64 nCapacity)
	{
		stSingleFileInfo* pSingleFileInfoList_Temp = m_pSingleFileInfoList;
		//
		m_pSingleFileInfoList = (stSingleFileInfo*)malloc(nCapacity * sizeof(stSingleFileInfo));
		if (m_pSingleFileInfoList)
		{
			//清零
			memset(m_pSingleFileInfoList, 0, nCapacity * sizeof(stSingleFileInfo));
			//拷贝已有的值
			if (m_nSingleFileInfoListSize > 0)
			{
				memcpy(m_pSingleFileInfoList, pSingleFileInfoList_Temp, m_nSingleFileInfoListSize*sizeof(stSingleFileInfo));
			}
			m_nSingleFileInfoListCapacity = nCapacity;
		}
		else
		{
			//申请内存失败。
			m_pSingleFileInfoList = 0;
			m_nSingleFileInfoListCapacity = 0;
			m_nSingleFileInfoListSize = 0;
		}
		//
		if (pSingleFileInfoList_Temp)
		{
			free(pSingleFileInfoList_Temp);
			pSingleFileInfoList_Temp = 0;
		}
	}
	//-----------------------------------------------------------------------------
	void SoPackageFile::ReleaseSingleFileInfoList()
	{
		m_nSingleFileInfoListCapacity = 0;
		m_nSingleFileInfoListSize = 0;
		if (m_pSingleFileInfoList)
		{
			free(m_pSingleFileInfoList);
			m_pSingleFileInfoList = 0;
		}
	}
	//-----------------------------------------------------------------------------
	void SoPackageFile::FormatFileFullName(std::string& strOut, const char* pszIn)
	{
		strOut = pszIn;
		__int64 nLength = strOut.length();
		for (__int64 i=0; i<nLength; ++i)
		{
			char& theC = strOut[i];
			if (theC >= 'A' && theC <= 'Z')
			{
				theC += 32;
			}
			else if (theC == '\\')
			{
				theC = '/';
			}
		}
	}
	//-----------------------------------------------------------------------------
	bool SoPackageFile::CheckValid_PackageHead(const SoPackageFile::stPackageHead& theHead)
	{
		bool br = true;
		//判断文件标志
		if (br)
		{
			const char* pszFileFlag = SoPackageFileFlag;
			for (__int64 i=0; i<SoPackageFileFlagLength; ++i)
			{
				if (theHead.szFileFlag[i] != pszFileFlag[i])
				{
					br = false;
					break;
				}
			}
		}
		//判断版本号
		if (br)
		{
			if (theHead.nVersion != SoPackageFileVersion)
			{
				br = false;
			}
		}
		//判断文件个数
		if (br)
		{
			if (theHead.nFileCount < 0)
			{
				br = false;
			}
		}
		//判断偏移量
		if (br)
		{
			if (theHead.nOffsetForFirstSingleFileInfo < 0)
			{
				br = false;
			}
			else
			{
				if (theHead.nFileCount > 0 && theHead.nOffsetForFirstSingleFileInfo <= sizeof(theHead))
				{
					br = false;
				}
			}
		}
		return br;
	}
	//-----------------------------------------------------------------------------
	bool SoPackageFile::CheckValid_SingleFileInfo(const SoPackageFile::stSingleFileInfo& theSingleFile)
	{
		bool br = true;
		//判断文件名
		if (br)
		{
			if (theSingleFile.szFileName[0] == 0 //文件名为空
				|| theSingleFile.szFileName[SoPackageFileMAX_PATH-1] != 0) //不是以0结尾的字符串
			{
				br = false;
			}
		}
		//
		if (br)
		{
			if (theSingleFile.nOriginalFileSize <= 0)
			{
				br = false;
			}
		}
		//
		if (br)
		{
			if (theSingleFile.nEmbededFileSize <= 0)
			{
				br = false;
			}
		}
		//
		if (br)
		{
			if (theSingleFile.nOffset <= sizeof(stPackageHead))
			{
				br = false;
			}
		}
		return br;
	}
}
//-----------------------------------------------------------------------------
