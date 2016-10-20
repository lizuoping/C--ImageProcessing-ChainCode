#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>

using namespace std;

class CCimage {
	private:
		int numRows, numCols, minVal, maxVal;

	public:
		int **zeroFramedAry;
		CCimage(string input_fileName) {
			int row = 1, col = 1;
			string str;
			ifstream myfile(input_fileName);
			if (myfile.is_open()) {
				int order=0;
				while(myfile>>str) {
					order++;
					if(order==1) numRows = stoi(str);
					else if(order==2) {
						numCols = stoi(str);
						zeroFramedAry = new int*[numRows+2];
						for (int i = 0; i < numRows+2; i++)
							zeroFramedAry[i] = new int[numCols+2];
						for(int i=0; i<numRows+2; i++) {
							zeroFramedAry[i][0] = 0;
							zeroFramedAry[i][numCols+1] = 0;
						}
						for(int j=0; j<numCols+2; j++) {
							zeroFramedAry[0][j] = 0;
							zeroFramedAry[numRows+1][j] = 0;
						}
					} else if(order==3) minVal = stoi(str);
					else if(order==4) maxVal = stoi(str);
					else {
						zeroFramedAry[row][col++] = stoi(str);
						if(col>numCols) {
							row++;
							col=1;
						}
					}
				}
				myfile.close();
			} else {
				cout << "cannot open " << input_fileName << endl;
				exit(1);
			}
		}
};

class CCproperty {
	private:
		const int num_properties = 6;

	public:
		int maxCC=0;
		int ** property;
		CCproperty(string input_fileName) {
			int row = 0, col = 0;
			string str, temp[99];
			ifstream myfile(input_fileName);
			if (myfile.is_open()) {
				while(getline(myfile, str))
					temp[maxCC++] = str;
				property = new int*[maxCC];
				for(int i=0; i<maxCC; i++)
					property[i] = new int[num_properties];
				myfile.close();
			} else {
				cout << "cannot open " << input_fileName << endl;
				exit(1);
			}
			myfile.open(input_fileName);
			if (myfile.is_open()) {
				while(myfile>>str) {
					property[row][col++] = stoi(str);
					if(col==num_properties) {
						row++;
						col=0;
					}
				}
				myfile.close();
			} else {
				cout << "cannot open " << input_fileName << endl;
				exit(1);
			}
		}
};

class ChainCode {
	private:
		int neighborAry[8];
		int nextDirTable[8] = {6,0,0,2,2,4,4,6};
		int currentCC=0, lastQ, nextQ;
		struct point {
			int row, col;
		};
		point neighborCoord[8];

	public:
		string *chain_data;
		ChainCode(CCimage *image, CCproperty *pp) {
			chain_data = new string[pp->maxCC+1];
			int minRowOffset, maxRowOffset, minColOffset, maxColOffset;
			int startRow, startCol;
			for(int i=0; i<pp->maxCC; i++) {
				currentCC=i+1;
				minRowOffset=pp->property[i][2];
				maxRowOffset=pp->property[i][4];
				minColOffset=pp->property[i][3];
				maxColOffset=pp->property[i][5];
				startRow=minRowOffset;
				startCol=minColOffset;
				while(image->zeroFramedAry[startRow][startCol]!=currentCC) 
					++startCol;
				chain_data[currentCC]=to_string(startRow)+" "+to_string(startCol)+" "+to_string(currentCC)+" ";
				point startP;
				startP.row=startRow;
				startP.col=startCol;
				point currentP = startP;
				point nextP;
				lastQ=4;
				while((startP.row!=nextP.row) || (startP.col!=nextP.col)) {
					loadNeighbors(image, currentP.row, currentP.col);
					nextQ=(lastQ+1)%8;
					int Pchain = findNextP(currentP, nextQ);
					nextP = neighborCoord[Pchain];
					chain_data[currentCC]+=to_string(Pchain)+" ";
					currentP=nextP;
					lastQ=nextDirTable[(Pchain+7)%8];
				}
			}
		}

		void loadNeighborCoord(int row, int col) {
			neighborCoord[0].row = row;
			neighborCoord[0].col = col+1;
			neighborCoord[1].row = row-1;
			neighborCoord[1].col = col+1;
			neighborCoord[2].row = row-1;
			neighborCoord[2].col = col;
			neighborCoord[3].row = row-1;
			neighborCoord[3].col = col-1;
			neighborCoord[4].row = row;
			neighborCoord[4].col = col-1;
			neighborCoord[5].row = row+1;
			neighborCoord[5].col = col-1;
			neighborCoord[6].row = row+1;
			neighborCoord[6].col = col;
			neighborCoord[7].row = row+1;
			neighborCoord[7].col = col+1;
		}

		int findNextP(point p, int q) {
			int row = p.row;
			int col = p.col;
			loadNeighborCoord(row, col);
			int return_Pchain=0;
			for(int i=0; i<8; i++)
				if(neighborAry[(q+i)%8]==currentCC)
					return (q+i)%8;
			return return_Pchain;
		}

		void loadNeighbors(CCimage *image, int row, int col) {
			neighborAry[0]=image->zeroFramedAry[row][col+1];
			neighborAry[1]=image->zeroFramedAry[row-1][col+1];
			neighborAry[2]=image->zeroFramedAry[row-1][col];
			neighborAry[3]=image->zeroFramedAry[row-1][col-1];
			neighborAry[4]=image->zeroFramedAry[row][col-1];
			neighborAry[5]=image->zeroFramedAry[row+1][col-1];
			neighborAry[6]=image->zeroFramedAry[row+1][col];
			neighborAry[7]=image->zeroFramedAry[row+1][col+1];
		}
};

int main(int argc, char** argv) {
	CCimage *image = new CCimage(argv[1]);
	CCproperty *pp = new CCproperty(argv[2]);
	ChainCode *mychaincode = new ChainCode(image, pp);
	ofstream myfile(argv[3]);
	if (myfile.is_open()) {
		for(int i=1; i<=pp->maxCC; i++)	
			myfile<<mychaincode->chain_data[i]<<endl;
		myfile.close();
	} else {
		cout << "cannot write " << argv[3] << endl;
		exit(1);
	}
	myfile.open(argv[4]);
	if (myfile.is_open()) {
		myfile<<"for debugging"<<endl;
		myfile.close();
	} else {
		cout << "cannot write " << argv[4] << endl;
		exit(1);
	}
	delete image;
	delete pp;
	delete mychaincode;
	cout << "All work done!" << endl;
	return 0;
};
