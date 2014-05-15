#include <string>
#include <string.h>
#include <map>
#include <set>

using namespace std;

typedef map<string, set<int> > SpliceEndStrHash; // SJ hash (in 2nd hash, key = anchor string)
typedef map<int, SpliceEndStrHash> SplicePosHash;
typedef SplicePosHash::iterator SplicePosHashIter; 
typedef SpliceEndStrHash::iterator SpliceEndStrHashIter;

typedef map<int, set<int> > SJintHash;  // SJ hash (only 1 hash, value is all possible splice junction positions in the other side)
typedef SJintHash::iterator SJintHashIter;

typedef map<int, set<int> > SJareaHash; //( areaNO = pos/100 ) intermediate hash to directly get all possible splice junctions near a certain position
typedef SJareaHash::iterator SJareaHashIter;


class SJhash_Info
{
public: 
	vector<SplicePosHash> spliceJunctionNormal; // size = chromNum in index_info file
	vector<SplicePosHash> spliceJunctionReverse; // size = chromNum in index_info file

	int anchorStringLength;

	vector<SJintHash> SJintHashNormal;
	vector<SJintHash> SJintHashReverse;

	int areaSize;

	vector<SJareaHash> SJstartPosAreaHash;
	vector<SJareaHash> SJendPosAreaHash;

	SJhash_Info()
	{
		areaSize = 1000;
		anchorStringLength = 3;
	}

