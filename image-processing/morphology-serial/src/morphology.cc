/**
 * @brief Morphology class definition, responsible for performing morphological
 * operations on a binary image using a structuring element.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @author Stefan Hernandez (Original author)
 *
 * @note: Based on the works of Stefan Hernandez
 * (https://github.com/StefanHernandez/Morphology)
 */

#include <fstream>
#include <iostream>

#include "../include/morphology.h"

/**
 * @brief The Morphology class is responsible for performing morphological
 * operations on a binary image using a structuring element.
*/
Morphology::Morphology(std::istream& infile1, std::istream& infile2) {
  infile1 >> numRowsIMG >> numColsIMG >> minValIMG >> maxValIMG;
  infile2 >> numRowsSE >> numColsSE >> minValSE >> maxValSE >> rowOrigin >>
      colOrigin;
  rowFrameSize = numRowsSE - 1;
  colFrameSize = numColsSE - 1;
  zeroFrame(infile1);
  morphFrame();
  structEle(infile2);
}

void Morphology::consolePrintIMG() {
  std::cout << "IMAGE: " << std::endl;
  std::cout << numRowsIMG << " " << numColsIMG << " " << minValIMG << " "
        << maxValIMG << std::endl;

  for (int i = rowFrameSize; i < numRowsIMG + rowFrameSize; i++) {
    for (int j = colFrameSize; j < numColsIMG + colFrameSize; j++) {
      std::cout << zeroFramedAry[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl;
}

void Morphology::consolePrintSE() {
  std::cout << "Structuring Element:" << std::endl;
  std::cout << numRowsSE << " " << numColsSE << " " << minValSE << " " << maxValSE
        << std::endl;
  std::cout << rowOrigin << " " << colOrigin << std::endl;

  for (int i = 0; i < numRowsSE; i++) {
    for (int j = 0; j < numColsSE; j++) {
      std::cout << structEleAry[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl;
}

void Morphology::consolePrintMorph() {
  std::cout << numRowsIMG << " " << numColsIMG << " " << minValIMG << " "
        << maxValIMG << std::endl;

  for (int i = rowFrameSize; i < numRowsIMG + rowFrameSize; i++) {
    for (int j = colFrameSize; j < numColsIMG + colFrameSize; j++) {
      std::cout << morphFramedAry[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl;
}

/////////////////
// Framing Image /
/////////////////
void Morphology::zeroFrame(std::istream& infile1) {
  zeroFramedAry = new int*[numRowsIMG + (2 * rowFrameSize)];
  for (int r = 0; r < (numRowsIMG + (2 * rowFrameSize)); r++) {
    zeroFramedAry[r] = new int[numColsIMG + (2 * colFrameSize)];
  }

  for (int k = 0; k < numRowsIMG + (2 * rowFrameSize); k++) {
    for (int l = 0; l < rowFrameSize; l++) {
      zeroFramedAry[k][l] = 0;
      zeroFramedAry[k][numColsIMG + (colFrameSize * 2) - 1 - l] = 0;
    }
  }

  for (int j = 0; j <= numColsIMG + (2 * colFrameSize); j++) {
    for (int m = 0; m < colFrameSize; m++) {
      zeroFramedAry[m][j] = 0;
      zeroFramedAry[numRowsIMG + (rowFrameSize * 2) - 1 - m][j] = 0;
    }
  }

  // Load image
  int data;
  for (int r = rowFrameSize; r < numRowsIMG + rowFrameSize; r++) {
    for (int c = colFrameSize; c < numColsIMG + colFrameSize; c++) {
      infile1 >> data;
      zeroFramedAry[r][c] = data;
    }
  }
}

/////////////////
// Framing Morph /
/////////////////
void Morphology::morphFrame() {
  morphFramedAry = new int*[numRowsIMG + (2 * rowFrameSize)];
  for (int r = 0; r < (numRowsIMG + (2 * rowFrameSize)); r++) {
    morphFramedAry[r] = new int[numColsIMG + (2 * colFrameSize)];
  }

  for (int i = 0; i < (numRowsIMG + (2 * rowFrameSize)); i++) {
    for (int j = 0; j < (numColsIMG + (2 * colFrameSize)); j++) {
      morphFramedAry[i][j] = 0;
    }
  }

  // We also set up the temp for opening/closing
  tempFramedAry = new int*[numRowsIMG + (2 * rowFrameSize)];
  for (int r = 0; r < (numRowsIMG + (2 * rowFrameSize)); r++) {
    tempFramedAry[r] = new int[numColsIMG + (2 * colFrameSize)];
  }

  for (int i = 0; i < (numRowsIMG + (2 * rowFrameSize)); i++) {
    for (int j = 0; j < (numColsIMG + (2 * colFrameSize)); j++) {
      tempFramedAry[i][j] = 0;
    }
  }
}

//////////////////
// Struct Element /
//////////////////
void Morphology::structEle(std::istream& infile2) {
  structEleAry = new int*[numRowsSE];
  for (int r = 0; r < (numRowsSE); r++) {
    structEleAry[r] = new int[numColsSE];
  }

  // Load image
  int data;
  for (int r = 0; r < numRowsSE; r++) {
    for (int c = 0; c < numColsSE; c++) {
      infile2 >> data;
      structEleAry[r][c] = data;
    }
  }
}

//////////////////
//   EROSION     /
//////////////////

void Morphology::Erosion(std::ostream& outfile) {
  morphFrameZERO();
  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (zeroFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Erosion /
        ////////////////////

        int flag = 1;  // Will use this as a boolean flag for whether it
                       // passes or not

        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              if (zeroFramedAry[i][j] != structEleAry[seRow][seCol]) {
                flag = 0;
              }
            }
          }
        }
        // std::cout<<std::endl<<std::endl;

        morphFramedAry[currentRow][currentCol] = flag;
      }
    }
  }
  // std::cout << "Erosion Result " << std::endl;
  // consolePrintMorph();

  prettyPrint(outfile);
}

///////////////
// This version is for OPENING operations, does not zeroing at start
//  edits temp and passes it to DilationOpening
///////////////
void Morphology::Morphology::ErosionO() {
  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (zeroFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Erosion /
        ////////////////////

        int flag = 1;  // Will use this as a boolean flag for whether it
                        // passes or not

        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              if (zeroFramedAry[i][j] != structEleAry[seRow][seCol]) {
                flag = 0;
              }
            }
          }
        }
        // std::cout<<std::endl<<std::endl;

        tempFramedAry[currentRow][currentCol] = flag;
      }
    }
  }
}
// Closing for Erosion, no zeroing, and edits morphFrame using temp
void Morphology::ErosionC() {
  // Should start from row = rowOrigin and end at <(numRowsIMG+2)-(numRowsSE-1
  // = colOrigin until

  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (tempFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Erosion /
        ////////////////////

        int flag = 1;  // Will use this as a boolean flag for whether it
                        // passes or not

        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              if (tempFramedAry[i][j] != structEleAry[seRow][seCol]) {
                flag = 0;
              }
            }
          }
        }

        morphFramedAry[currentRow][currentCol] = flag;
      }
    }
  }
}

