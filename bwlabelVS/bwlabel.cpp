//  [10/13/2016 Dawn]
//	dawn.chli@gmail.com

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include "AssistantImage.h"
#include "FreeImage.h"
#include "bwlabel.h"

using namespace std;

int* souroundIndex;
int* souroundIndexWithCenter;

class ImageData{
public:
	int width;
	int height;
	int pixelSize;
	int* data;
	int* label;
	vector<int> groupMinLab;
	vector< vector<int> > groupLab;
	int  curLabCnt;
};

bool BoundaryCheck( ImageData& imgData,int x, int y )
{
	if ( x < 0 || x >= imgData.width || y < 0 || y >= imgData.height )
		return false;
	else
		return true;
}

int SurroundSumVal( ImageData& imgData, int idx, int idy )
{
	int sumVal = 0;
	for ( int i=0; i<4 ;i++ )
	{
		int newIdx = idx+souroundIndex[i*2+0];
		int newIdy = idy+souroundIndex[i*2+1];
		int newIndex = newIdy*imgData.width+newIdx;
		if ( BoundaryCheck( imgData, newIdx, newIdy ) )
		{
			if ( imgData.label[newIndex] >= 0 )
			{
				sumVal++;
			}
		}
	}
	return sumVal;
}

bool HasEleInVector( vector<int>& groupVec, int ele )
{
	for ( int i=0; i<groupVec.size() ;i++ )
	{
		if ( groupVec[i] == ele )
			return true;
	}
	return false;
}

void SurroundNewEquivalence( ImageData& imgData, int labVal, int idx, int idy )
{
	vector<int>& groupVec = imgData.groupLab[ labVal ];

	for ( int i=0; i<5 ;i++ )
	{
		int newIdx = idx+souroundIndexWithCenter[i*2+0];
		int newIdy = idy+souroundIndexWithCenter[i*2+1];
		int newIndex = newIdy*imgData.width+newIdx;
		if ( BoundaryCheck( imgData, newIdx, newIdy ) )
		{
			if ( imgData.label[newIndex] >= 0 )
			{
				if ( !HasEleInVector( groupVec, imgData.label[newIndex] )  )
				{
					groupVec.push_back( imgData.label[newIndex] );
				}

				vector<int>& neiGroupVec = imgData.groupLab[ imgData.label[newIndex] ];
				if( !HasEleInVector( neiGroupVec, labVal ) )
				{
					neiGroupVec.push_back( labVal );
				}
			}
		}
	}
}

int SurroundMinVal( ImageData& imgData, int idx, int idy )
{
	int minLabVal = 9999999;
	for ( int i=0; i<4 ;i++ )
	{
		int newIdx = idx+souroundIndex[i*2+0];
		int newIdy = idy+souroundIndex[i*2+1];
		int newIndex = newIdy*imgData.width+newIdx;
		if ( BoundaryCheck( imgData, newIdx, newIdy ) )
		{
			if ( imgData.label[newIndex] >= 0 && imgData.label[newIndex] < minLabVal )
			{
				minLabVal = imgData.label[newIndex];
			}
		}
	}
	return minLabVal;
}

void FirstPass( ImageData& imgData )
{
	int index = 0;
	for ( int i=0; i<imgData.height ;i++ )
	{
		for ( int j=0; j<imgData.width ;j++ )
		{
			if ( imgData.data[index] > 0 )
			{
				if ( SurroundSumVal(imgData, j, i) == 0 )
				{
					vector<int> newVec;
					imgData.groupLab.push_back( newVec );

					imgData.label[index] = imgData.curLabCnt;
					imgData.curLabCnt++;
				}
				else
				{
					imgData.label[index] = SurroundMinVal(imgData, j, i);
				}

				SurroundNewEquivalence( imgData, imgData.label[index], j, i );
			}
			index++;
		}
	}
}

void SecondPass( ImageData& imgData )
{
	imgData.groupMinLab.resize( imgData.curLabCnt, -1 );
	imgData.groupMinLab[ 0 ] = 0;

	int index = 0;
	for ( int i=0; i<imgData.groupLab.size() ;i++ )
	{
		for ( int j=0; j<imgData.groupLab[i].size(); j++ )
		{
			int subItem = imgData.groupLab[i][j];
			if ( imgData.groupMinLab[ subItem ] != -1 && imgData.groupMinLab[ subItem ] < i )
			{
				imgData.groupMinLab[i] = imgData.groupMinLab[subItem];
			}
		}

		if ( imgData.groupMinLab[i] == -1 )
		{
			imgData.groupMinLab[i] = i;
		}
	}

	index = 0;
	for ( int i=0; i<imgData.height ;i++ )
	{
		for ( int j=0; j<imgData.width ;j++ )
		{
			if ( imgData.label[index] >= 0 )
			{
				imgData.label[index] = imgData.groupMinLab[ imgData.label[index] ];
			}
			index++;
		}
	}
}