	void initiateSpliceJunctionStringHash(int chromNum)
	{
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			SplicePosHash newSplicePosHash;
			spliceJunctionNormal.push_back(newSplicePosHash);
		}
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			SplicePosHash newSplicePosHash;
			spliceJunctionReverse.push_back(newSplicePosHash);
		}
	}

	void initiateSpliceJunctionAreaHash(int chromNum)
	{
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			//SplicePosHash newSplicePosHash;
			//spliceJunctionNormal.push_back(newSplicePosHash);
			SJareaHash newSJareaHash;
			SJstartPosAreaHash.push_back(newSJareaHash);

		}
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			//SplicePosHash newSplicePosHash;
			//spliceJunctionReverse.push_back(newSplicePosHash);
			SJareaHash newSJareaHash;
			SJendPosAreaHash.push_back(newSJareaHash);			
		}
	}

	void initiateTwoLevelHash_areaAndStringHash(int chromNum)
	{
		this->initiateSpliceJunctionStringHash(chromNum);
		this->initiateSpliceJunctionAreaHash(chromNum);
	}

	void initiateSJintHash(int chromNum)
	{
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			SJintHash newSJintHash;
			SJintHashNormal.push_back(newSJintHash);	
		}
		for(int tmp = 0; tmp < chromNum; tmp++)
		{
			SJintHash newSJintHash;
			SJintHashReverse.push_back(newSJintHash);	
		}		
	}

	void insert2AreaHash(int chrInt,
		int spliceStartPos, int spliceEndPos)
	{
		int spliceStartPosAreaNO = (int)(spliceStartPos/areaSize);
		int spliceEndPosAreaNO = (int)(spliceEndPos/areaSize);

		SJareaHashIter foundAreaHashIter;
		
		// insert to SJstartPosAreaHash
		foundAreaHashIter = SJstartPosAreaHash[chrInt].find(spliceStartPosAreaNO);
		if(foundAreaHashIter == SJstartPosAreaHash[chrInt].end())
		{
			set<int> newPosSet;
			newPosSet.insert(spliceStartPos);
			SJstartPosAreaHash[chrInt].insert(pair<int, set<int> > (spliceStartPosAreaNO, newPosSet));
		}
		else
		{
			if( (foundAreaHashIter->second).find(spliceStartPos) == (foundAreaHashIter->second).end() )
			{
				(foundAreaHashIter->second).insert(spliceStartPos);
			}
			else
			{}
		}

		// insert to SJendPosAreaHash
		foundAreaHashIter = SJendPosAreaHash[chrInt].find(spliceEndPosAreaNO);
		if(foundAreaHashIter == SJendPosAreaHash[chrInt].end())
		{
			set<int> newPosSet;
			newPosSet.insert(spliceEndPos);
			SJendPosAreaHash[chrInt].insert(pair<int, set<int> > (spliceEndPosAreaNO, newPosSet));
		}
		else
		{
			if( (foundAreaHashIter->second).find(spliceEndPos)  == (foundAreaHashIter->second).end() )
			{
				(foundAreaHashIter->second).insert(spliceEndPos);
			}
			else
			{}
		}
	}

	void insert2StringHash(int chrInt,
		int spliceStartPos, int spliceEndPos, Index_Info* indexInfo)
	{
		string anchorString_doner = (indexInfo->chromStr)[chrInt].substr(
			spliceStartPos - anchorStringLength, anchorStringLength);
		string anchorString_acceptor = (indexInfo->chromStr)[chrInt].substr(
			spliceEndPos - 1, anchorStringLength);

		//insert to spliceJunctionNormal
		SplicePosHashIter foundPosHashIter;

		foundPosHashIter = spliceJunctionNormal[chrInt].find(spliceStartPos);
		if(foundPosHashIter != spliceJunctionNormal[chrInt].end())
		{
			SpliceEndStrHashIter foundEndStrHashIter 
				= (foundPosHashIter->second).find(anchorString_acceptor);
			if(foundEndStrHashIter != (foundPosHashIter->second).end())
			{
				set<int>::iterator intSetIter = (foundEndStrHashIter->second).find(spliceEndPos);
				if(intSetIter != (foundEndStrHashIter->second).end())
				{}  
				else
				{
					(foundEndStrHashIter->second).insert(spliceEndPos);
				}
			}
			else
			{
				set<int> newAcceptorPosSet;
				newAcceptorPosSet.insert(spliceEndPos);
				(foundPosHashIter->second).insert( 
					pair<string, set<int> > (anchorString_acceptor, newAcceptorPosSet) );
				//(foundPosHashIter->second).insert( pairn<string, set<int> > ("*", newDonerPosSet) );
			}

			SpliceEndStrHashIter foundEndStrHashIter_star = (foundPosHashIter->second).find("*");
			if(foundEndStrHashIter_star != (foundPosHashIter->second).end() )
			{
				set<int>::iterator intSetIter 
					= (foundEndStrHashIter_star->second).find(spliceEndPos);
				if(intSetIter != (foundEndStrHashIter_star->second).end())
				{}
				else
				{
					(foundEndStrHashIter_star->second).insert(spliceEndPos);
				}
			}
			else
			{
				cout << "key value = * cannot be found !!! in spliceJunction_info.h" << endl;
			}

		}
		else
		{
			set<int> newSpliceEndPosSet;
			newSpliceEndPosSet.insert(spliceEndPos);

			SpliceEndStrHash newSpliceEndStrHash;
			newSpliceEndStrHash.insert( 
				pair<string, set<int> > (anchorString_acceptor, newSpliceEndPosSet) );
			newSpliceEndStrHash.insert( 
				pair<string, set<int> > ("*", newSpliceEndPosSet) );

			spliceJunctionNormal[chrInt].insert(
				pair<int, SpliceEndStrHash> (spliceStartPos, newSpliceEndStrHash) );
		}

		foundPosHashIter = spliceJunctionReverse[chrInt].find(spliceEndPos);
		if(foundPosHashIter != spliceJunctionReverse[chrInt].end())
		{
			SpliceEndStrHashIter foundEndStrHashIter 
				= (foundPosHashIter->second).find(anchorString_doner);
			if(foundEndStrHashIter != (foundPosHashIter->second).end())
			{
				set<int>::iterator intSetIter = (foundEndStrHashIter->second).find(spliceStartPos);
				if(intSetIter != (foundEndStrHashIter->second).end())
				{}
				else
				{
					(foundEndStrHashIter->second).insert(spliceStartPos);
				}
			}
			else
			{
				set<int> newDonerPosSet;
				newDonerPosSet.insert(spliceStartPos);
				(foundPosHashIter->second).insert( 
					pair<string, set<int> > (anchorString_doner, newDonerPosSet) );
			}

			SpliceEndStrHashIter foundEndStrHashIter_star = (foundPosHashIter->second).find("*");
			if(foundEndStrHashIter_star != (foundPosHashIter->second).end() )
			{
				set<int>::iterator intSetIter 
					= (foundEndStrHashIter_star->second).find(spliceStartPos);
				if(intSetIter != (foundEndStrHashIter_star->second).end() )
				{}
				else
				{
					(foundEndStrHashIter_star->second).insert(spliceStartPos);
				} 
			}
			else
			{
				cout << "key value = * cannot be found !!! in spliceJunction_info.h" << endl;
			}

		}
		else
		{
			set<int> newSpliceStartPosSet;
			newSpliceStartPosSet.insert(spliceStartPos);

			SpliceEndStrHash newSpliceEndStrHash;
			newSpliceEndStrHash.insert( 
				pair<string, set<int> > (anchorString_doner, newSpliceStartPosSet) );
			newSpliceEndStrHash.insert(
				pair<string, set<int> > ("*", newSpliceStartPosSet) );
		
			spliceJunctionReverse[chrInt].insert(
				pair<int, SpliceEndStrHash> (spliceEndPos, newSpliceEndStrHash) );
		}
	}

	void insert2SJintHash(
		int chrInt, int spliceStartPos, int spliceEndPos)
	{

		SJintHashIter foundIntHashIter;
		// insert to SJintHashNormal
		foundIntHashIter = SJintHashNormal[chrInt].find(spliceStartPos);
		if(foundIntHashIter == SJintHashNormal[chrInt].end())
		{
			set<int> newPosSet; 
			newPosSet.insert(spliceEndPos);
			SJintHashNormal[chrInt].insert(pair<int, set<int> > (spliceStartPos, newPosSet));
		}
		else
		{
			if((foundIntHashIter->second).find(spliceEndPos) == (foundIntHashIter->second).end())
			{
				(foundIntHashIter->second).insert(spliceEndPos);
			}
			else
			{}
		}

		//insert to SJintHashReverse
		foundIntHashIter = SJintHashReverse[chrInt].find(spliceEndPos);
		if(foundIntHashIter == SJintHashReverse[chrInt].end())
		{
			set<int> newPosSet; 
			newPosSet.insert(spliceStartPos);
			SJintHashReverse[chrInt].insert(pair<int, set<int> > (spliceEndPos, newPosSet));
		}
		else
		{
			if((foundIntHashIter->second).find(spliceStartPos) == (foundIntHashIter->second).end())
			{
				(foundIntHashIter->second).insert(spliceStartPos);
			}
			else
			{}
		}		
	}

	/*void insert2SJintHashAsClass(Splice_Junction& newSJ)
	{
		//int chrInt =
		//int spliceStartPos = 
		//int spliceEndPos = 
		//this->insert2SJintHash(chrInt, spliceStartPos, spliceEndPos)
	}*/

	/*void insert2SJareaHash(
		int chrInt, int spliceStartPos, int spliceEndPos)
	{
		int spliceStartPosArea = spliceStartPos/areaSize;
		int spliceEndPosArea = spliceEndPos/areaSize;

		SJareaHashIter foundAreaHashIter;
		
		// insert to SJstartPosAreaHash;
		foundAreaHashIter = SJstartPosAreaHash[chrInt].find(spliceStartPosArea);
		if(foundAreaHashIter == SJstartPosAreaHash[chrInt].end())
		{
			set<int> newPosSet;
			newPosSet.insert(spliceStartPos);
			SJstartPosAreaHash[chrInt].insert(pair<int, set<int> > (spliceStartPosArea, newPosSet));
		}
		else
		{
			if( (foundAreaHashIter->second).find(spliceStartPos) == (foundAreaHashIter->second).end() )
			{
				(foundAreaHashIter->second).insert(spliceStartPos);
			}
			else
			{}
		}

		
		// insert to SJendPosAreaHash;
		foundAreaHashIter = SJendPosAreaHash[chrInt].find(spliceEndPosArea);
		if(foundAreaHashIter == SJendPosAreaHash[chrInt].end())
		{
			set<int> newPosSet;
			newPosSet.insert(spliceEndPos);
			SJendPosAreaHash[chrInt].insert(pair<int, set<int> > (spliceEndPosArea, newPosSet));
		}
		else
		{
			if( (foundAreaHashIter->second).find(spliceStartPos) == (foundAreaHashIter->second).end() )
			{
				(foundAreaHashIter->second).insert(spliceStartPos);
			}
			else
			{}
		}
	}*/

	/*void insert2SJareaHashAsClass(Splice_Junction& newSJ)
	{
		//int chrInt =
		//int spliceStartPos = 
		//int spliceEndPos = 
		//this->insert2SJintHash(chrInt, spliceStartPos, spliceEndPos)
	}*/

	
};