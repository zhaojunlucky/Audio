#pragma once
class CDirectoryUtil
{
public:
	CDirectoryUtil(void);
	~CDirectoryUtil(void);

	int FindFiles(const wchar_t* directory);
	int GetFileCount();
	const wchar_t* GetFilePath(int index);
private:
	CStringList m_files;
};

