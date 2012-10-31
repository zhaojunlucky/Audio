#include <vector>
#include "..\ZLP\Media.h"

using namespace std;

struct LRCTagTime
{
	int lineNum;
	long time;
};

struct LRCWord
{
	CString lrcWord;
	LRCWord()
	{

	}
	~LRCWord()
	{
		lrcWord.Empty();
	}

	LRCWord(const LRCWord& l)
	{
		lrcWord = l.lrcWord;
	}
	LRCWord &operator=(const LRCWord& l)
	{
		lrcWord = l.lrcWord;
		return *this;
	}
};

long StrToSS(LPWSTR lpszTag);

int CompareTime(LRCTagTime& tag1, LRCTagTime& tag2);


class CZLRC
{
public:
	CZLRC();
	~CZLRC();
	int ReadLRCFile(wchar_t *lrcFile);
	int ReadLRCFile(const Media &m);
	void Clear();
private:
	bool ParseLine(wchar_t*  line,int &lineNum);
	void AddLRCTag(wchar_t* tag,int &lineNum);
	void AddLRCWord(wchar_t* lrcWord);
public:
	vector<LRCTagTime> tagTimes;
	vector<LRCWord> lrcWords;
	int offset;
	wchar_t lrcDir[MAX_PATH*2];
};

