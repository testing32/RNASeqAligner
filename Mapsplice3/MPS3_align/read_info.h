#ifndef __READ_INFO_H_INCLUDED__
#define __READ_INFO_H_INCLUDED__

#include <string>
#include <string.h>
#include <stdexcept>

using namespace std;

class Read
{
private:

	string reverseComplement(const char& character)
	{
		switch(character)
		{
			case 'A':
				return "T";
				break;
			case 'T':
				return "A";
				break;
			case 'G':
				return "C";
				break;
			case 'C':
				return "G";
				break;
			case 'N':
				return "N";
				break;
			default: // invalid character
				throw std::invalid_argument(string("Invalid character ") + character + string(" in read"));
				break;
		}
	}

	string getReverseComplementString()
	{
		string rc = "";
		for(int i = 0; i < length(); i++)
		{
			rc += reverseComplement(readSeq.at(length() - 1 - i));
		}
		return rc;
	}

	string getReverseComplementQuaility()
	{
		int length = readQual.size();
		string resultString = readQual.substr(length-1, 1);
		for (int i = 1; i < length; i++)
			resultString = resultString + readQual.substr(length - 1 - i, 1);

		return resultString;
	}

public:

	string readName;
	string readSeq;
	string readQual;

	int length()
	{
		return readSeq.length();
	}

	/*
	 * returns the reverse complement of this read
	 */
	Read getReverseComplement()
	{
		Read reverseComplementRead;
		reverseComplementRead.readName = readName;
		reverseComplementRead.readSeq = getReverseComplementString();
		reverseComplementRead.readQual = getReverseComplementQuaility();

		return reverseComplementRead;
	}

};

class PairedEndRead
{

public:

	Read firstPairedEndRead;
	Read secondPairedEndRead;

	PairedEndRead()
	{}
	/* FIX ME - DELETE LATER 5/28/14
	bool checkEnd1OrEnd2WithAlignInfoTypeNo(int alignInfoType)
	{
		if(alignInfoType <= 2)
			return true;
		else
			return false;
	}

	bool checkNorOrRcmWithAlignInfoTypeNo(int alignInfoType)
	{
		if((alignInfoType == 1)||(alignInfoType == 3))
			return true;
		else
			return false;
	}

	int checkReadLengthWithAlignInfoTypeNo(int alignInfoType)
	{
		if(alignInfoType <= 2)
			return firstPairedEndRead.length();
		else
			return secondPairedEndRead.length();
	}
	 */

	void get_PE_Read_Info(const string& readName1, const string& readName2,
		const string& readSeq1, const string& readSeq2)
	{
		(firstPairedEndRead.readName) = readName1;
		(firstPairedEndRead.readSeq) = readSeq1;

		(secondPairedEndRead.readName) = readName2;
		(secondPairedEndRead.readSeq) = readSeq2;
	}

	void getFastaFormatReadInfo(const string& readName_1, const string& readName_2, 
		const string& readSeq_1, const string& readSeq_2)
	{
		firstPairedEndRead.readName = readName_1;
		firstPairedEndRead.readSeq = readSeq_1;
		firstPairedEndRead.readQual = "*";

		secondPairedEndRead.readName = readName_2;
		secondPairedEndRead.readSeq = readSeq_2;
		secondPairedEndRead.readQual = "*";

	}

	Read getFirstRead()
	{
		return firstPairedEndRead;
	}

	Read getSecondRead()
	{
		return secondPairedEndRead;
	}

	Read getFirstReadReverseComplement()
	{
		return firstPairedEndRead.getReverseComplement();
	}

	Read getSecondReadReverseComplement()
	{
		return secondPairedEndRead.getReverseComplement();
	}
};

#endif