void Morphology::Dilation(std::ostream& outfile) {
  morphFrameZERO();

  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (zeroFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Dilation /
        ////////////////////

        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              morphFramedAry[i][j] = structEleAry[seRow][seCol];
            }
          }
        }

      }  // end if
    }
  }

  // std::cout << "Dilation Result " << std::endl;
  // consolePrintMorph();

  prettyPrint(outfile);
}

// Dilation for Opening-  no zeroframing needed AND we create morphAry,
// working with temp
void Morphology::DilationO() {
  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (tempFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Dilation /
        ////////////////////

        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              morphFramedAry[i][j] = structEleAry[seRow][seCol];
            }
          }
        }

      }  // end if
    }
  }
}

// Dilation for Closing
// We generate the tempAry for erosionC to use afterwards
void Morphology::DilationC() {
  for (int currentRow = rowFrameSize; currentRow < numRowsIMG + rowFrameSize;
        currentRow++) {
    for (int currentCol = colFrameSize;
          currentCol < numColsIMG + colFrameSize; currentCol++) {
      if (zeroFramedAry[currentRow][currentCol] ==
          structEleAry[rowOrigin][colOrigin]) {
        ////////////////////
        // Test for Dilation /
        ////////////////////
        int rowStart = (currentRow - rowOrigin);
        int rowEnd = (rowStart + numRowsSE);
        int colStart = (currentCol - colOrigin);
        int colEnd = (colStart + numColsSE);

        int seRow;
        int seCol;

        for (int i = rowStart; i < rowEnd; i++) {
          for (int j = colStart; j < colEnd; j++) {
            seRow = (numRowsSE - (rowEnd - i));
            seCol = (numColsSE - (colEnd - j));

            if (structEleAry[seRow][seCol] == 1) {
              tempFramedAry[i][j] = structEleAry[seRow][seCol];
            }
          }
        }

      }  // end if
    }
  }
}

void Morphology::prettyPrint(std::ostream& outfile) {
  outfile << numRowsIMG << " " << numColsIMG << " " << minValIMG << " "
          << maxValIMG << std::endl;

  for (int i = 1; i < numRowsIMG + 1; i++) {
    for (int j = 1; j < numColsIMG + 1; j++) {
      outfile << morphFramedAry[i][j] << " ";
    }
    outfile << std::endl;
  }
}

void Morphology::morphFrameZERO() {
  for (int i = 0; i < (numRowsIMG + (2 * rowFrameSize)); i++) {
    for (int j = 0; j < (numColsIMG + (2 * colFrameSize)); j++) {
      morphFramedAry[i][j] = 0;
    }
  }
}

void Morphology::Opening(std::ostream& outfile) {
  morphFrameZERO();

  ErosionO();
  DilationO();

  // std::cout << "Opening Result " << std::endl;
  // consolePrintMorph();

  prettyPrint(outfile);
}

void Morphology::Closing(std::ostream& outfile) {
  morphFrameZERO();

  DilationC();
  ErosionC();

  // std::cout << "Closing Result " << std::endl;
  // consolePrintMorph();

  prettyPrint(outfile);
}

void Morphology::setSE(std::istream& infile) {}
