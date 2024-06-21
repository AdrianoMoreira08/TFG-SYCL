/**
 * @brief Morphology class declaration, responsible for performing morphological
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

/**
 * @brief The Morphology class is responsible for performing morphological
 * operations on a binary image using a structuring element.
*/
class Morphology {
 public:
  Morphology(std::istream& infile1, std::istream& infile2);
  void consolePrintIMG();
  void consolePrintSE();
  void consolePrintMorph();
  void zeroFrame(std::istream& infile1);
  void morphFrame();
  void structEle(std::istream& infile2);
  void Erosion(std::ostream& outfile);
  ///////////////
  // This version is for OPENING operations, does not zeroing at start
  //  edits temp and passes it to DilationOpening
  ///////////////
  void ErosionO();
  // Closing for Erosion, no zeroing, and edits morphFrame using temp
  void ErosionC();
  void Dilation(std::ostream& outfile);
  // Dilation for Opening-  no zeroframing needed AND we create morphAry,
  // working with temp
  void DilationO();
  // Dilation for Closing
  // We generate the tempAry for erosionC to use afterwards
  void DilationC();
  void prettyPrint(std::ostream& outfile);
  void morphFrameZERO();
  void Opening(std::ostream& outfile);
  void Closing(std::ostream& outfile);
  void setSE(std::istream& infile);

 private:
  int numRowsIMG, numColsIMG, minValIMG, maxValIMG;
  int numRowsSE, numColsSE, minValSE, maxValSE;
  int rowOrigin, colOrigin;
  int** zeroFramedAry;
  int** morphFramedAry;
  int** structEleAry;
  int** tempFramedAry;
  int rowFrameSize, colFrameSize;
};