void PrintLabel( ImageData& imgData )
{
	int index = 0;
	for ( int i=0; i<imgData.height ;i++ )
	{
		for ( int j=0; j<imgData.width ;j++ )
		{
			cout << (int)(imgData.label[index]);
			cout << ' ';
			index++;
		}
		cout << '\n';
	}
}

int RunBwlabel(  const char* imageName, int threshold, bool showPass )
{
	souroundIndex = new int[8];
	souroundIndex[0] = -1;
	souroundIndex[1] = 0;
	souroundIndex[2] = 0;
	souroundIndex[3] = -1;
	souroundIndex[4] = +1;
	souroundIndex[5] = 0;
	souroundIndex[6] = 0;
	souroundIndex[7] = +1;

	souroundIndexWithCenter = new int[10];
	souroundIndexWithCenter[0] = -1;
	souroundIndexWithCenter[1] = 0;
	souroundIndexWithCenter[2] = 0;
	souroundIndexWithCenter[3] = -1;
	souroundIndexWithCenter[4] = +1;
	souroundIndexWithCenter[5] = 0;
	souroundIndexWithCenter[6] = 0;
	souroundIndexWithCenter[7] = +1;
	souroundIndexWithCenter[8] = 0;
	souroundIndexWithCenter[9] = 0;

	ImageData       imgData;
	imgData.curLabCnt = 0;

	CAssistantImage* tmpImage = new CAssistantImage( imageName );
	FIBITMAP* tmpData = tmpImage->GetPFiBitmap();
	char* pixeles = (char*)FreeImage_GetBits(tmpData);

	imgData.width = tmpImage->GetWidth();
	imgData.height = tmpImage->GetHeight();
	imgData.pixelSize = tmpImage->GetPixelSize();
	imgData.data = new int[ imgData.width * imgData.height ];
	imgData.label = new int[ imgData.width * imgData.height ];

	int index = 0;
	for( unsigned int y=0; y < imgData.height ;y++ )
	{   
		unsigned char* bits = FreeImage_GetScanLine( tmpData,imgData.height - y -1 );

		for( unsigned int x=0; x<imgData.width ;x++ )
		{
			if( bits[ x*imgData.pixelSize+0 ] > threshold)
				imgData.data[index] = 1;
			else
				imgData.data[index] = 0;

			imgData.label[index] = -1;

			index++;
		}
	}

	FirstPass(imgData);

	index = 0;
	for( unsigned int y=0; y < imgData.height ;y++ )
	{   
		unsigned char* bits = FreeImage_GetScanLine( tmpData,imgData.height - y -1 );

		for( unsigned int x=0; x<imgData.width ;x++ )
		{
			if( imgData.label[index] >= 0 )
			{
				int result = ((imgData.label[index] + 1) * 16) % 256;
				if ( tmpImage->GetPixelSize() == 1 )
				{
					bits[ x ] = result;
				}
				else
				{
					bits[ x*4+0 ] = result;
					bits[ x*4+1 ] = result;
					bits[ x*4+2 ] = result;
				}
			}
			else
			{
				if ( tmpImage->GetPixelSize() == 1 )
				{
					bits[ x+0 ] = 0;
				}
				else
				{
					bits[ x*4+0 ] = 0;
					bits[ x*4+1 ] = 0;
					bits[ x*4+2 ] = 0;
				}
			}

			index++;
		}
	}

	if (showPass)
		tmpImage->Save( "pass1.png", tmpImage->GetBpp() );

	SecondPass(imgData);

	index = 0;
	for( unsigned int y=0; y < imgData.height ;y++ )
	{   
		unsigned char* bits = FreeImage_GetScanLine( tmpData,imgData.height - y -1 );

		for( unsigned int x=0; x<imgData.width ;x++ )
		{
			if( imgData.label[index] >= 0 )
			{
				int result = ((imgData.label[index] + 1) * 16) % 256;
				if ( tmpImage->GetPixelSize() == 1 )
				{
					bits[ x ] = result;
				}
				else
				{
					bits[ x*4+0 ] = result;
					bits[ x*4+1 ] = result;
					bits[ x*4+2 ] = result;
				}
			}
			else
			{
				if ( tmpImage->GetPixelSize() == 1 )
				{
					bits[ x+0 ] = 0;
				}
				else
				{
					bits[ x*4+0 ] = 0;
					bits[ x*4+1 ] = 0;
					bits[ x*4+2 ] = 0;
				}
			}

			index++;
		}
	}

	if (showPass)
		tmpImage->Save( "pass2.png", tmpImage->GetBpp() );

	delete tmpImage;
	delete[] souroundIndex;
	delete[] souroundIndexWithCenter;
	delete[] imgData.label;
	delete[] imgData.data;

	return 0;
}