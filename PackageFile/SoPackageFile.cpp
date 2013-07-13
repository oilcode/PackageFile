//-----------------------------------------------------------------------------
// SoPackageFile
// (C) oil
// 2013-07-13
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
			//未完待续
		}
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::ReleasePackageFile()
	{
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
	SoPackageFile::OperationResult SoPackageFile::Read(void* pBuff, int nBuffSize, int nReadSize, int& nActuallyReadSize, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Tell(int& nFilePos, stReadSingleFile& theFile)
	{
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::Seek(int nOffset, SeekOrigin theOrigin, stReadSingleFile& theFile)
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
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	SoPackageFile::OperationResult SoPackageFile::ParsePackageFile()
	{
		if (m_pFile == 0)
		{
			return Result_PackageFileHaveNotOpen;
		}
		int nResult = fseek(m_pFile, 0, SEEK_SET);
		if (nResult != 0)
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
		bool bFileFlagOK = true;
		const char* pszFileFlag = SoPackageFileFlag;
		for (int i=0; i<SoPackageFileFlagLength; ++i)
		{
			if (m_stPackageHead.szFileFlag[i] != pszFileFlag[i])
			{
				bFileFlagOK = false;
				break;
			}
		}
		if (!bFileFlagOK)
		{
			return Result_IsNotPackageFile;
		}
		//获取SingleFile信息列表。
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
		//生成map，未完待续
		return Result_OK;
	}
	//-----------------------------------------------------------------------------
	void SoPackageFile::ReCreateSingleFileInfoList(int nCapacity)
	{
		stSingleFileInfo* pSingleFileInfoList_Temp = m_pSingleFileInfoList;
		//
		m_pSingleFileInfoList = (stSingleFileInfo*)malloc(nCapacity * sizeof(stSingleFileInfo));
		if (m_pSingleFileInfoList)
		{
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
		int nLength = (int)strOut.length();
		for (int i=0; i<nLength; ++i)
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
}
//-----------------------------------------------------------------------------
