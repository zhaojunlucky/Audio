#pragma once
class CZParseCommandLine
{
public:
	CZParseCommandLine(void);
	~CZParseCommandLine(void);
	unsigned int GetCount();
	const CString GetAt(unsigned int);

	void ParseCommand();
private:
	CStringArray fileList;
};

