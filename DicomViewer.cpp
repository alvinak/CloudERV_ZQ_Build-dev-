// DicomViewer.cpp : implementation file
//

#include "stdafx.h"
#include "DicomViewer.h"
#include "resource.h"
#include <math.h>

#include "CalibrateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#if _MSC_VER >= 1300    // for VC 7.0
  // from ATL 7.0 sources
  #ifndef _delayimp_h
  extern "C" IMAGE_DOS_HEADER __ImageBase;
  #endif
#endif


#define PI 3.1415926


HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)

  // Here's a trick that will get you the handle of the module
  // you're running in without any a-priori knowledge:
  // http://www.dotnet247.com/247reference/msgs/13/65259.aspx

  MEMORY_BASIC_INFORMATION mbi;
  static int dummy;
  VirtualQuery( &dummy, &mbi, sizeof(mbi) );

  return reinterpret_cast<HMODULE>(mbi.AllocationBase);

#else    // VC 7.0

  // from ATL 7.0 sources

  return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}


char g_szUnregistered[] = 
{
	(char) 0x59,(char) 0x78,(char) 0x63,(char) 0x37,(char) 0x45,(char) 0x72,(char) 0x70,(char) 0x7E,(char) 0x64,(char) 0x63,(char) 0x72,(char) 0x65,(char) 0x72,(char) 0x73,(char) 0x36,(char) 0x00
};



CString ProcessTextA(const TCHAR *szText)
{
	CString sText("");
	int nPos;

	sText = szText;

	
	nPos = sText.Find(_T("$)A"));
	
	if (nPos >= 0)
	{
		sText = sText.Mid(nPos + 3);
	}

	nPos = sText.Find(_T("="));

	if (nPos >= 0)
	{
		sText = sText.Mid(nPos + 1);
	}
	
	return sText;
}


CString ProcessTextB(const TCHAR *szText)
{
	CString sText("");
	TCHAR szStrA[5] = {_T(')'),_T('$'),_T(')'),_T('A'),_T('\0')};// 0x1B,$)A
	TCHAR szStrB[5] = {_T(''),_T('('),_T('B'),_T('\0')};	 // 0x1B,(B
	int nPos1,nPos2;

	sText = szText;


	nPos1 = sText.Find(szStrA);
	nPos2 = sText.Find(szStrB);

	while (nPos1 >= 0 || nPos2 >= 0)
	{
		if (nPos1 >= 0)
		{
			sText.Delete(nPos1,_tcslen(szStrA));
		}
		else if (nPos2 >= 0)
		{
			sText.Delete(nPos2,_tcslen(szStrB));
		}

		nPos1 = sText.Find(szStrA);
		nPos2 = sText.Find(szStrB);
	}

	return sText;
}



CsliceProcessing::CsliceProcessing()
{

}

CsliceProcessing::~CsliceProcessing()
{

}

bool CsliceProcessing::ProjectSlice(CString scoutPos, 
							   CString scoutOrient, 
							   CString scoutPixSpace, 
							   int scoutRows, 
							   int scoutCols, 
							   CString imgPos, 
							   CString imgOrient, 
							   CString imgPixSpace, 
							   int imgRows, 
							   int imgCols)
{
	//
	// the routine receives the coordinate and position information as appears in the DICOM header.  
	// The scout and image positional information is first chacked for correctness. 
	// It is assumed to be correct if the the coordinates contain 3 float numbers separated by '\'
	// The vectors must form a unit vector with each triplet.  Each image orient vector must contain 
	// two triplets
	// The pixel spacing is assumed to be in mm
	// The rows and columns must be > 0
	// The object stores the results of each calculation a new call to project slice will wipe out the results of the previous calculation
	// if any of the positional information is ommitted (null or 0 length string.) the previous value will be used

	// Obviously one needs to perform a parallel projection of the plane and extent
	// of the source slice onto the plane of the target localizer image. One can think
	// of various ways of calculating angles between planes, dropping normals, etc.,
	// but there is a simple approach ...

	// If one considers the localizer plane as a "viewport" onto the DICOM 3D
	// coordinate space, then that viewport is described by its origin, its
	// row unit vector, column unit vector and a normal unit vector (derived
	// from the row and column vectors by taking the cross product). Now if
	// one moves the origin to 0,0,0 and rotates this viewing plane such that
	// the row vector is in the +X direction, the column vector the +Y direction,
	// and the normal in the +Z direction, then one has a situation where the
	// X coordinate now represents a column offset in mm from the localizer's
	// top left hand corner, and the Y coordinate now represents a row offset in
	// mm from the localizer's top left hand corner, and the Z coordinate can be
	// ignored. One can then convert the X and Y mm offsets into pixel offsets
	// using the pixel spacing of the localizer image.

	// This trick is neat, because the actual rotations can be specified entirely
	// using the direction cosines that are the row, column and normal unit vectors,
	// without having to figure out any angles, arc cosines and sines, which octant
	// of the 3D space one is dealing with, etc. Indeed, simplified it looks like:

	//   dst_nrm_dircos_x = dst_row_dircos_y * dst_col_dircos_z - dst_row_dircos_z * dst_col_dircos_y;
	//   dst_nrm_dircos_y = dst_row_dircos_z * dst_col_dircos_x - dst_row_dircos_x * dst_col_dircos_z;
	//   dst_nrm_dircos_z = dst_row_dircos_x * dst_col_dircos_y - dst_row_dircos_y * dst_col_dircos_x;

	//   src_pos_x -= dst_pos_x;
	//   src_pos_y -= dst_pos_y;
	//   src_pos_z -= dst_pos_z;

	//   dst_pos_x = dst_row_dircos_x * src_pos_x
	// 		  + dst_row_dircos_y * src_pos_y
	// 		  + dst_row_dircos_z * src_pos_z;

	//   dst_pos_y = dst_col_dircos_x * src_pos_x
	// 		  + dst_col_dircos_y * src_pos_y
	// 		  + dst_col_dircos_z * src_pos_z;

	//   dst_pos_z = dst_nrm_dircos_x * src_pos_x
	// 		  + dst_nrm_dircos_y * src_pos_y
	// 		  + dst_nrm_dircos_z * src_pos_z;

	// The traditional homogeneous transformation matrix form of this is:

	//   [ dst_row_dircos_x   dst_row_dircos_y   dst_row_dircos_z   -dst_pos_x ]
	//   [                                                                     ]
	//   [ dst_col_dircos_x   dst_col_dircos_y   dst_col_dircos_z   -dst_pos_y ]
	//   [                                                                     ]
	//   [ dst_nrm_dircos_x   dst_nrm_dircos_y   dst_nrm_dircos_z   -dst_pos_z ]
	//   [                                                                     ]
	//   [ 0                  0                  0                  1          ]

	// So this tells you how to transform arbitrary 3D points into localizer pixel
	// offset space (which then obviously need to be clipped to the localizer
	// boundaries for drawing), but which points to draw ?

	// My approach was to project the square that is the bounding box of the source
	// image (i.e. lines joining the TLHC, TRHC,BRHC and BLHC of the slice). That way,
	// if the slice is orthogonal to the localizer the square will project as a single
	// line (i.e. all four lines will pile up on top of each other), and if it is not,
	// some sort of rectangle or trapezoid will be drawn. I rather like the effect and
	// it provides a general solution, though looks messy with a lot of slices with
	// funky angulations. Other possibilities are just draw the longest projected side,
	// draw a diagonal, etc.

	// Anyway, as I mentioned, I am no math whiz, but this approach seems to work
	// for all the cases I have tried. I would be interested to hear if this method
	// is flawed or if anyone has a better solution.

	// Locals
	bool retVal = true;

	// Fisrs step check if either the scout or the image has to be updated.

	if (scoutPos && *scoutPos && scoutOrient && *scoutOrient && scoutPixSpace && *scoutPixSpace && scoutRows && scoutCols) {
		// scout parameters appear to be semi-valid try to update the scout information
		if (setScoutPosition(scoutPos) && setScoutOrientation(scoutOrient) && setScoutSpacing(scoutPixSpace)) {
			_mScoutRows = scoutRows;
			_mScoutCols = scoutCols;
			setScoutDimensions();
			retVal = normalizeScout();
		}
	}
	//  Image and scout information is independent of one and other
	if (imgPos && *imgPos && imgOrient && *imgOrient && imgPixSpace && *imgPixSpace && imgRows && imgCols) {
		// Img parameters appear to be semi-valid try to update the Img information
		if (setImgPosition(imgPos) && setImgOrientation(imgOrient) && setImgSpacing(imgPixSpace)) {
			_mImgRows = imgRows;
			_mImgCols = imgCols;
			setImgDimensions();
		}
	}
	if (retVal) {
		// start the calculation of the projected bounding box and the ends of the axes along the sides.
		calculateBoundingBox();
		calculateAxisPoints();
	}
	return(retVal);

}

bool CsliceProcessing::getBoundingBox(int &Ulx, int &Uly, int &Urx, int &Ury, int &Llx, int &Lly, int &Lrx, int &Lry)
{
	// retreive the calculated bounding box coordinates.  
	Ulx = _mBoxUlx;
	Uly = _mBoxUly;
	Urx = _mBoxUrx;
	Ury = _mBoxUry;
	Llx = _mBoxLlx;
	Lly = _mBoxLly;
	Lrx = _mBoxLrx;
	Lry = _mBoxLry;
	return (true);
}
bool CsliceProcessing::getAxisPoints(int &TopX, int &TopY, int &BottomX, int &BottomY, int &LeftX, int &LeftY, int &RightX, int &RightY)
{
	// retreive the axis endpoints
	TopX		= _mAxisTopx;
	TopY		= _mAxisTopy;
	BottomX		= _mAxisBottomx;
	BottomY		= _mAxisBottomy;
	LeftX		= _mAxisLeftx;
	LeftY		= _mAxisLefty;
	RightX		= _mAxisRightx;
	RightY		= _mAxisRighty;
	return(true);
}

bool CsliceProcessing::setScoutPosition(const TCHAR *Pos)
{
	bool retVal= true;
	// set the member variables 
	// _mScoutx, _mScouty, _mScoutz to the actual position information
	// if the input pointer is null or the string is empty set all position to 0 and
	// set the position valid flag (_mScoutvalid) to false
	retVal = checkPosString(Pos);
	if (retVal && Pos && *Pos) {
		// the position information contains valid data.  It has been checked prior to
		// the activation of theis member function
		_stscanf (Pos, _T("%f\\%f\\%f"), &_mScoutx, &_mScouty, &_mScoutz);
		_mScoutValid = true;
	} else {
		// The Pos contains no valid information it is assumed that the sout position is to be clweared of all valid 
		// entries
		clearScout();
	}
	return (retVal);

}

bool CsliceProcessing::setScoutOrientation(const TCHAR *Pos)
{
	bool retVal;
	// Scna the sout orientation vactor into the local variables and chack it for validity
	retVal = checkVectorString(Pos);
	if (retVal) {
		_stscanf(Pos, _T("%f\\%f\\%f\\%f\\%f\\%f"), &_mScoutRowCosx, &_mScoutRowCosy, &_mScoutRowCosz,
			   &_mScoutColCosx, &_mScoutColCosy, &_mScoutColCosz);
		_mScoutValid = checkScoutVector();
		if (!_mScoutValid) {
			clearScout();
		}
	}
	return (retVal);
}

bool CsliceProcessing::setImgPosition(const TCHAR *Pos)
{
	bool retVal= true;
	// set the member variables 
	// _mImgx, _mImgy, _mImgz to the actual position information
	// if the input pointer is null or the string is empty set all position to 0 and
	// set the position valid flag (_mImgvalid) to false
	retVal = checkPosString(Pos);
	if (retVal && Pos && *Pos) {
		// the position information contains valid data.  It has been checked prior to
		// the activation of theis member function
		_stscanf (Pos, _T("%f\\%f\\%f"), &_mImgx, &_mImgy, &_mImgz);
		_mImgValid = true;
	} else {
		// The Pos contains no valid information it is assumed that the sout position is to be clweared of all valid 
		// entries
		clearImg();
	}
	return (retVal);
}

bool CsliceProcessing::setImgOrientation(const TCHAR *Pos)
{
	bool retVal;
	// Scna the sout orientation vactor into the local variables and chack it for validity
	retVal = checkVectorString(Pos);
	if (retVal) {
		_stscanf(Pos, _T("%f\\%f\\%f\\%f\\%f\\%f"), &_mImgRowCosx, &_mImgRowCosy, &_mImgRowCosz,
			   &_mImgColCosx, &_mImgColCosy, &_mImgColCosz);
		_mImgValid = checkImgVector();
		if (!_mImgValid) {
			clearImg();
		}
	}
	return (retVal);
}

bool CsliceProcessing::checkScoutVector()
{
	bool retVal;

	// check the row vector and check the column vector
	retVal = checkVector(_mScoutRowCosx, _mScoutRowCosy, _mScoutRowCosz);
	retVal = checkVector(_mScoutColCosx, _mScoutColCosy, _mScoutColCosz);
	return (retVal);
}

bool CsliceProcessing::checkImgVector()
{
	bool retVal;

	// check the row vector and check the column vector
	retVal = checkVector(_mImgRowCosx, _mImgRowCosy, _mImgRowCosz);
	retVal = checkVector(_mImgColCosx, _mImgColCosy, _mImgColCosz);
	return (retVal);

}

void CsliceProcessing::clearScout()
{
	// clear all the scout parameters and set the scout valid flag to false.
	_mScoutRowCosx = 0;
	_mScoutRowCosy = 0;
	_mScoutRowCosz = 0;
	_mScoutColCosx = 0;
	_mScoutColCosy = 0;
	_mScoutColCosz = 0;
	_mScoutx = 0;
	_mScouty = 0;
	_mScoutz = 0;
	_mScoutValid = false;
}

void CsliceProcessing::clearImg()
{
	// clear all the image prameters and set the image valid flag to false.
	_mImgRowCosx = 0;
	_mImgRowCosy = 0;
	_mImgRowCosz = 0;
	_mImgColCosx = 0;
	_mImgColCosy = 0;
	_mImgColCosz = 0;
	_mImgx = 0;
	_mImgy = 0;
	_mImgz = 0;
	_mImgValid = false;
}

bool CsliceProcessing::checkPosString(const TCHAR *Pos)
{
	TCHAR *t1,*t2, *cp;
	bool retVal = true;

	// check if the string contains three valid floating point numbers separated by '\' characters.
	// The firat step in the process is to tokenize the string.  Find the occurances of '\'
	cp = (TCHAR*)Pos;
	t1 = _tcsstr(cp, _T("\\"));
	if (t1) {
		t2 = _tcsstr(t1+1, _T("\\"));
		if (t2) {
			// at this point we have found the two "\" string.  T1 points to the firs occurence and t2 point to
			// second occurence of the "\" string.   Check if all the characters point to 
			// valid numberical values [0..9, -,=,.e,E]
			for (cp = (TCHAR*)Pos ; *cp; cp++) {
				if ((*cp < _T('0') ||
					 *cp > _T('9') ) &&
					*cp != _T('-') &&
					*cp != _T('+') &&
					*cp != _T('.') &&
					*cp != _T('e') &&
					*cp != _T('E') &&
					*cp != _T('\\')) {
					retVal = false;
				}
			}
		} else {
			// second token was not located
			retVal = false;
		}
	} else {
		// first token was not located
		retVal = false;
	}
	return (retVal);
}

bool CsliceProcessing::checkVectorString(const TCHAR *Pos)
{
	// The routine checks if the position string is valid.  To be a valid position string it shall contain 6 
	// float numbers separated by '\' 
	TCHAR *t[5];  // separators
	int idx;
	TCHAR *cp;
	bool retVal = true;
	//  Locate all the separators.  Make sure there are five of them
	if (Pos && *Pos) {
		for (cp = (TCHAR*)Pos, idx = 0; idx < 5 ; idx++)  {
			cp = t[idx] = _tcsstr (cp, _T("\\"));
			if (!cp) {
				retVal = false;
				break;
			}
			cp++;
		}
		// check for the valid characters alloved in the Position string [0..9,-,+,.,e,E]
		for (cp = (TCHAR*)Pos ; *cp; cp++) {
			if ((*cp < _T('0') ||
				 *cp > _T('9') ) &&
				*cp != _T('-') &&
				*cp != _T('+') &&
				*cp != _T('.') &&
				*cp != _T('e') &&
				*cp != _T('E') &&
				*cp != _T('\\')) {
				retVal = false;
				break;
			}
		}
	}
	return (retVal);
}

bool CsliceProcessing::checkVector(float CosX, float CosY, float CosZ)
{
	// Check if the vector passed is a unit vector
	if (fabs(CosX*CosX+CosY*CosY+CosZ*CosZ-1) < 0) {
		return (false);
	} else {
		return (true);
	}
}


bool CsliceProcessing::normalizeScout()
{
	// first create the scout normal vector
	_mNrmCosX = _mScoutRowCosy * _mScoutColCosz - _mScoutRowCosz * _mScoutColCosy;
	_mNrmCosY = _mScoutRowCosz * _mScoutColCosx - _mScoutRowCosx * _mScoutColCosz;
	_mNrmCosZ = _mScoutRowCosx * _mScoutColCosy - _mScoutRowCosy * _mScoutColCosx;
	return (checkVector(_mNrmCosX, _mNrmCosY, _mNrmCosZ));
}

bool CsliceProcessing::setScoutSpacing(const TCHAR *Spacing)
{
	//  Convert the pixelspacing for the scout image and return true if both values are > 0
	// the pixel spacing is specified in adjacent row/adjacent column spacing
	// in this code ..xSpacing refers to column spacing
	_stscanf (Spacing, _T("%f\\%f"), &_mScoutySpacing, &_mScoutxSpacing);
	if (_mScoutxSpacing == 0 || _mScoutySpacing == 0) {
		return (false);
	} else {
		return (true);
	}
}

bool CsliceProcessing::setImgSpacing(const TCHAR *Spacing)
{
	//  Convert the pixelspacing for the Img image and return true if both values are > 0
	// the pixel spacing is specified in adjacent row/adjacent column spacing
	// in this code ..xSpacing refers to column spacing
	_stscanf (Spacing, _T("%f\\%f"), &_mImgySpacing, &_mImgxSpacing);
	if (_mImgxSpacing == 0 || _mImgySpacing == 0) {
		return (false);
	} else {
		return (true);
	}
}

bool CsliceProcessing::rotateImage(float imgPosx, 
							  float imgPosy, 
							  float imgPosz, 
							  float &scoutPosx, 
							  float &scoutPosy, 
							  float &scoutPosz)
{
	// projet the pioints passed into the space of the normalized scout image

	scoutPosx = _mScoutRowCosx*imgPosx + _mScoutRowCosy*imgPosy + _mScoutRowCosz*imgPosz;
	scoutPosy = _mScoutColCosx*imgPosx + _mScoutColCosy*imgPosy + _mScoutColCosz*imgPosz;
	scoutPosz = _mNrmCosX*imgPosx + _mNrmCosY*imgPosy + _mNrmCosZ*imgPosz;
	return (true);
}


bool CsliceProcessing::setScoutDimensions()
{
	_mScoutRowLen = _mScoutRows * _mScoutxSpacing;
	_mScoutColLen = _mScoutCols * _mScoutySpacing;
	return(true);
}

bool CsliceProcessing::setImgDimensions()
{
	_mImgRowLen = _mImgRows * _mImgxSpacing;
	_mImgColLen = _mImgCols * _mImgySpacing;
	return(true);
}

void CsliceProcessing::calculateBoundingBox()
{
	// the four points in 3d space that defines the corners of the bounding box
	float posX[4];
	float posY[4];
	float posZ[4];
	int rowPixel[4];
	int colPixel[4];
	int i;

	// upper left hand Corner
	posX[0] = _mImgx;
	posY[0] = _mImgy;
	posZ[0] = _mImgz;

	// upper right hand corner

	posX[1] = posX[0] + _mImgRowCosx*_mImgRowLen;
	posY[1] = posY[0] + _mImgRowCosy*_mImgRowLen;
	posZ[1] = posZ[0] + _mImgRowCosz*_mImgRowLen;

	// Buttom right hand corner

	posX[2] = posX[1] + _mImgColCosx*_mImgColLen;
	posY[2] = posY[1] + _mImgColCosy*_mImgColLen;
	posZ[2] = posZ[1] + _mImgColCosz*_mImgColLen;
	
	// bottom left hand corner

	posX[3] = posX[0] + _mImgColCosx*_mImgColLen;
	posY[3] = posY[0] + _mImgColCosy*_mImgColLen;
	posZ[3] = posZ[0] + _mImgColCosz*_mImgColLen;

	// Go through all four corners

	for (i = 0; i < 4; i++) {
		// we want to view the source slice from the "point of view" of
		// the target localizer, i.e. a parallel projection of the source
		// onto the target

		// do this by imaging that the target localizer is a view port
		// into a relocated and rotated co-ordinate space, where the
		// viewport has a row vector of +X, col vector of +Y and normal +Z,
		// then the X and Y values of the projected target correspond to
		// row and col offsets in mm from the TLHC of the localizer image !

		// move everything to origin of target
		posX[i] -= _mScoutx;
		posY[i] -= _mScouty;
		posZ[i] -= _mScoutz;

		rotateImage(posX[i], posY[i], posZ[i],posX[i], posY[i], posZ[i]);
		// at this point the position contains the location on the scout image. calculate the pixel position
		// dicom coordinates are center of pixel 1\1
		colPixel[i] = int(posX[i]/_mScoutySpacing + 0.5);
		rowPixel[i] = int(posY[i]/_mScoutxSpacing + 0.5);
	}
	//  sort out the column and row pixel coordinates into the bounding box named coordinates
	// same order as the position ULC -> URC -> BRC -> BLC
	_mBoxUlx = colPixel[0];
	_mBoxUly = rowPixel[0];
	_mBoxUrx = colPixel[1];
	_mBoxUry = rowPixel[1];
	_mBoxLrx = colPixel[2];
	_mBoxLry = rowPixel[2];
	_mBoxLlx = colPixel[3];
	_mBoxLly = rowPixel[3];
}

void CsliceProcessing::calculateAxisPoints()
{

	// the four points in 3d space that defines the corners of the bounding box
	float posX[4];
	float posY[4];
	float posZ[4];
	int rowPixel[4];
	int colPixel[4];
	int i;

	// upper center
	posX[0] = _mImgx + _mImgRowCosx*_mImgRowLen/2;
	posY[0] = _mImgy + _mImgRowCosy*_mImgRowLen/2;
	posZ[0] = _mImgz + _mImgRowCosz*_mImgRowLen/2;

	// right hand center

	posX[1] = _mImgx + _mImgRowCosx*_mImgRowLen + _mImgColCosx*_mImgColLen/2;
	posY[1] = _mImgy + _mImgRowCosy*_mImgRowLen + _mImgColCosy*_mImgColLen/2;
	posZ[1] = _mImgz + _mImgRowCosz*_mImgRowLen + _mImgColCosz*_mImgColLen/2;

	// Buttom center

	posX[2] = posX[0] + _mImgColCosx*_mImgColLen;
	posY[2] = posY[0] + _mImgColCosy*_mImgColLen;
	posZ[2] = posZ[0] + _mImgColCosz*_mImgColLen;
	
	// left hand center

	posX[3] = _mImgx + _mImgColCosx*_mImgColLen/2;
	posY[3] = _mImgy + _mImgColCosy*_mImgColLen/2;
	posZ[3] = _mImgz + _mImgColCosz*_mImgColLen/2;

	// Go through all four corners

	for (i = 0; i < 4; i++) {
		// we want to view the source slice from the "point of view" of
		// the target localizer, i.e. a parallel projection of the source
		// onto the target

		// do this by imaging that the target localizer is a view port
		// into a relocated and rotated co-ordinate space, where the
		// viewport has a row vector of +X, col vector of +Y and normal +Z,
		// then the X and Y values of the projected target correspond to
		// row and col offsets in mm from the TLHC of the localizer image !

		// move everything to origin of target
		posX[i] -= _mScoutx;
		posY[i] -= _mScouty;
		posZ[i] -= _mScoutz;

		rotateImage(posX[i], posY[i], posZ[i],posX[i], posY[i], posZ[i]);
		// at this point the position contains the location on the scout image. calculate the pixel position
		// dicom coordinates are center of pixel 1\1
		colPixel[i] = int(posX[i]/_mScoutySpacing + 0.5);
		rowPixel[i] = int(posY[i]/_mScoutxSpacing + 0.5);
	}
	//  sort out the column and row pixel coordinates into the bounding box axis named coordinates
	// same order as the position top -> right -> bottom -> left
	_mAxisTopx		= colPixel[0];
	_mAxisTopy		= rowPixel[0];
	_mAxisRightx	= colPixel[1];
	_mAxisRighty	= rowPixel[1];
	_mAxisBottomx	= colPixel[2];
	_mAxisBottomy	= rowPixel[2];
	_mAxisLeftx		= colPixel[3];
	_mAxisLefty		= rowPixel[3];
}



CCellWnd::CCellWnd(CDicomViewer *pDicomViewer)
{
	int ni;
	CPoint pt(-1,-1);

	m_lpDicomImage = NULL;

	m_pSeriesObject = NULL;

	m_pDicomObject = NULL;
	
	m_pDicomViewer = pDicomViewer;

	m_bLocalizer = FALSE;
	m_bCTMR      = FALSE;

	m_lpBMIH = NULL;

	m_lpBits = NULL;

	m_rtBorder.left = 0;
	m_rtBorder.top = 0;
	m_rtBorder.right = 100;
	m_rtBorder.bottom = 100;

	m_rtView = m_rtBorder;

	m_rtProcessView.left = m_rtView.Width() / 4;
	m_rtProcessView.top = m_rtView.Height() / 4;
	m_rtProcessView.right = m_rtProcessView.left + m_rtView.Width() / 2;
	m_rtProcessView.bottom = m_rtProcessView.top + m_rtView.Height() / 2;


	m_lpvColorTable = NULL;

	m_lpBMIH = NULL;
	m_lpBits = NULL;

	m_lpFont = NULL;

	m_bDrawOverlayInfo = FALSE;
	m_bDrawMeasure = FALSE;
	m_bDrawCTValue = FALSE;
	
	m_bDrawProcessView = FALSE;

	m_fZoomFactor = 1.0;

	m_nShiftX = 0;
	m_nShiftY = 0;

	m_bHFlip = FALSE;
	m_nRotateAngle = 0;

	m_nSmoothFilterNo = 0;
	m_nSharpFilterNo = 0;

	m_bReversed = FALSE;

	m_bUseExtLut = FALSE;

	for (ni = 0; ni < 256; ni ++)
	{
		m_cExtLut[ni]		= ni;
		m_cExtLut[256 + ni] = ni;
		m_cExtLut[512 + ni] = ni;
	}

	m_lpDicomImage = NULL;

	m_sUnregistered = g_szUnregistered;

	for (ni = 0; ni < (int) strlen(g_szUnregistered); ni ++)
	{
		m_sUnregistered.SetAt(ni,m_sUnregistered.GetAt(ni) ^ 0x17);
	}

	m_nFrameIndex = 0;

	m_bPrintPreviewMode = FALSE;

	m_nMagnifierSize = 256;

	m_ptMeasureCobbPoint1 = pt;
	m_ptMeasureCobbPoint2 = pt;
	m_ptMeasureCobbPoint3 = pt;
	m_ptMeasureCobbPoint4 = pt;

	m_nCineMode = 0;

}

CCellWnd::~CCellWnd()
{
	if (m_lpBMIH != NULL)
	{
		delete m_lpBMIH;
		m_lpBMIH = NULL;
	}

	if (m_lpBits != NULL)
	{
		delete [] m_lpBits;
		m_lpBits = NULL;
	}

	if (m_lpDicomImage != NULL)
	{
		delete m_lpDicomImage;
		m_lpDicomImage = NULL;
	}
}

void CCellWnd::SetShift(int nX,int nY)
{
	m_nShiftX = nX;
	m_nShiftY = nY;

}


void CCellWnd::GetShift(int &nX,int &nY)
{
	nX = m_nShiftX;
	nY = m_nShiftY;
}


void CCellWnd::SetBorder(CRect rect)
{
	m_rtBorder = rect;
	ComputeImageView();
}



CRect CCellWnd::GetBorder()
{
	return m_rtBorder;
}

CRect CCellWnd::GetImageViewRect()
{
	CRect rt;

	rt = m_rtView;
	
	rt.left = rt.left + m_nShiftX;
	rt.top = rt.top + m_nShiftY;
	rt.right = rt.right + m_nShiftX;
	rt.bottom = rt.bottom + m_nShiftY;

	return rt;
}


CRect CCellWnd::GetProcessViewRect()
{
	CRect rt;

	rt = m_rtProcessView;
	
	rt.left = rt.left + m_nShiftX;
	rt.top = rt.top + m_nShiftY;
	rt.right = rt.right + m_nShiftX;
	rt.bottom = rt.bottom + m_nShiftY;

	return rt;
}


void CCellWnd::SetSeriesObject(CSeriesObject *pObj)
{	
	int nCount;
	POSITION pos;
	CString sModality("");

	m_bLocalizer = FALSE;
	m_bCTMR = FALSE;

	m_pSeriesObject = (CSeriesObject *) pObj;

	m_pDicomObject = NULL;

	if (m_pSeriesObject != NULL)
	{
		nCount = m_pSeriesObject->m_DicomObjectList.GetCount();
	
		if (m_pSeriesObject->m_nCurrentIndex < 0) 
			m_pSeriesObject->m_nCurrentIndex = 0;
	
		if (m_pSeriesObject->m_nCurrentIndex >= nCount) 
			m_pSeriesObject->m_nCurrentIndex = nCount - 1;

		if (m_pSeriesObject->m_nCurrentIndex >= 0)
		{
			pos = m_pSeriesObject->m_DicomObjectList.FindIndex(m_pSeriesObject->m_nCurrentIndex);

			if (pos != NULL)
			{
				m_pDicomObject = (CDicomObject *) m_pSeriesObject->m_DicomObjectList.GetAt(pos);
			}
		}
	}


	if (m_lpBMIH != NULL)
	{
		delete m_lpBMIH;
		m_lpBMIH = NULL;
	}

	if (m_lpBits != NULL)
	{
		delete []m_lpBits;
		m_lpBits = NULL;
	}

	if (m_lpDicomImage != NULL)
	{
		delete m_lpDicomImage;
		m_lpDicomImage = NULL;
	}

	MakeBitmap(FALSE);

	ComputeImageView();

	if (m_pDicomObject != NULL)
	{
		m_bLocalizer = m_pDicomObject->IsLocalizer();
		sModality = m_pDicomObject->GetModality();

		m_bCTMR = (sModality == "CT" ||  sModality == "MR");
	}
}



void CCellWnd::SetDicomObject(CSeriesObject *pSeriesObject,CDicomObject *pDicomObject)
{	
	CString sModality;

	m_bCTMR = FALSE;
	m_bLocalizer = FALSE;

	m_nCineMode = 0;

	m_pSeriesObject = pSeriesObject;

	m_pDicomObject = pDicomObject;

	if (m_lpBMIH != NULL)
	{
		delete m_lpBMIH;
		m_lpBMIH = NULL;
	}

	if (m_lpBits != NULL)
	{
		delete []m_lpBits;
		m_lpBits = NULL;
	}

	if (m_lpDicomImage != NULL)
	{
		delete m_lpDicomImage;
		m_lpDicomImage = NULL;
	}

	MakeBitmap(FALSE);

	ComputeImageView();

	if (m_pDicomObject != NULL)
	{
		m_bLocalizer = m_pDicomObject->IsLocalizer();

		sModality = m_pDicomObject->GetModality();

		m_bCTMR = (sModality == "CT" || sModality == "MR");
	}
}


CDicomObject * CCellWnd::GetDicomObject()
{
	return m_pDicomObject;
}


void CCellWnd::SetFlip(BOOL bHFlip,BOOL bVFlip)
{ 
	m_bHFlip = bHFlip;
	m_bVFlip = bVFlip;

	if (m_lpBMIH != NULL)
	{
		delete m_lpBMIH;
		m_lpBMIH = NULL;
	}

	if (m_lpBits != NULL)
	{
		delete []m_lpBits;
		m_lpBits = NULL;
	}

	if (m_lpDicomImage != NULL)
	{
		delete m_lpDicomImage;
		m_lpDicomImage = NULL;
	}


}


void CCellWnd::GetFlip(BOOL &bHFlip,BOOL &bVFlip)
{
	bHFlip = m_bHFlip;
	bVFlip = m_bVFlip;
}


void CCellWnd::SetExtLut(BOOL bUseExtLut,BYTE *lpLut)
{
	m_bUseExtLut = bUseExtLut;

	if (m_bUseExtLut)
	{
		memcpy(m_cExtLut,lpLut,768);
	}

	if (m_lpBMIH != NULL)
	{
		delete m_lpBMIH;
		m_lpBMIH = NULL;
	}
	
}

BOOL CCellWnd::DicomObjectIsExisting()
{
	return (m_pDicomObject != NULL);
}

 

void CCellWnd::MakeBitmap(BOOL bFast)
{
	OFCondition status = EC_Normal;
	int ni,nBytesPerPixel,nScanLineBytes,nImageWidth,nImageHeight,nBitsBufferSize;
	int nWinScaleShl16,nTemp,nFrameIndex;
	int nBitsStored,nWid;
	unsigned long nSize=0;
	BOOL bPhotoMetric1;
	LPRGBQUAD pDibQuad;
	const UINT8 *lpBits=NULL;
	UINT8 *lpBitsData;
	UINT8 *lpLine1,*lpLine2,*lpOrginBits=NULL;
	UINT8 v1,v2,v3,v,*p1,*p2;
	double fcen,fwid,fmin,fmax;
	CString sPhotometric;
	unsigned long nParam1,nParam2,nParam3;      
	UINT8 pLutData8[255];
	const UINT8 *pLut8 = NULL;
	UINT16 pLutData16[255],nValue;
	const UINT16 *pLut16 = NULL;
	int nX,nY,nZ;
	float gauss[9] = {1.0f, 2.0f, 1.0f, 2.0f, 4.0f, 2.0f, 1.0f, 2.0f, 1.0f};
	float laplacian[9] = {-1.0f, -1.0f, -1.0f, -1.0f, 9.0f, -1.0f, -1.0f, -1.0f, -1.0f};
	float coef,coefarray[9],tmpnum;
	BYTE *pExtLut;


	if (m_pDicomObject == NULL) return;

	nFrameIndex = m_pDicomObject->GetFrameIndex();

	pLut8 = (unsigned char * )&pLutData8;
	pLut16 = (unsigned short *) &pLutData16;
	      
	sPhotometric = m_pDicomObject->GetPhotometricInterpretation();
	
	bPhotoMetric1 = (_tcsicmp(sPhotometric,_T("MONOCHROME1")) == 0);

	nImageWidth = m_pDicomObject->GetImageWidth();


	nImageHeight = m_pDicomObject->GetImageHeight();

	nBitsStored = m_pDicomObject->GetBitsStored();

	nBytesPerPixel = m_pDicomObject->GetBytesPerPixel();
	nScanLineBytes = m_pDicomObject->GetScanLineBytes(); 
	nBitsBufferSize = m_pDicomObject->GetImageSize();


	if (m_lpBits == NULL)
	{
		m_lpBits = new char[nBitsBufferSize];
	}

	if (m_lpBMIH == NULL)
	{
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof (BITMAPINFOHEADER) + 
			sizeof (RGBQUAD) * (m_pDicomObject->GetColorTableEntries() + 4)];
	

		// Setup BitmapInfoHeader
		m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
		m_lpBMIH->biWidth = nImageWidth;
		m_lpBMIH->biHeight = nImageHeight;
		m_lpBMIH->biPlanes = 1;
		m_lpBMIH->biBitCount = m_pDicomObject->GetColorDepth();
		m_lpBMIH->biSizeImage = nBitsBufferSize;
		m_lpBMIH->biXPelsPerMeter = 0;
		m_lpBMIH->biYPelsPerMeter = 0;
		m_lpBMIH->biClrUsed = m_pDicomObject->GetColorTableEntries();
		m_lpBMIH->biClrImportant = m_pDicomObject->GetColorTableEntries();
		m_lpBMIH->biCompression = BI_RGB;
	
		m_nImageWidth = m_lpBMIH->biWidth;
		m_nImageHeight = m_lpBMIH->biHeight;

		m_lpvColorTable = (LPBYTE) m_lpBMIH + sizeof(BITMAPINFOHEADER);
    
		pDibQuad = (LPRGBQUAD) m_lpvColorTable;
		memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * (m_pDicomObject->GetColorTableEntries() + 4));
	
		for (ni = 0; ni <  m_pDicomObject->GetColorTableEntries(); ni++)
		{
			pDibQuad->rgbRed		= ni;
			pDibQuad->rgbGreen      = ni;
			pDibQuad->rgbBlue       = ni;
			pDibQuad->rgbReserved   = 0;

			pDibQuad++;
		}

	}


	if (sPhotometric == "MONOCHROME1" || sPhotometric == "MONOCHROME2" || sPhotometric == "RGB" || sPhotometric == "YBRFULL" || sPhotometric == "YBRFULL422")
	{

		if (m_lpDicomImage == NULL)
		{
			m_lpDicomImage = new DicomImage(m_pDicomObject->GetDicomDataset(),m_pDicomObject->GetDicomDataset()->getOriginalXfer());//CIF_KeepYCbCrColorModel);

			m_lpDicomImage->flipImage(m_bHFlip ? 1 : 0, m_bVFlip ? 1 : 0);

		}
		
		m_pDicomObject->GetWinLevel(fcen,fwid);

		nWid = 1 << nBitsStored;

		if (fwid == 0)
		{

			if (m_lpDicomImage->getWindowCount() > 0)
			{
				m_lpDicomImage->setWindow(0);
				m_lpDicomImage->getWindow(fcen,fwid);
			}

			if (fwid == 0)
			{
				fmin = 0;
				fmax = 0;

				m_lpDicomImage->getMinMaxValues(fmin,fmax,0);
				
				if (fmax > fmin)
				{
					fwid = fmax - fmin;
					fcen = fmin + (((int) fwid ) >> 1) ;
				}

				if (fwid == 0)
				{
					//nWid = 1 << nBitsStored;
				
					fwid = nWid - 1;
					fcen = nWid >> 1;
				}
			}

		}
			
		if (fwid > nWid)
		{
			fmin = 0;
			fmax = 0;

			m_lpDicomImage->getMinMaxValues(fmin,fmax,0);
				
			if (fmax > fmin)
			{
				fwid = fmax - fmin;
				fcen = fmin + (((int) fwid ) >> 1) ;
			}

			if (fwid == 0)
			{
				nWid = 1 << nBitsStored;
			
				fwid = nWid - 1;
				fcen = nWid >> 1;
			}

		}
		
		m_pDicomObject->SetWinLevel(fcen,fwid);

		m_lpDicomImage->setWindow(fcen,fwid);

		m_lpDicomImage->createWindowsDIB(m_lpBits,m_pDicomObject->GetImageSize(),nFrameIndex,m_pDicomObject->GetColorDepth(),1,1);
		
		
	}
	else if (sPhotometric == "PALETTECOLOR")
	{
		m_pDicomObject->GetWinLevel(fcen,fwid);
	
		if (fwid == 0)
		{
			fwid = 256;
			fcen = 128;
		}
	
		m_pDicomObject->SetWinLevel(fcen,fwid);


	    nWinScaleShl16 = (int) (1 << 16);
        nWinScaleShl16 = (int) (nWinScaleShl16 * (256.0 / fwid));

		m_pDicomObject->GetDicomDataset()->findAndGetUint8Array(DCM_PixelData,lpBits,&nSize);

		if (nSize == nBitsBufferSize)
		{
			lpLine1 = (UINT8 *) m_lpBits;
			lpLine2 = (UINT8 *) lpBits + (m_nImageHeight - 1) * nScanLineBytes;

			for (ni = 0; ni < m_nImageHeight; ni ++)
			{
				memcpy(lpLine1,lpLine2,nScanLineBytes);
				lpLine1 += nScanLineBytes;
				lpLine2 -= nScanLineBytes;
			}
		}
		
		nParam1 = m_pDicomObject->GetUint16(DCM_RedPaletteColorLookupTableDescriptor,0);
		nParam2 = m_pDicomObject->GetUint16(DCM_RedPaletteColorLookupTableDescriptor,1);
		nParam3 = m_pDicomObject->GetUint16(DCM_RedPaletteColorLookupTableDescriptor,2);

		if (nParam3 == 16)
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint16Array(DCM_RedPaletteColorLookupTableData,pLut16,&nParam1);
		}
		else
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint8Array(DCM_RedPaletteColorLookupTableData,pLut8,&nParam1);
		}

	    pDibQuad = (LPRGBQUAD) m_lpvColorTable;

		for (ni = nParam2; ni <= (int) nParam1 && ni < m_pDicomObject->GetColorTableEntries(); ni++)
		{
			if (nParam3 == 16)
			{      
				nValue = *pLut16 >> 8;
				nTemp = nValue;
				nTemp = (int) (nTemp - fcen);
				nTemp = nTemp * nWinScaleShl16;
				nTemp = nTemp / 65536;
				nTemp = nTemp + 128;
				
				if (nTemp < 0) 
					nTemp = 0;
				else if (nTemp > 255) 
					nTemp = 255;

				nValue = nTemp;
			}
			else
			{
				nValue = *pLut8;
			}

			pDibQuad->rgbRed = (BYTE) nValue;
			pDibQuad->rgbGreen = (BYTE) nValue;
			pDibQuad->rgbBlue = (BYTE) nValue;

			pDibQuad++;

			pLut16 ++;
		}


		nParam1 = m_pDicomObject->GetUint16(DCM_GreenPaletteColorLookupTableDescriptor,0);
		nParam2 = m_pDicomObject->GetUint16(DCM_GreenPaletteColorLookupTableDescriptor,1);
		nParam3 = m_pDicomObject->GetUint16(DCM_GreenPaletteColorLookupTableDescriptor,2);

		if (nParam3 == 16)
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint16Array(DCM_GreenPaletteColorLookupTableData,pLut16,&nParam1);
		}
		else
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint8Array(DCM_GreenPaletteColorLookupTableData,pLut8,&nParam1);
		}

	    pDibQuad = (LPRGBQUAD) m_lpvColorTable;

		for (ni = nParam2; ni <= (int) nParam1 && ni < m_pDicomObject->GetColorTableEntries(); ni++)
		{
			if (nParam3 == 16)
			{
				nValue = *pLut16 >> 8;
				nTemp = nValue;
				nTemp = (int) (nTemp - fcen);
				nTemp = nTemp * nWinScaleShl16;
				nTemp = nTemp / 65536;
				nTemp = nTemp + 128;
				
				if (nTemp < 0) 
					nTemp = 0;
				else if (nTemp > 255) 
					nTemp = 255;

				nValue = nTemp;
			}
			else
			{
				nValue = *pLut8;
			}
			pDibQuad->rgbGreen = (BYTE) nValue;
			pDibQuad++;
			pLut16 ++;
		}


		nParam1 = m_pDicomObject->GetUint16(DCM_BluePaletteColorLookupTableDescriptor,0);
		nParam2 = m_pDicomObject->GetUint16(DCM_BluePaletteColorLookupTableDescriptor,1);
		nParam3 = m_pDicomObject->GetUint16(DCM_BluePaletteColorLookupTableDescriptor,2);

		if (nParam3 == 16)
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint16Array(DCM_BluePaletteColorLookupTableData,pLut16,&nParam1);
		}
		else
		{
			m_pDicomObject->GetDicomDataset()->findAndGetUint8Array(DCM_BluePaletteColorLookupTableData,pLut8,&nParam1);
		}

	    pDibQuad = (LPRGBQUAD) m_lpvColorTable;

		for (ni = nParam2; ni <= (int) nParam1 && ni < m_pDicomObject->GetColorTableEntries(); ni++)
		{
			if (nParam3 == 16)
			{
				nValue = *pLut16 >> 8 ;
				nTemp = nValue;
				nTemp = (int) (nTemp - fcen);
				nTemp = nTemp * nWinScaleShl16;
				nTemp = nTemp / 65536;
				nTemp = nTemp + 128;
				
				if (nTemp < 0) 
					nTemp = 0;
				else if (nTemp > 255) 
					nTemp = 255;

				nValue = nTemp;
			}
			else
			{
				nValue = *pLut8;
			}
			pDibQuad->rgbBlue = (BYTE) nValue;
			pDibQuad++;
			pLut16 ++;
		}
	}

	/////////////////////////////////////////////////////////////////
	// ÕºœÛŒ±≤ 
	/////////////////////////////////////////////////////////////////

	pDibQuad = (LPRGBQUAD) m_lpvColorTable;

	//if (nFakeColorSetNo >= 1 && nFakeColorSetNo <= 13)
	if (m_bUseExtLut)
	{
		//pExtLut = m_pDicomViewer->GetExtLut(m_nFakeColorSetNo);

		for (ni = 0; ni < m_pDicomObject->GetColorTableEntries(); ni ++)
		{
			//pDibQuad->rgbRed = pExtLut[ni];
			//pDibQuad->rgbGreen = pExtLut[256 + ni];
			//pDibQuad->rgbBlue = pExtLut[512 + ni];
 
			pDibQuad->rgbRed = m_cExtLut[ni];
			pDibQuad->rgbGreen = m_cExtLut[256 + ni];
			pDibQuad->rgbBlue = m_cExtLut[512 + ni];

			pDibQuad ++;
		}
	}
	else
	{
		for (ni = 0; ni < m_pDicomObject->GetColorTableEntries(); ni ++)
		{

			pDibQuad->rgbRed = ni;
			pDibQuad->rgbGreen = ni;
			pDibQuad->rgbBlue = ni;

			pDibQuad ++;
		}

	}
	

	/////////////////////////////////////////////////////////////////
	// µﬂµπ∫⁄∞◊
	/////////////////////////////////////////////////////////////////

	if (m_bReversed)
	{
		lpBitsData = (BYTE *) m_lpBits + nBitsBufferSize - 1;

		while (lpBitsData >= m_lpBits)
		{
			*lpBitsData = 255 - *lpBitsData;
			lpBitsData --;
		}
	}


	/////////////////////////////////////////////////////////////////
	// ÕºœÛ∆Ωª¨
	/////////////////////////////////////////////////////////////////

	
	if ((m_nSmoothFilterNo == 1 || m_nSmoothFilterNo == 2) && !bFast)
	{
		lpOrginBits = new unsigned char[nBitsBufferSize];
		
		if (lpOrginBits != NULL)
		{
			memcpy(lpOrginBits,m_lpBits,nBitsBufferSize);

			lpLine1 = (unsigned char *) m_lpBits + nBitsBufferSize - nScanLineBytes;
			lpLine2 = (unsigned char *) lpOrginBits + nBitsBufferSize - nScanLineBytes;

			for (nY = 1; nY < nImageHeight - 1; nY ++)
			{
				lpLine1 = lpLine1 - nScanLineBytes;
				lpLine2 = lpLine2 - nScanLineBytes;
				
				for (nX = 1; nX < nImageWidth - 1; nX ++)
				{
					p1 = lpLine1 + nX * nBytesPerPixel;
					p2 = lpLine2 + nX * nBytesPerPixel;

					for (nZ = 0; nZ < nBytesPerPixel; nZ ++)
					{
						p1 = p1 + nZ;
						p2 = p2 + nZ;

						v2 = *p2;

						if (m_nSmoothFilterNo == 1)
						{
							v1 = *(p2 - 1);
							v3 = *(p2 + 1);
						}
						else
						{
							v1 = *(p2 + nScanLineBytes);
							v3 = *(p2 - nScanLineBytes);
						}

						if (v1 > v2)
						{
							if (v2 > v3) 
								v = v2;
							else
							{
								if (v1 > v3) 
									v = v3;
								else
									v = v1;
							}
						}
						else
						{
							if (v1 > v3)
								v = v1;
							else
							{
								if (v2 > v3)
									v = v3;
								else
									v = v2;
							}
						}

						*p1 = v;
					}
				}
			}

			delete 	[]lpOrginBits;

			lpOrginBits = NULL;
		}
	}
	else if (m_nSmoothFilterNo == 3 && !bFast)
	{
		/*
		memcpy(coefarray,gauss,9 * sizeof(float));

		coef = (float)(1.0/16.0);
	
		lpOrginBits = new unsigned char[nBitsBufferSize];
		
		if (lpOrginBits != NULL)
		{
			memcpy(lpOrginBits,m_lpBits,nBitsBufferSize);

			lpLine1 = (unsigned char *) m_lpBits + nBitsBufferSize - nScanLineBytes;
			lpLine2 = (unsigned char *) lpOrginBits + nBitsBufferSize - nScanLineBytes;


			for (nY = 1; nY < nImageHeight - 1; nY ++)
			{
				lpLine1 = lpLine1 - nScanLineBytes;
				lpLine2 = lpLine2 - nScanLineBytes;
				
				for (nX = 1; nX < nImageWidth - 1; nX ++)
				{
					p1 = lpLine1 + nX * nBytesPerPixel;
					p2 = lpLine2 + nX * nBytesPerPixel;

					for (nZ = 0; nZ < nBytesPerPixel; nZ ++)
					{
						p1 = p1 + nZ;
						p2 = p2 + nZ;
						
						tmpnum  = (float) (*(p2 + nScanLineBytes -1)) * coefarray[0];
						tmpnum += (float) (*(p2 + nScanLineBytes)) * coefarray[1];
						tmpnum += (float) (*(p2 + nScanLineBytes + 1)) * coefarray[2];
						tmpnum += (float) (*(p2 - 1)) * coefarray[3];
						tmpnum += (float) (*(p2)) * coefarray[4];
						tmpnum += (float) (*(p2 + 1)) * coefarray[5];
						tmpnum += (float) (*(p2 - nScanLineBytes - 1)) * coefarray[6];
						tmpnum += (float) (*(p2 - nScanLineBytes)) * coefarray[7];
						tmpnum += (float) (*(p2 - nScanLineBytes + 1)) * coefarray[8];
						
						tmpnum *= coef;
						
						if (tmpnum > 255.0)
							tmpnum = 255;
						else if (tmpnum < 0.0)
							tmpnum = -tmpnum;
						
						*p1 = (BYTE) tmpnum;
					}
				}
			}


			delete []lpOrginBits;

			lpOrginBits = NULL;
			
		}
		*/
	}

  

	/////////////////////////////////////////////////////////////////
	// ÕºœÛ»ÒªØ
	/////////////////////////////////////////////////////////////////
	if (m_nSharpFilterNo == 1 && !bFast)
	{
		memcpy(coefarray,laplacian,9 * sizeof(float));

		coef = 1.0;
	
		lpOrginBits = new unsigned char[nBitsBufferSize];
		
		if (lpOrginBits != NULL)
		{
			memcpy(lpOrginBits,m_lpBits,nBitsBufferSize);

			lpLine1 = (unsigned char *) m_lpBits + nBitsBufferSize - nScanLineBytes;
			lpLine2 = (unsigned char *) lpOrginBits + nBitsBufferSize - nScanLineBytes;


			for (nY = 1; nY < nImageHeight - 1; nY ++)
			{
				lpLine1 = lpLine1 - nScanLineBytes;
				lpLine2 = lpLine2 - nScanLineBytes;
				
				for (nX = 1; nX < nImageWidth - 1; nX ++)
				{
					p1 = lpLine1 + nX * nBytesPerPixel;
					p2 = lpLine2 + nX * nBytesPerPixel;

					for (nZ = 0; nZ < nBytesPerPixel; nZ ++)
					{
						p1 = p1 + nZ;
						p2 = p2 + nZ;
						
						tmpnum  = (float) (*(p2 + nScanLineBytes -1)) * coefarray[0];
						tmpnum += (float) (*(p2 + nScanLineBytes)) * coefarray[1];
						tmpnum += (float) (*(p2 + nScanLineBytes + 1)) * coefarray[2];
						tmpnum += (float) (*(p2 - 1)) * coefarray[3];
						tmpnum += (float) (*(p2)) * coefarray[4];
						tmpnum += (float) (*(p2 + 1)) * coefarray[5];
						tmpnum += (float) (*(p2 - nScanLineBytes - 1)) * coefarray[6];
						tmpnum += (float) (*(p2 - nScanLineBytes)) * coefarray[7];
						tmpnum += (float) (*(p2 - nScanLineBytes + 1)) * coefarray[8];
						
						tmpnum *= coef;
						
						if (tmpnum > 255.0)
							tmpnum = 255;
						else if (tmpnum < 0.0)
							tmpnum = -tmpnum;
						
						*p1 = (BYTE) tmpnum;
					}
				}
			}


			delete []lpOrginBits;

			lpOrginBits = NULL;
		}
	}

}


void CCellWnd::DrawProcessView(CDC *pDC)
{
	pDC->DrawFocusRect(&m_rtProcessView);
}


void CCellWnd::DrawOverlayInfo(CRect rt,CDC *pDC)
{
	CString sModality;
	CRect rtText;
	CString sText,sTmp,sAge("");
	int nTextHeight1,nTextHeight2,nPos;
	int nFrameIndex,nFrameCount;
	CFont *lpFont = NULL;
	double fwid,fcen;
	COLORREF oldColor;
	BOOL bFromRIS;

	if (m_pDicomObject == NULL) return;

	if (m_lpFont != NULL)
	{
		lpFont = pDC->SelectObject(m_lpFont);
	}

	sModality = m_pDicomObject->GetModality();

	sAge = m_pDicomObject->GetPatientAge();

	bFromRIS = m_pDicomObject->GetFromRIS();

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	
	/////////////////////////////////////////////////////////////////
	// Draw Orientation 
	/////////////////////////////////////////////////////////////////

	pDC->DrawText(_T("H"),&rtText,DT_CALCRECT);

	nTextHeight1 = rtText.Height();
	nTextHeight2 = nTextHeight1 + 4;

	rtText.left = rt.left;
	rtText.right = rt.right;
	rtText.top = rt.top + (rt.Height() - nTextHeight1) / 2;
	rtText.bottom = rtText.top + nTextHeight1;

	sText = m_pDicomObject->GetOrientationText(0,m_bHFlip,m_bVFlip,m_nRotateAngle);
	pDC->DrawText(sText,rtText,DT_RIGHT);
	
	sText = m_pDicomObject->GetOrientationText(2,m_bHFlip,m_bVFlip,m_nRotateAngle);
	pDC->DrawText(sText,rtText,DT_LEFT);

	rtText.top = rt.top;
	rtText.bottom = rtText.top + nTextHeight1;
	rtText.left = rt.left;
	rtText.right = rt.right;

	sText = m_pDicomObject->GetOrientationText(3,m_bHFlip,m_bVFlip,m_nRotateAngle);
	pDC->DrawText(sText,rtText,DT_CENTER);

	rtText.bottom = rt.bottom;
	rtText.top = rt.bottom - nTextHeight1;
	rtText.left = rt.left;
	rtText.right = rt.right;

	sText = m_pDicomObject->GetOrientationText(1,m_bHFlip,m_bVFlip,m_nRotateAngle);
	pDC->DrawText(sText,rtText,DT_CENTER);
	
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// Draw Basic Information
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////
	//Right_Top Corner
	/////////////////////////////////////////////

	rtText.top = rt.top;
	rtText.bottom = rt.top +  nTextHeight2;
	rtText.left = rt.left;
	rtText.right = rt.right;

	sText = m_pDicomObject->GetString(DCM_InstitutionName);

	sText = ProcessTextA(sText);

	pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);


	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;	
	
	sText = m_pDicomObject->GetString(DCM_PatientName);
	sText = ProcessTextA(sText);
	sText = ProcessTextB(sText);

	nPos = sText.Find(_T("^"));
	if (nPos > 0)
	{
		sText = sText.Left(nPos);
	}

	pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);

	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;	

	sText.Format(_T("%s %4s %s"),m_pDicomObject->GetString(DCM_PatientSex),sAge,m_pDicomObject->GetString(DCM_PatientID));
	pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);

	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;	
	sText.Format(_T("Acc: %s"),m_pDicomObject->GetString(DCM_AccessionNumber));	
	pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);

	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;	
	
	sTmp = m_pDicomObject->GetString(DCM_StudyDate);

	if (sTmp.GetLength() == 8)
	{
		sTmp.Insert(4,_T("-"));
		sTmp.Insert(7,_T("-"));
	}

	pDC->DrawText(sTmp,rtText,DT_RIGHT | DT_VCENTER);

	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;	
	
	sTmp = m_pDicomObject->GetString(DCM_AcquisitionTime);
	
	nPos = sTmp.ReverseFind('.');
	
	if (nPos == 6)
	{
		sTmp.Delete(nPos,sTmp.GetLength() - nPos );
		sTmp.Insert(2,_T(":"));
		sTmp.Insert(5,_T(":"));
	}
	else if (sTmp.GetLength() == 6)
	{
		sTmp.Insert(2,_T(":"));
		sTmp.Insert(5,_T(":"));
	}


	//if (!m_bSeriesMode)
	{
		sText.Format(_T("Acq Tm: %s"),sTmp);
		pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);
	}

	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;
	
	rtText.bottom = rtText.bottom + nTextHeight2;
	rtText.top = rtText.top + nTextHeight2;
	sText.Format(_T("%d x %d"),m_pDicomObject->GetImageWidth(),m_pDicomObject->GetImageHeight());


	pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);
	
	
	/////////////////////////////////////////////
	// For MultiFrame
	nFrameCount = m_pDicomObject->GetFrameCount();

	if (nFrameCount > 1 && !m_bSeriesMode)
	{
		nFrameIndex = m_pDicomObject->GetFrameIndex() + 1;

		sText.Format(_T("%d / %d"),nFrameIndex,nFrameCount);
	
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;

		pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);
	}

	
	/////////////////////////////////////////////
	//Left_Down Corner
	/////////////////////////////////////////////

	///if (!m_pDicomObject->IsFromRIS())
	{

		rtText.bottom = rt.bottom;
		rtText.top = rt.bottom - nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

	
		m_pDicomObject->GetWinLevel(fcen,fwid);

		sText.Format(_T("W: %.0f   L: %.0f"),fwid,fcen);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
	}

	///////////////////////////////////////////////////////////

	// Demo
	if (!m_bRegistered)
	{
		pDC->MoveTo(rt.left,rt.top);
		pDC->LineTo(rt.right,rt.bottom);
		pDC->MoveTo(rt.right,rt.top);
		pDC->LineTo(rt.left,rt.bottom);

		/////////////////////////////////////////////
		//Right_Down Corner
		/////////////////////////////////////////////

		rtText.bottom = rt.bottom;
		rtText.top = rt.bottom - nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		oldColor = pDC->GetTextColor();

		pDC->SetTextColor(RGB(255,0,0));

		sText.Format(_T("%s"),m_sUnregistered);
		pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);
		
		pDC->SetTextColor(oldColor);

		///////////////////////////////////////////////////////////
	}


	/////////////////////////////////////////////////////////////////
	// Draw Basic Information
	/////////////////////////////////////////////////////////////////

	if (sModality == _T("CT") )// && !m_bSeriesMode)
	{
		rtText.top = rt.top ;
		rtText.bottom = rtText.top + nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText = m_pDicomObject->GetString(DCM_ManufacturerModelName);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Ex: %s"),m_pDicomObject->GetString(DCM_StudyID));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText = m_pDicomObject->GetString(DCM_SeriesDescription);

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Se: %s"),m_pDicomObject->GetString(DCM_SeriesNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
	
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Im: %s"),m_pDicomObject->GetString(DCM_InstanceNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;

		sTmp = m_pDicomObject->GetString(DCM_Laterality);
		if (sTmp.IsEmpty())
			sTmp = _T("-");
		sText.Format(_T("Lat:%s"),  sTmp);   
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;
		sText = m_pDicomObject->GetString(DCM_ContrastBolusAgent);
			    
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rt.bottom  + nTextHeight2 * 8;
		rtText.top = rt.top + nTextHeight2 * 7;	
		sText.Format(_T("Mag: %5.2fx"),GetRealZoomFactor());
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);



		/////////////////////////////////////////////////////////////
		rtText.bottom = rt.bottom - nTextHeight2;
		rtText.top = rtText.bottom - nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText.Format(_T("Lin:DCM / Lin:DCM / Id:ID"));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
	
		rtText.top = rtText.top - nTextHeight2;
		rtText.bottom = rtText.bottom - nTextHeight2;
		sText.Format(_T("%.1f S"),m_pDicomObject->GetSint32(DCM_ExposureTime) / 1000.0);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.top = rtText.top - nTextHeight2;
		rtText.bottom = rtText.bottom - nTextHeight2;
		sText.Format(_T("Tile: %.1f"),m_pDicomObject->GetFloat64(DCM_GantryDetectorTilt));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.top = rtText.top - nTextHeight2;
		rtText.bottom = rtText.bottom - nTextHeight2;
		//

		rtText.top = rtText.top - nTextHeight2;
		rtText.bottom = rtText.bottom - nTextHeight2;
		sText.Format(_T("%.1d mA"), m_pDicomObject->GetSint32(DCM_XRayTubeCurrent));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.top = rtText.top - nTextHeight2;
		rtText.bottom = rtText.bottom - nTextHeight2;
		sText.Format(_T("%.1f kV"), m_pDicomObject->GetFloat64(DCM_KVP));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

	}
	else if (sModality == _T("CR"))
	{
		rtText.top = rt.top ;
		rtText.bottom = rtText.top + nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText = m_pDicomObject->GetString(DCM_ManufacturerModelName);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Ex: %s"),m_pDicomObject->GetString(DCM_StudyID));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText = m_pDicomObject->GetString(DCM_SeriesDescription);


		sText = ::ProcessTextA(sText);
		sText = ::ProcessTextB(sText);


		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Se: %s"),m_pDicomObject->GetString(DCM_SeriesNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
	
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Im: %s"),m_pDicomObject->GetString(DCM_InstanceNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("%s"),m_pDicomObject->GetString(DCM_BodyPartExamined));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rt.bottom  + nTextHeight2 * 7;
		rtText.top = rt.top + nTextHeight2 * 7;	
		sText.Format(_T("Mag: %5.2fx"),GetRealZoomFactor());
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


	}
	else if (sModality == _T("MR"))
	{

		rtText.top = rt.top ;
		rtText.bottom = rtText.top + nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText = m_pDicomObject->GetString(DCM_ManufacturerModelName);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Ex: %s"),m_pDicomObject->GetString(DCM_StudyID));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText = m_pDicomObject->GetString(DCM_SeriesDescription);


		sText = ::ProcessTextA(sText);
		sText = ::ProcessTextB(sText);

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Se: %s"), m_pDicomObject->GetString(DCM_SeriesNumber));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		// ≤øŒªœ‘ æ”–µƒπ˝≥§, “™«Û≤ªœ‘ æ
		//rtText.bottom = rtText.bottom + nTextHeight2;
		//rtText.top = rtText.top + nTextHeight2;	
		//sText.Format(_T("%s"),  m_pDicomObject->GetString(DCM_BodyPartExamined));
			    
		//pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;

		sTmp = m_pDicomObject->GetString(DCM_Laterality);
		if (sTmp.CompareNoCase(_T("L")) == 0)
			sTmp = _T("◊Û");
		else if (sTmp.CompareNoCase(_T("R")) == 0)
			sTmp = _T("”“");
		else if (sTmp.IsEmpty())
			sTmp = _T("-");

		sText.Format(_T("Lat:%s"),  sTmp);
			    
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		// DCM_Laterality                           DcmTagKey(0x0020, 0x0060)
		// DCM_ImageLaterality                      DcmTagKey(0x0020, 0x0062)


		// DCM_ContrastBolusAgent					DcmTagKey(0x0018, 0x0010)
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;
		sText = m_pDicomObject->GetString(DCM_ContrastBolusAgent);
			    
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Im: %s"), m_pDicomObject->GetString(DCM_InstanceNumber));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rt.bottom  + nTextHeight2 * 8;
		rtText.top = rt.top + nTextHeight2 * 7;	
		sText.Format(_T("Mag: %5.2fx"),GetRealZoomFactor());
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);



		rtText.top = rt.bottom - nTextHeight2 * 5;
		rtText.bottom = rt.bottom;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText.Format(_T("Ss:%s"), m_pDicomObject->GetString(DCM_ScanningSequence));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.top = rt.bottom - nTextHeight2 * 4;
		rtText.bottom = rt.bottom;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText.Format(_T("Sv:%s"), m_pDicomObject->GetString(DCM_SequenceVariant));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);



		rtText.top = rt.bottom - nTextHeight2 * 3;
		rtText.bottom = rt.bottom;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText.Format(_T("Sn:%s"), m_pDicomObject->GetString(DCM_SequenceName));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.top = rt.bottom - nTextHeight2 * 2;
		rtText.bottom = rt.bottom;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText.Format(_T("St: %smm"), m_pDicomObject->GetString(DCM_SliceThickness));

		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

	}
	else if (sModality == _T("US"))
	{


	}
	else if (sModality == _T("XA"))
	{


	}
	else if (sModality ==  _T("ES"))
	{


	}
	else if (sModality == _T("DX"))
	{


	}
	else if (sModality == _T("MG"))
	{


	}
	else if (sModality == _T("NM"))
	{


	}
	else if (sModality == _T("OT"))
	{

		rtText.top = rt.top ;
		rtText.bottom = rtText.top + nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;

		sText = m_pDicomObject->GetString(DCM_ManufacturerModelName);
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Ex: %s"),m_pDicomObject->GetString(DCM_StudyID));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText = m_pDicomObject->GetString(DCM_SeriesDescription);


		sText = ::ProcessTextA(sText);
		sText = ::ProcessTextB(sText);


		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
		
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Se: %s"),m_pDicomObject->GetString(DCM_SeriesNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);
	
		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("Im: %s"),m_pDicomObject->GetString(DCM_InstanceNumber));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

		rtText.bottom = rtText.bottom + nTextHeight2;
		rtText.top = rtText.top + nTextHeight2;	
		sText.Format(_T("%s"),m_pDicomObject->GetString(DCM_BodyPartExamined));
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);


		rtText.bottom = rt.bottom  + nTextHeight2 * 7;
		rtText.top = rt.top + nTextHeight2 * 7;	
		sText.Format(_T("Mag: %5.2fx"),GetRealZoomFactor());
		pDC->DrawText(sText,rtText,DT_LEFT | DT_VCENTER);

	}
	else if (sModality == _T("PT"))
	{


	}
	else if (sModality == _T("RF"))
	{


	}


	if (m_bSeriesMode)
	{
		rtText.bottom = rt.bottom;
		rtText.top = rt.bottom - nTextHeight2;
		rtText.left = rt.left;
		rtText.right = rt.right;
		
		sText.Format(_T("%d / %d"),m_pSeriesObject->GetCurrentIndex() + 1,m_pSeriesObject->GetDicomObjectCount());
		pDC->DrawText(sText,rtText,DT_RIGHT | DT_VCENTER);
	}

	/////////////////////////////////////////////////////////////////

	if (lpFont != NULL)
	{
		pDC->SelectObject(lpFont);
	}
}


double CCellWnd::GetRealZoomFactor()
{
	double f1,f2,fZoomFactor,fPixelAspectRatio;
	int nImageWidth,nImageHeight,nImageWidth0,nImageHeight0;
	

	if (m_pDicomObject != NULL)
	{
		fPixelAspectRatio = m_pDicomObject->GetPixelAspectRatio();
	}
	else
	{
		fPixelAspectRatio = 1.0;
	}
	

	nImageWidth0 = m_nImageWidth;
	nImageHeight0 = m_nImageHeight;

	if (nImageWidth0 > nImageHeight0)
	{
		nImageWidth0 = (int) (nImageWidth0 * fPixelAspectRatio);
	}
	else
	{
		nImageHeight0 = (int) (nImageHeight0 * fPixelAspectRatio);
	}



	if ((m_nRotateAngle % 180) != 0)
	{
		//nImageWidth = m_nImageHeight;
		//nImageHeight = m_nImageWidth;
		nImageWidth = nImageHeight0;
		nImageHeight = nImageWidth0;

	}
	else
	{
		//nImageWidth = m_nImageWidth;
		//nImageHeight = m_nImageHeight;
		nImageWidth = nImageWidth0;
		nImageHeight = nImageHeight0;
	}

	if (m_fZoomFactor == 0)
	{
		if (nImageWidth != 0)
		{
			f1 = ((double) m_rtBorder.Width() / nImageWidth);
		}
		else
		{
			f1 = 1;
		}
		
		if (nImageHeight != 0)
		{
			f2 = ((double) m_rtBorder.Height() / nImageHeight);
		}
		else
		{
			f2 = 1;
		}

		fZoomFactor = f1;

		if (fZoomFactor > f2) 
			fZoomFactor = f2;

	}
	else
	{
		fZoomFactor = m_fZoomFactor;
	}
	
	return fZoomFactor;

}

void CCellWnd::ComputeImageView()
{
	int nImageWidth0,nImageWidth,nImageHeight0,nImageHeight,nStepX,nStepY; 
	double fZoomFactor,fPixelAspectRatio;

	fZoomFactor = GetRealZoomFactor();

	if (m_pDicomObject != NULL)
	{
		fPixelAspectRatio = m_pDicomObject->GetPixelAspectRatio();
	}
	else
	{
		fPixelAspectRatio = 1.0;
	}
	

	nImageWidth0 = m_nImageWidth;
	nImageHeight0 = m_nImageHeight;

	if (nImageWidth0 > nImageHeight0)
	{
		nImageWidth0 = (int) (nImageWidth0 * fPixelAspectRatio);
	}
	else
	{
		nImageHeight0 = (int) (nImageHeight0 * fPixelAspectRatio);
	}


	if ((m_nRotateAngle % 180) != 0)
	{
//		nImageWidth = m_nImageHeight;
//		nImageHeight = m_nImageWidth;
		nImageWidth = nImageHeight0;
		nImageHeight = nImageWidth0;
	}
	else
	{
//		nImageWidth = m_nImageWidth;
//		nImageHeight = m_nImageHeight;
		nImageWidth = nImageWidth0;
		nImageHeight = nImageHeight0;
	}

	m_rtView.left = m_rtBorder.left;
	m_rtView.top = m_rtBorder.top;

	m_rtView.right = (int) ( m_rtView.left + nImageWidth * fZoomFactor);
	m_rtView.bottom = (int) (m_rtView.top + nImageHeight * fZoomFactor);
	
	nStepX = (m_rtView.Width() - m_rtBorder.Width()) / 2;
	nStepY = (m_rtView.Height() - m_rtBorder.Height()) / 2;

	m_rtView.left -= nStepX;
	m_rtView.right -= nStepX;
	m_rtView.top -= nStepY;
	m_rtView.bottom -= nStepY;

	m_rtView.NormalizeRect();

	m_rtProcessView.left = m_rtView.left + m_rtView.Width() / 4;
	m_rtProcessView.top  = m_rtView.top + m_rtView.Height() / 4;
	m_rtProcessView.right = m_rtProcessView.left + m_rtView.Width() / 2;
	m_rtProcessView.bottom = m_rtProcessView.top + m_rtView.Height() / 2;

}



void CCellWnd::ComputeExportImageSize(int &nImageWidth,int &nImageHeight)
{
	int nImageWidth0,nImageHeight0,nStepX,nStepY; 
	int nImageWidth1,nImageHeight1;
	int nImageWidth2,nImageHeight2;
	double f1,f2,fZoomFactor,fPixelAspectRatio;
	CRect rtBorder,rtView;


	if (m_pDicomObject != NULL)
	{
		fPixelAspectRatio = m_pDicomObject->GetPixelAspectRatio();
	}
	else
	{
		fPixelAspectRatio = 1.0;
	}

	nImageWidth0 = m_nImageWidth;
	nImageHeight0 = m_nImageHeight;

	if (nImageWidth0 > nImageHeight0)
	{
		nImageWidth0 = (int) (nImageWidth0 * fPixelAspectRatio);
	}
	else
	{
		nImageHeight0 = (int) (nImageHeight0 * fPixelAspectRatio);
	}


	if ((m_nRotateAngle % 180) != 0)
	{
		nImageWidth1 = nImageHeight0;
		nImageHeight1 = nImageWidth0;
	}
	else
	{
		nImageWidth1 = nImageWidth0;
		nImageHeight1 = nImageHeight0;
	}


	rtBorder.left = 0;
	rtBorder.top = 0;

	if (nImageWidth0 > 800)
	{
		rtBorder.right = 800;
		rtBorder.bottom = 800;
	}
	else
	{
		if ((m_nRotateAngle % 180) != 0)
		{
			rtBorder.right = nImageHeight1;
			rtBorder.bottom = nImageWidth1;
		}
		else
		{
			rtBorder.right = nImageWidth1;
			rtBorder.bottom = nImageHeight1;
		}		
	}

	/////////////////////////////////////////////////////////////////
	// Computer fZoomFactor

	if ((m_nRotateAngle % 180) != 0)
	{
		nImageWidth2 = m_nImageHeight;
		nImageHeight2 = m_nImageWidth;
	}
	else
	{
		nImageWidth2 = m_nImageWidth;
		nImageHeight2 = m_nImageHeight;
	}

	if (nImageWidth2 != 0)
	{
		f1 = ((double) rtBorder.Width() / nImageWidth2);
	}
	else
	{
		f1 = 1;
	}
		
	if (nImageHeight2 != 0)
	{
		f2 = ((double) rtBorder.Height() / nImageHeight2);
	}
	else
	{
		f2 = 1;
	}

	fZoomFactor = f1;

	if (fZoomFactor > f2) 
		fZoomFactor = f2;


	/////////////////////////////////////////////////////////////////
	
	rtView.left = rtBorder.left;
	rtView.top = rtBorder.top;

	rtView.right = (int) (rtView.left + nImageWidth1 * fZoomFactor);
	rtView.bottom = (int) (rtView.top + nImageHeight1 * fZoomFactor);
	
	nStepX = (rtView.Width() - rtBorder.Width()) / 2;
	nStepY = (rtView.Height() - rtBorder.Height()) / 2;

	rtView.left -= nStepX;
	rtView.right -= nStepX;
	rtView.top -= nStepY;
	rtView.bottom -= nStepY;

	rtView.NormalizeRect();

	nImageWidth = rtView.Width();
	nImageHeight = rtView.Height();

}



void CCellWnd::AddMeasurePoint(CPoint pt)
{
	CPointObj *pObj;
	
	pObj = new CPointObj;
	pObj->m_ptPoint.x = pt.x;
	pObj->m_ptPoint.y = pt.y;

	m_MeasurePointsList.AddTail(pObj);
}

void CCellWnd::ClearMeasurePointsList()
{
	POSITION pos;
	CPointObj *pObj;

	pos = m_MeasurePointsList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CPointObj *) m_MeasurePointsList.GetNext(pos);
		
		delete pObj;
	}

	m_MeasurePointsList.RemoveAll();
}


//calculate circumference of an ellipse

double CCellWnd::Circumference(double semimajor, double semiminor)
{
	int nLimit = 15,i,j,k;
    double fItem[20],e,l_deno, l_numa, pe, r_deno, mo, ne,tmp,l;


	if (semimajor >= semiminor)
    {
		e = sqrt(1 - pow(semiminor / semimajor, 2));
    }
    else
	{
        e = sqrt(1 - pow(semimajor / semiminor, 2));
    }

    for(j = 1; j < nLimit; ++j)
    {
        for(i = 1, l_deno = l_numa = mo = 1, ne = 2; i < j + 1; ++i, mo += 2, ne += 2)
        {
			l_numa *= mo;

            l_deno *= ne;          
        }

        pe = pow(e, 2 * j);

        r_deno = 2 * j - 1;

        fItem[j - 1] = l_numa * l_numa * pe /(l_deno * l_deno * r_deno);

     }

     tmp = 0.0;

     for(k = 0; k < nLimit - 1; ++k)
     {
         tmp += fItem[k];
     }

     l = 2 * PI * semimajor * (1 - tmp);

	 return l;
}


double CCellWnd::GetLineLengthOfPoint1AndPoint2()
{
	double fLength,fZoomFactor;
	int nStepX,nStepY;

	nStepX = m_ptMeasurePoint2.x - m_ptMeasurePoint1.x;
	nStepY = m_ptMeasurePoint2.y - m_ptMeasurePoint1.y;

	fLength = sqrt((double)(nStepX * nStepX + nStepY * nStepY));

	fZoomFactor = GetRealZoomFactor();
	fLength = fLength / fZoomFactor;

	return fLength;
}


void CCellWnd::DrawCTValue(CRect rtBorder,CDC *pDC)
{
	CRect rect,rtImageRect;
	OFCondition ofc;
	CDicomObject *pDicomObj;
	int ofst,nWidth,nHeight,nX0,nY0,nX,nY,nImageWidth,nImageHeight,nIdx;
	double fSlope,fIntercept,fPixelAspectRatio;
	CString sCTValue("");
	const Uint16 *pa = NULL;
	const unsigned char *pb = NULL;
	short nCTValue = 0;
	unsigned long num = 0;
	BOOL b8Bits;

	if (!DicomObjectIsExisting()) return;

	pDicomObj = GetDicomObject();

	if (pDicomObj == NULL) return;

	nImageWidth = pDicomObj->GetImageWidth();
	nImageHeight = pDicomObj->GetImageHeight();
	
	fPixelAspectRatio = pDicomObj->GetPixelAspectRatio();


	rtImageRect = GetImageViewRect();

	if (!rtImageRect.PtInRect(m_ptCTValue)) return;

	nX0 = (int)((m_ptCTValue.x - rtImageRect.left) / GetRealZoomFactor()) ;
	nY0 = (int)((m_ptCTValue.y - rtImageRect.top) / GetRealZoomFactor());
		
	nIdx = (m_nRotateAngle % 360) / 90;

	switch(nIdx)
	{
	case 0:
		nX = nX0;
		nY = nY0;
		break;
	case 1:
		nX = nY0;
		nY = nImageHeight - nX0;
		break;
	case 2:
		nX = nImageWidth - nX0;
		nY = nImageHeight - nY0;
		break;
	case 3:
		nX = nImageWidth - nY0;
		nY = nX0;
		break;
	}

	if (m_bHFlip)
	{
		nX = nImageWidth - nX;
	}

	if (m_bVFlip)
	{
		nY = nImageHeight - nY;
	}

	///////////////////////////////////////////////////
	if (nImageWidth > nImageHeight)
	{
		nX = (int) (nX / fPixelAspectRatio);
	}
	else
	{
		nY = (int) (nY / fPixelAspectRatio);
	}
	///////////////////////////////////////////////////

	if (nX >= nImageWidth) nX = nImageWidth - 1;
	if (nY >= nImageHeight) nY = nImageHeight - 1;


	ofst = nY * pDicomObj->GetImageWidth() + nX;

	fSlope = pDicomObj->GetRescaleSlope();
	fIntercept = pDicomObj->GetRescaleIntercept();

	b8Bits = (pDicomObj->GetBitsStored() == 8);
	if (b8Bits)
	{
		ofc = pDicomObj->GetDicomDataset()->findAndGetUint8Array(DCM_PixelData,pb,&num);
	}
	else
	{
		ofc = pDicomObj->GetDicomDataset()->findAndGetUint16Array(DCM_PixelData,pa,&num);
	}

	if (ofc == EC_Normal && ofst >= 0 && ofst < (int) num)
	{

		if (pDicomObj->HaveCTValue())
		{
			if (b8Bits)
			{
				nCTValue = (int) (pb[ofst] * fSlope + fIntercept);
			}
			else
			{
				nCTValue = (int) (pa[ofst] * fSlope + fIntercept);
			}

			sCTValue.Format(_T("(%d,%d)=%4d HU"),nX,nY,nCTValue);
		}
		else
		{
			if (b8Bits)
			{
				nCTValue = (int) (pb[ofst]);
			}
			else
			{
				nCTValue = (int) (pa[ofst]);
			}

			sCTValue.Format(_T("(%d,%d)=%4d"),nX,nY,nCTValue);
		}
		
		pDC->DrawText(sCTValue,rect,DT_CALCRECT);
		
		nHeight = rect.Height();
		nWidth = rect.Width();

		rect.left = m_ptCTValue.x - nWidth;
		rect.top = m_ptCTValue.y - nHeight;


		if (rect.top < rtBorder.top)
		{
			rect.top = m_ptCTValue.y;
		}

		if (rect.left < rtBorder.left)
		{
			rect.left = m_ptCTValue.x;
		}


		rect.right = rect.left + nWidth;
		rect.bottom = rect.top + nHeight;

		pDC->SetTextColor(m_pDicomViewer->GetTextColor());
		pDC->SetBkColor(m_pDicomViewer->GetBkColor());
		pDC->SetBkMode(m_pDicomViewer->GetBkMode());

		pDC->DrawText(sCTValue,&rect,DT_LEFT);
	}

}

void CCellWnd::DrawLineMeasure(CRect rtBorder,CDC *pDC)
{	
	CPen pen,*oldpen;
	double fLength,fx,fy,fZoomFactor;
	int nStepX,nStepY,nWidth,nHeight,nDelta;
	CString sText;
	CRect rect;
	LOGBRUSH logBrush;
	CDicomObject *pDicomObj;

	if (!DicomObjectIsExisting()) return;

	pDicomObj = GetDicomObject();


	pDicomObj->GetCalibrateValue(fx,fy);


	//pen.CreatePen(PS_SOLID,1,RGB(255,255,255));

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);

	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 2, &logBrush);

	oldpen = pDC->SelectObject(&pen);

	nStepX = m_ptMeasurePoint2.x - m_ptMeasurePoint1.x;
	nStepY = m_ptMeasurePoint2.y - m_ptMeasurePoint1.y;

	fLength = sqrt(nStepX * nStepX * fx * fx + nStepY * nStepY * fy * fy);

	fZoomFactor = GetRealZoomFactor();
	fLength = fLength / fZoomFactor;

	sText.Format(_T("Len = %.2f mm"), fLength);
	
	pDC->DrawText(sText,&rect,DT_CALCRECT);

	nWidth = rect.Width();
	nHeight = rect.Height();

	rect.left = m_ptMeasurePoint2.x + nStepX / 2 - nWidth / 2;
	rect.top = m_ptMeasurePoint2.y;
	rect.right = rect.left + nWidth;
	rect.bottom = rect.top + nHeight;

	if (rect.left < rtBorder.left)
	{
		nDelta = rtBorder.left - rect.left;

		rect.left = rect.left  + nDelta;
		rect.right = rect.right + nDelta;
	}

	if (rect.top < rtBorder.top)
	{
		nDelta = rtBorder.top - rect.top;

		rect.top = rect.top + nDelta;
		rect.bottom = rect.top + nDelta;
	}

	if (rect.right > rtBorder.right)
	{
		nDelta = rect.right - rtBorder.right;
		rect.right = rect.right - nDelta;
		rect.left = rect.left - nDelta;
	}

	if (rect.bottom > rtBorder.bottom)
	{
		nDelta = rect.bottom - rtBorder.bottom;
		rect.bottom = rect.bottom - nDelta;
		rect.top = rect.top - nDelta;
	}

	pDC->MoveTo(m_ptMeasurePoint1);
	pDC->LineTo(m_ptMeasurePoint2);

	pDC->SetTextColor(m_pDicomViewer->GetTextColor());
	pDC->SetBkColor(m_pDicomViewer->GetBkColor());
	pDC->SetBkMode(m_pDicomViewer->GetBkMode());	

	pDC->DrawText(sText,&rect,DT_LEFT);
	
	pDC->SelectObject(oldpen);
}

void CCellWnd::DrawRectangleMeasure(CRect rtBorder,CDC *pDC)
{
	OFCondition ofc;
	CPen pen,*oldpen;
	CBrush br,*oldbr;
	double fArea,fx,fy,fZoomFactor,fPixelAspectRatio,fLength;
	int nStepX,nStepY,nWidth,nHeight,nDelta,ni,nj,nImageWidth,nImageHeight;
	int nX,nY,nX0,nY0,nIdx,nCTValue,ofst,nPointsCount,nMinValue,nMaxValue;
	CString sText;
	CRect rect,rtRect,rtImageRect;
	LOGBRUSH logBrush;
	double fSlope,fIntercept;
	int nMean;	
	CDicomObject *pDicomObj;
	const Uint16 *pa = NULL;
	const unsigned char *pb = NULL;
	unsigned long num = 0;
	BOOL bHaveCTValue,b8Bits = FALSE;

	if (!DicomObjectIsExisting()) return;
	
	pDicomObj = GetDicomObject();

	if (pDicomObj == NULL) return;
	
	fPixelAspectRatio = pDicomObj->GetPixelAspectRatio();

	/*
	fx = pDicomObj->GetFloat64(DCM_PixelSpacing,0);
	fy = pDicomObj->GetFloat64(DCM_PixelSpacing,1);

	if (fx == 0.0 && fy == 0.0)
	{
		fx = GetDicomObject()->GetFloat64(DCM_ImagerPixelSpacing,0);
		fy = GetDicomObject()->GetFloat64(DCM_ImagerPixelSpacing,1);
	}
	*/

	pDicomObj->GetCalibrateValue(fx,fy);

	nImageWidth = pDicomObj->GetImageWidth();
	nImageHeight = pDicomObj->GetImageHeight();

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);
	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 1, &logBrush);

	oldpen = pDC->SelectObject(&pen);

	nStepX = m_ptMeasurePoint2.x - m_ptMeasurePoint1.x;
	nStepY = m_ptMeasurePoint2.y - m_ptMeasurePoint1.y;

	if (nStepX < 0) nStepX = - nStepX;
	if (nStepY < 0) nStepY = - nStepY;


	fZoomFactor = GetRealZoomFactor();

	fArea = ((nStepX * fx) / fZoomFactor) * ((nStepY *fy) / fZoomFactor);

	fLength = 2 * (((nStepX * fx) / fZoomFactor) + ((nStepY *fy) / fZoomFactor));
				
	rtRect.top = m_ptMeasurePoint1.y;
	rtRect.left = m_ptMeasurePoint1.x;
	rtRect.right = m_ptMeasurePoint2.x;
	rtRect.bottom = m_ptMeasurePoint2.y;

	rtRect.NormalizeRect();

	bHaveCTValue = pDicomObj->HaveCTValue();

	fSlope = pDicomObj->GetRescaleSlope();
	fIntercept = pDicomObj->GetRescaleIntercept();

	rtImageRect = GetImageViewRect();

	nMean = 0;
	nPointsCount = 0;

	nMaxValue = -40960;
	nMinValue = 40960;


	for (nj = rtRect.top ; nj <= rtRect.bottom ; nj ++)
	{
		for (ni = rtRect.left; ni <= rtRect.right; ni ++)
		{
			nX0 = (int) ((ni  - rtImageRect.left) / GetRealZoomFactor()) ;
			nY0 = (int) ((nj  - rtImageRect.top) / GetRealZoomFactor());

			nIdx = (m_nRotateAngle % 360) / 90;

			switch(nIdx)
			{
			case 0:
				nX = nX0;
				nY = nY0;
				break;
			case 1:
				nX = nY0;
				nY = nImageHeight - nX0;
				break;
			case 2:
				nX = nImageWidth - nX0;
				nY = nImageHeight - nY0;
				break;
			case 3:
				nX = nImageWidth - nY0;
				nY = nX0;
				break;
			}	

			if (m_bHFlip)
			{
				nX = nImageWidth - nX;
			}

			if (m_bVFlip)
			{
				nY = nImageHeight - nY;
			}



			///////////////////////////////////////////////////
			if (nImageWidth > nImageHeight)
			{
				nX = (int) (nX / fPixelAspectRatio);
			}
			else
			{
				nY = (int) (nY / fPixelAspectRatio);
			}
			///////////////////////////////////////////////////


			if (nX >= nImageWidth) nX = nImageWidth - 1;
			if (nY >= nImageHeight) nY = nImageHeight - 1;

			ofst = nY * pDicomObj->GetImageWidth() + nX;

			b8Bits = (pDicomObj->GetBitsStored() == 8);			

			if (b8Bits)
			{
				ofc = pDicomObj->GetDicomDataset()->findAndGetUint8Array(DCM_PixelData,pb,&num);
			}
			else
			{
				ofc = pDicomObj->GetDicomDataset()->findAndGetUint16Array(DCM_PixelData,pa,&num);
			}

	
			if (ofc == EC_Normal && ofst >= 0 && ofst < (int) num)
			{
				nPointsCount ++;

				if (bHaveCTValue)
				{
					if (b8Bits)
					{
						nCTValue = (int) (pb[ofst] * fSlope + fIntercept);
					}
					else
					{
						nCTValue = (int) (pa[ofst] * fSlope + fIntercept);
					}
				}
				else
				{
					if (b8Bits)
					{
						nCTValue = (int) (pb[ofst]);
					}
					else
					{
						nCTValue = (int) (pa[ofst]);
					}
				}

				if (nCTValue > nMaxValue) nMaxValue = nCTValue;
				if (nCTValue < nMinValue) nMinValue = nCTValue;

				nMean = nMean + nCTValue;
			}
		}
	}
	
	if (nPointsCount > 0)
	{
		nMean = nMean / nPointsCount;
	}

	if (bHaveCTValue)
	{
		sText.Format(_T("Area = %.2f mm^2 \nLen = %.2f mm \nMean = %d HU\nMin  = %d HU\nMax  = %d HU"), fArea,fLength,nMean,nMinValue,nMaxValue);
	}
	else
	{
		sText.Format(_T("Area = %.2f mm^2 \nLen = %.2f mm \nMean = %d \nMin  = %d \nMax  = %d "), fArea,fLength,nMean,nMinValue,nMaxValue);
	}
	
	pDC->DrawText(sText,&rect,DT_CALCRECT);

	nWidth = rect.Width();
	nHeight = rect.Height();

	rect.left = m_ptMeasurePoint2.x + nStepX / 2 - nWidth / 2;
	rect.top = m_ptMeasurePoint2.y;
	rect.right = rect.left + nWidth;
	rect.bottom = rect.top + nHeight;

	if (rect.left < rtBorder.left)
	{
		nDelta = rtBorder.left - rect.left;

		rect.left = rect.left  + nDelta;
		rect.right = rect.right + nDelta;
	}

	if (rect.top < rtBorder.top)
	{
		nDelta = rtBorder.top - rect.top;

		rect.top = rect.top + nDelta;
		rect.bottom = rect.top + nDelta;
	}

	if (rect.right > rtBorder.right)
	{
		nDelta = rect.right - rtBorder.right;
		rect.right = rect.right - nDelta;
		rect.left = rect.left - nDelta;
	}

	if (rect.bottom > rtBorder.bottom)
	{
		nDelta = rect.bottom - rtBorder.bottom;
		rect.bottom = rect.bottom - nDelta;
		rect.top = rect.top - nDelta;
	}


	oldbr = (CBrush *) pDC->SelectStockObject(NULL_BRUSH);

	pDC->Rectangle(&rtRect);

	pDC->SetTextColor(m_pDicomViewer->GetTextColor());
	pDC->SetBkColor(m_pDicomViewer->GetBkColor());
	pDC->SetBkMode(m_pDicomViewer->GetBkMode());

	pDC->DrawText(sText,&rect,DT_LEFT);
	
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbr);

}


void CCellWnd::DrawLocalizer(CRect rtBorder,CDC *pDC)
{
	CDicomObject *pDicomObj1,*pDicomObj2;
	CCellWnd *pCellWnd;
	CsliceProcessing slice;
	CString scoutPos,scoutOrient,scoutPixSpace;
	CString imgPos,imgOrient,imgPixSpace,sImgNo("");
	int nScoutRows,nScoutCols,nImgRows,nImgCols;
	int nUlx = 0,nUly = 0,nUrx = 0,nUry = 0,nLlx = 0,nLly = 0,nLrx = 0,nLry = 0;
	int nX1,nY1,nX2,nY2,nX1_0,nY1_0,nX2_0,nY2_0,nIdx;
	CPen pen,*oldpen;
	double fZoomFactor;
	
	if (!DicomObjectIsExisting()) return;
	
	pDicomObj1 = GetDicomObject();

	if (pDicomObj1 == NULL) return;
	
	if (!pDicomObj1->IsLocalizer()) return;

	pCellWnd = m_pDicomViewer->GetCellWndSelected();

	if (pCellWnd == NULL) return;

	if (pCellWnd == this) return;

	pDicomObj2 = pCellWnd->GetDicomObject();

	if (pDicomObj2 == NULL) return;

	if (pDicomObj2->IsLocalizer()) return;

	if (pDicomObj1->GetFrameOfReferenceUID() != pDicomObj2->GetFrameOfReferenceUID()) return;

	
	nScoutRows = pDicomObj1->GetImageHeight();
	nScoutCols = pDicomObj1->GetImageWidth();
	
	nImgRows = pDicomObj2->GetImageHeight();
	nImgCols = pDicomObj2->GetImageWidth();


	scoutPos = pDicomObj1->GetString(DCM_ImagePositionPatient);
	scoutOrient = pDicomObj1->GetString(DCM_ImageOrientationPatient);
	scoutPixSpace = pDicomObj1->GetString(DCM_PixelSpacing);


	imgPos = pDicomObj2->GetString(DCM_ImagePositionPatient);
	imgOrient = pDicomObj2->GetString(DCM_ImageOrientationPatient);
	imgPixSpace = pDicomObj2->GetString(DCM_PixelSpacing);

	sImgNo = pDicomObj2->GetString(DCM_InstanceNumber);


	slice.ProjectSlice(scoutPos,scoutOrient,scoutPixSpace,nScoutRows,nScoutCols,imgPos,imgOrient,imgPixSpace,nImgRows,nImgCols);
	

	slice.getBoundingBox(nUlx,nUly,nUrx,nUry,nLlx,nLly,nLrx,nLry);

	fZoomFactor = GetRealZoomFactor();


	if (nUlx == nUrx && nUly == nUry)
	{
		nX1 = nUlx;
		nY1 = nUly;

		nX2 = nLlx;
		nY2 = nLly;
	}
	else 
	{
		nX1 = nUlx;
		nY1 = nUly;

		nX2 = nUrx;
		nY2 = nUry;
	}


	if (m_bHFlip)
	{
		nX1 = nScoutCols - nX1;
		nX2 = nScoutCols - nX2;
	}

	if (m_bVFlip)
	{
		nY1 = nScoutRows - nY1;
		nY2 = nScoutRows - nY2;
	}


	nX1_0 = nX1;
	nY1_0 = nY1;

	nX2_0 = nX2;
	nY2_0 = nY2;

	nIdx = (m_nRotateAngle % 360) / 90;
	switch(nIdx)
	{
	case 0:

		break;
	case 1:
		
		nX1_0 = nScoutRows - nY1;
		nY1_0 = nX1;

		nX2_0 = nScoutRows - nY2;
		nY2_0 = nX2;

		break;
	case 2:

		nX1_0 = nScoutCols - nX1;
		nY1_0 = nScoutRows - nY1;

		nX2_0 = nScoutCols - nX2;
		nY2_0 = nScoutRows - nY2;

		break;
	case 3:

		nX1_0 = nY1;
		nY1_0 = nScoutCols - nX1;

		nX2_0 = nY2;
		nY2_0 = nScoutCols - nX2;

		break;
	}	

	nX1 = nX1_0;
	nY1 = nY1_0;

	nX2 = nX2_0;
	nY2 = nY2_0;

	nX1 = nX1 * fZoomFactor + m_rtView.left + m_nShiftX;
	nY1 = nY1 * fZoomFactor + m_rtView.top + m_nShiftY;

	nX2 = nX2 * fZoomFactor + m_rtView.left + m_nShiftX;
	nY2 = nY2 * fZoomFactor + m_rtView.top + m_nShiftY;

	pen.CreatePen(PS_DOT,1,RGB(255,50,50));

	oldpen = pDC->SelectObject(&pen); 
	
	pDC->MoveTo(nX1,nY1);
	pDC->LineTo(nX2,nY2);

	pDC->SetTextColor(RGB(255,0,0));

	pDC->TextOut(nX1,nY1,sImgNo);

	pDC->SelectObject(oldpen);

}




void CCellWnd::DrawEllipseMeasure(CRect rtBorder,CDC *pDC)
{
	OFCondition ofc;
	CPen pen,*oldpen;
	CBrush br,*oldbr;
	double fa,fb,fArea,fx,fy,fZoomFactor,fPixelAspectRatio,fCircumference;
	int nStepX,nStepY,nWidth,nHeight,nDelta,ni,nj,nImageWidth,nImageHeight;
	int nX,nY,nX0,nY0,nIdx,nCTValue,ofst,nPointsCount,nMinValue,nMaxValue;	
	CString sText;
	CRect rect,rtEllipse,rtImageRect;
	LOGBRUSH logBrush;
	CRgn rgn;
	double fSlope,fIntercept;
	int nMean;	
	CDicomObject *pDicomObj;
	const Uint16 *pa = NULL;
	const unsigned char *pb = NULL;
	unsigned long num = 0;
	BOOL bHaveCTValue,b8Bits;
	
	if (!DicomObjectIsExisting()) return;

	pDicomObj = GetDicomObject();

	if (pDicomObj == NULL) return;

	
	fPixelAspectRatio = pDicomObj->GetPixelAspectRatio();


	/*
	fx = pDicomObj->GetFloat64(DCM_PixelSpacing,0);
	fy = pDicomObj->GetFloat64(DCM_PixelSpacing,1);


	if (fx == 0.0 && fy == 0.0)
	{
		fx = pDicomObj->GetFloat64(DCM_ImagerPixelSpacing,0);
		fy = pDicomObj->GetFloat64(DCM_ImagerPixelSpacing,1);
	}
	*/
	
	pDicomObj->GetCalibrateValue(fx,fy);


	nImageWidth = pDicomObj->GetImageWidth();
	nImageHeight = pDicomObj->GetImageHeight();

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);
	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 1, &logBrush);

	oldpen = pDC->SelectObject(&pen);


	nStepX = m_ptMeasurePoint2.x - m_ptMeasurePoint1.x;
	nStepY = m_ptMeasurePoint2.y - m_ptMeasurePoint1.y;

	if (nStepX < 0) nStepX = -nStepX;
	if (nStepY < 0) nStepY = -nStepY;

	fa = nStepX * fx;
	fb = nStepY * fy;

	fZoomFactor = GetRealZoomFactor();

	fArea =  PI * fa * fb / (fZoomFactor * fZoomFactor);

	fCircumference = Circumference(fa,fb) / fZoomFactor;
	
	rtEllipse.top = m_ptMeasurePoint1.y - ( m_ptMeasurePoint2.y - m_ptMeasurePoint1.y);
	rtEllipse.left = m_ptMeasurePoint1.x - (m_ptMeasurePoint2.x - m_ptMeasurePoint1.x);
	rtEllipse.right = m_ptMeasurePoint2.x;
	rtEllipse.bottom = m_ptMeasurePoint2.y;

	rtEllipse.NormalizeRect();


	rgn.CreateEllipticRgn(rtEllipse.left,rtEllipse.top,rtEllipse.right,rtEllipse.bottom);


	bHaveCTValue = pDicomObj->HaveCTValue();

	fSlope = pDicomObj->GetRescaleSlope();
	fIntercept = pDicomObj->GetRescaleIntercept();

	rtImageRect = GetImageViewRect();

	nMean = 0;
	nPointsCount = 0;

	nMaxValue = -40960;
	nMinValue = 40960;

	for (nj = rtEllipse.top ; nj <= rtEllipse.bottom ; nj ++)
	{
		for (ni = rtEllipse.left; ni <= rtEllipse.right; ni ++)
		{
			if (rgn.PtInRegion(ni,nj))
			{
				nX0 = (int) ((ni  - rtImageRect.left) / GetRealZoomFactor()) ;
				nY0 = (int) ((nj  - rtImageRect.top) / GetRealZoomFactor());

				nIdx = (m_nRotateAngle % 360) / 90;

				switch(nIdx)
				{
				case 0:
					nX = nX0;
					nY = nY0;
					break;
				case 1:
					nX = nY0;
					nY = nImageHeight - nX0;
					break;
				case 2:
					nX = nImageWidth - nX0;
					nY = nImageHeight - nY0;
					break;
				case 3:
					nX = nImageWidth - nY0;
					nY = nX0;
					break;
				}	

				if (m_bHFlip)
				{
					nX = nImageWidth - nX;
				}

				if (m_bVFlip)
				{
					nY = nImageHeight - nY;
				}


				///////////////////////////////////////////////////
				if (nImageWidth > nImageHeight)
				{
					nX = (int) (nX / fPixelAspectRatio);
				}
				else
				{
					nY = (int) (nY / fPixelAspectRatio);
				}
				///////////////////////////////////////////////////
				


				if (nX >= nImageWidth) nX = nImageWidth - 1;
				if (nY >= nImageHeight) nY = nImageHeight - 1;

				ofst = nY * pDicomObj->GetImageWidth() + nX;

				b8Bits = (pDicomObj->GetBitsStored() == 8);
				
				if (b8Bits)
				{
					ofc = pDicomObj->GetDicomDataset()->findAndGetUint8Array(DCM_PixelData,pb,&num);
				}
				else
				{
					ofc = pDicomObj->GetDicomDataset()->findAndGetUint16Array(DCM_PixelData,pa,&num);
				}
	
				if (ofc == EC_Normal && ofst >= 0 && ofst < (int) num)
				{
					nPointsCount ++;

					if (bHaveCTValue)
					{
						if (b8Bits)
						{
							nCTValue = (int) (pb[ofst] * fSlope + fIntercept);
						}
						else
						{
							nCTValue = (int) (pa[ofst] * fSlope + fIntercept);
						}
					}
					else
					{
						if (b8Bits)
						{
							nCTValue = (int) (pb[ofst]);
						}
						else
						{
							nCTValue = (int) (pa[ofst]);
						}
					}

					if (nCTValue > nMaxValue) nMaxValue = nCTValue;
					if (nCTValue < nMinValue) nMinValue = nCTValue;

					nMean = nMean + nCTValue;
				}
			}
		}
	}

	if (nPointsCount > 0)
	{
		nMean = nMean / nPointsCount;

		if (bHaveCTValue)
		{
			sText.Format(_T("Area = %.2f mm^2 \nLen =%.2f mm \nMean = %d HU\nMin = %d HU\nMax = %d HU"), fArea,fCircumference,nMean,nMinValue,nMaxValue);
		}
		else
		{
			sText.Format(_T("Area = %.2f mm^2 \nLen =%.2f mm \nMean = %d \nMin = %d \nMax = %d "), fArea,fCircumference,nMean,nMinValue,nMaxValue);
		}
	}
	else
	{
		sText = "";
	}


	
	pDC->DrawText(sText,&rect,DT_CALCRECT);

	nWidth = rect.Width();
	nHeight = rect.Height();

	rect.left = m_ptMeasurePoint2.x + nStepX / 2 - nWidth / 2;
	rect.top = m_ptMeasurePoint2.y;
	rect.right = rect.left + nWidth;
	rect.bottom = rect.top + nHeight;

	if (rect.left < rtBorder.left)
	{
		nDelta = rtBorder.left - rect.left;

		rect.left = rect.left  + nDelta;
		rect.right = rect.right + nDelta;
	}

	if (rect.top < rtBorder.top)
	{
		nDelta = rtBorder.top - rect.top;

		rect.top = rect.top + nDelta;
		rect.bottom = rect.top + nDelta;
	}

	if (rect.right > rtBorder.right)
	{
		nDelta = rect.right - rtBorder.right;
		rect.right = rect.right - nDelta;
		rect.left = rect.left - nDelta;
	}

	if (rect.bottom > rtBorder.bottom)
	{
		nDelta = rect.bottom - rtBorder.bottom;
		rect.bottom = rect.bottom - nDelta;
		rect.top = rect.top - nDelta;
	}


	oldbr = (CBrush *) pDC->SelectStockObject(NULL_BRUSH);

	pDC->Ellipse(&rtEllipse);

	pDC->SetTextColor(m_pDicomViewer->GetTextColor());
	pDC->SetBkColor(m_pDicomViewer->GetBkColor());
	pDC->SetBkMode(m_pDicomViewer->GetBkMode());

	
	pDC->DrawText(sText,&rect,DT_LEFT);
	
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbr);

}


void CCellWnd::DrawPolygonMeasure(CRect rtBorder,CDC *pDC)
{
	CPen pen,*oldpen;
	double fArea,fx,fy,fZoomFactor,fv,fLength;
	int ni,nWidth,nHeight,nx1,ny1,nx2,ny2,nMinX,nMinXIdx,nIdx,nStepX,nStepY;
	CString sText;
	CRect rect;
	LOGBRUSH logBrush;
	POSITION pos,pos1,pos2;
	CPointObj *pObj,*pStartObj,*pObj1,*pObj2;
	int nPointCount;
	CPoint *pPoints;
	CDicomObject *pDicomObj;

	if (!DicomObjectIsExisting()) return;

	pDicomObj = GetDicomObject();
	if (pDicomObj == NULL) return;

	nPointCount = (int) m_MeasurePointsList.GetCount();

	/*

	fx = pDicomObj->GetFloat64(DCM_PixelSpacing,0);
	fy = pDicomObj->GetFloat64(DCM_PixelSpacing,1);


	if (fx == 0.0 && fy == 0.0)
	{
		fx = pDicomObj->GetFloat64(DCM_ImagerPixelSpacing,0);
		fy = pDicomObj->GetFloat64(DCM_ImagerPixelSpacing,1);
	}
	*/

	pDicomObj->GetCalibrateValue(fx,fy);


	fArea = 0.0;
	fLength = 0.0;
	fZoomFactor = GetRealZoomFactor();

	if (nPointCount >= 3)
	{
		
		ni = 0;
		while (ni < nPointCount)
		{
			pos1 = m_MeasurePointsList.FindIndex(ni);
			
			if ((ni + 1) < nPointCount )
			{
				pos2 = m_MeasurePointsList.FindIndex(ni+1);
			}
			else
			{
				pos2 = m_MeasurePointsList.FindIndex(0);
			}

			pObj1 = (CPointObj *) m_MeasurePointsList.GetAt(pos1);
			pObj2 = (CPointObj *) m_MeasurePointsList.GetAt(pos2);


			nStepX = pObj2->m_ptPoint.x - pObj1->m_ptPoint.x;
			nStepY = pObj2->m_ptPoint.y - pObj1->m_ptPoint.y;

			fLength = fLength + sqrt(nStepX * nStepX * fx * fx + nStepY * nStepY * fy * fy);

			ni ++;
		}

		fLength = fLength / fZoomFactor;

		pPoints = new CPoint[nPointCount];
		
		pos = m_MeasurePointsList.FindIndex(0);
		pObj = (CPointObj *) m_MeasurePointsList.GetAt(pos);
		
		nMinX = pObj->m_ptPoint.x;
		nMinXIdx = 0;
		
		ni = 1;
		while (ni < nPointCount)
		{
			pos = m_MeasurePointsList.FindIndex(ni);
			pObj = (CPointObj *) m_MeasurePointsList.GetAt(pos);

			if (pObj->m_ptPoint.x < nMinX)
			{
				nMinX = pObj->m_ptPoint.x;
				nMinXIdx = ni;
			}

			ni ++;
		}
			
		nIdx = 0;
		ni = nMinXIdx;
		while (ni < nPointCount)
		{
			pos = m_MeasurePointsList.FindIndex(ni);
			pObj = (CPointObj *) m_MeasurePointsList.GetAt(pos);
			
			pPoints[nIdx].x = pObj->m_ptPoint.x;
			pPoints[nIdx].y = pObj->m_ptPoint.y;

			nIdx ++;
			ni ++;
		}


		ni = 0;
		while (ni < nMinXIdx)
		{
			pos = m_MeasurePointsList.FindIndex(ni);
			pObj = (CPointObj *) m_MeasurePointsList.GetAt(pos);

			pPoints[nIdx].x = pObj->m_ptPoint.x;
			pPoints[nIdx].y = pObj->m_ptPoint.y;

			nIdx ++;

			ni ++;
		}

		nx1 = pPoints[0].x;
		ny1 = pPoints[0].y;

		nx2 = pPoints[1].x;
		ny2 = pPoints[1].y;

		fv = (nx2 - nx1) * (ny1 + ny2) / 2.0;
		
		fArea = fv;

		ni = 2;

		while (ni <= nPointCount - 1)
		{
			nx1 = nx2;
			ny1 = ny2;

			nx2 = pPoints[ni].x;
			ny2 = pPoints[ni].y;

			fv = (nx2 - nx1) * (ny1 + ny2) / 2.0;
			
			fArea = fArea + fv;

			ni ++;
		}

		fArea = fArea  * fx  * fy / (fZoomFactor * fZoomFactor);

		if (fArea < 0) fArea = -fArea;

		delete []pPoints;
	}


	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);
	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 1, &logBrush);

	oldpen = pDC->SelectObject(&pen);

	pStartObj = (CPointObj *) m_MeasurePointsList.GetHead();
	
	sText.Format(_T("Area=%.4f mm^2 \nLen = %.2f mm"),fArea,fLength);

	pDC->DrawText(sText,rect,DT_CALCRECT);

	nWidth = rect.Width();
	nHeight = rect.Height();
	
	rect.left = pStartObj->m_ptPoint.x - nWidth;
	rect.top = pStartObj->m_ptPoint.y - nHeight;


	if (rect.top < rtBorder.top)
	{
		rect.top = pStartObj->m_ptPoint.y;
	}

	if (rect.left < rtBorder.left)
	{
		rect.left = pStartObj->m_ptPoint.x;
	}

	rect.right = rect.left + nWidth;
	rect.bottom = rect.top + nHeight;

	pDC->MoveTo(pStartObj->m_ptPoint);

	pos = m_MeasurePointsList.GetHeadPosition();
	
	while (pos != NULL)
	{
		pObj = (CPointObj *) m_MeasurePointsList.GetNext(pos);
		
		pDC->LineTo(pObj->m_ptPoint);
	}

	pDC->LineTo(pStartObj->m_ptPoint);

	pDC->SetTextColor(m_pDicomViewer->GetTextColor());
	pDC->SetBkColor(m_pDicomViewer->GetBkColor());
	pDC->SetBkMode(m_pDicomViewer->GetBkMode());

	if (nPointCount >= 3)
	{
		pDC->DrawText(sText,&rect,DT_LEFT);
	}
	
	pDC->SelectObject(oldpen);
}



void CCellWnd::DrawAngleMeasure(CRect rtBorder,CDC *pDC)
{

	CPen pen,*oldpen;
	int nStepX,nStepY,nWidth,nHeight,nDelta;
	CString sText;
	CRect rect;
	LOGBRUSH logBrush;
	double fs,fa,fb,fc,cosc;


	if (!DicomObjectIsExisting()) return;

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);
	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 2, &logBrush);

	oldpen = pDC->SelectObject(&pen);

	if (m_ptMeasureAnglePoint1 == m_ptMeasureAnglePoint2)
	{
		pDC->MoveTo(m_ptMeasureAnglePoint1);
		pDC->LineTo(m_ptMeasurePoint);
	}
	else if (m_ptMeasureAnglePoint1 != m_ptMeasureAnglePoint2)
	{

		pDC->MoveTo(m_ptMeasureAnglePoint1);
		pDC->LineTo(m_ptMeasureAnglePoint2);

		pDC->MoveTo(m_ptMeasureAnglePoint1);
		pDC->LineTo(m_ptMeasurePoint);

		nStepX = m_ptMeasureAnglePoint2.x - m_ptMeasureAnglePoint1.x;
		nStepY = m_ptMeasureAnglePoint2.y - m_ptMeasureAnglePoint1.y;

		fa = sqrt(nStepX * nStepX * 1.0 + nStepY * nStepY * 1.0);

		nStepX = m_ptMeasurePoint.x - m_ptMeasureAnglePoint1.x;
		nStepY = m_ptMeasurePoint.y - m_ptMeasureAnglePoint1.y;


		fb = sqrt(nStepX * nStepX * 1.0 + nStepY * nStepY * 1.0);


		nStepX = m_ptMeasurePoint.x - m_ptMeasureAnglePoint2.x;
		nStepY = m_ptMeasurePoint.y - m_ptMeasureAnglePoint2.y;

		fc = sqrt(nStepX * nStepX * 1.0 + nStepY * nStepY * 1.0);

		if (fa == 0 || fb == 0)
		{
			sText = "";
		}
		else
		{
			cosc = ((fa * fa + fb * fb - fc * fc) / (2 * fa * fb));

			fs = acos(cosc) * 180.0 / PI;

			sText.Format(_T("Angle = %5.1f°„"), fs);
		}
	
		pDC->DrawText(sText,&rect,DT_CALCRECT);

		nWidth = rect.Width();
		nHeight = rect.Height();

		rect.left = m_ptMeasureAnglePoint2.x + nStepX / 2 - nWidth / 2;
		rect.top = m_ptMeasureAnglePoint2.y;
		rect.right = rect.left + nWidth;
		rect.bottom = rect.top + nHeight;

		if (rect.left < rtBorder.left)
		{
			nDelta = rtBorder.left - rect.left;

			rect.left = rect.left  + nDelta;
			rect.right = rect.right + nDelta;
		}

		if (rect.top < rtBorder.top)
		{
			nDelta = rtBorder.top - rect.top;

			rect.top = rect.top + nDelta;
			rect.bottom = rect.top + nDelta;
		}

		if (rect.right > rtBorder.right)
		{
			nDelta = rect.right - rtBorder.right;
			rect.right = rect.right - nDelta;
			rect.left = rect.left - nDelta;
		}

		if (rect.bottom > rtBorder.bottom)
		{
			nDelta = rect.bottom - rtBorder.bottom;
			rect.bottom = rect.bottom - nDelta;
			rect.top = rect.top - nDelta;
		}


		pDC->SetTextColor(m_pDicomViewer->GetTextColor());
		pDC->SetBkColor(m_pDicomViewer->GetBkColor());
		pDC->SetBkMode(m_pDicomViewer->GetBkMode());	

		pDC->DrawText(sText,&rect,DT_LEFT);

	}

	pDC->SelectObject(oldpen);

}


void CCellWnd::DrawCobbMeasure(CRect rtBorder,CDC *pDC)
{	
	CPen pen,*oldpen;
	double fs,fk;
	int ndx1,ndy1,ndx2,ndy2;
	CString sText;
	LOGBRUSH logBrush;
	CPoint ptPoint2(-1,-1),ptPoint4(-1,-1),ptMidPoint1,ptMidPoint2,ptLine1,ptLine2;

	if (!DicomObjectIsExisting()) return;

	if ((m_ptMeasureCobbPoint1.x == -1 && m_ptMeasureCobbPoint1.y == -1) || (m_ptMeasurePoint.x < 0 || m_ptMeasurePoint.y < 0)) return;

	ptPoint2 = m_ptMeasureCobbPoint2;
	ptPoint4 = m_ptMeasureCobbPoint4;

	if (m_ptMeasureCobbPoint2.x == -1 && m_ptMeasureCobbPoint2.y == -1)
	{
		ptPoint2 = m_ptMeasurePoint;
	}
	else if (m_ptMeasureCobbPoint4.x == -1 && m_ptMeasureCobbPoint4.y == -1)
	{
		ptPoint4 = m_ptMeasurePoint;
	}

	ndx1 = ptPoint2.x - m_ptMeasureCobbPoint1.x;
	ndy1 = ptPoint2.y - m_ptMeasureCobbPoint1.y;
	ndx2 = ptPoint4.x - m_ptMeasureCobbPoint3.x;
	ndy2 = ptPoint4.y - m_ptMeasureCobbPoint3.y;

	fk = sqrt(ndx1 * ndx1 * 1.0 + ndy1 * ndy1 *1.0)  * sqrt(ndx2 * ndx2 * 1.0 + ndy2 * ndy2 * 1.0);

	if (fk != 0)
	{
		fs = acos( ((double)(ndx1 * ndx2 + ndy1 * ndy2)) / fk );
		fs = fs * 180.0 / PI;
	}
	else
	{
		fs = 0;
	}
    
	
	//pen.CreatePen(PS_SOLID,1,RGB(255,255,255));

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255,0,0);
	pen.CreatePen(PS_SOLID | PS_GEOMETRIC|PS_ENDCAP_ROUND, 2, &logBrush);

	oldpen = pDC->SelectObject(&pen);


	if (m_ptMeasureCobbPoint3.x == -1 && m_ptMeasureCobbPoint3.y == -1)
	{
		pDC->MoveTo(m_ptMeasureCobbPoint1);
		pDC->LineTo(ptPoint2);
	}
	else 
	{
		ptMidPoint1.x = (m_ptMeasureCobbPoint1.x + ptPoint2.x) / 2;
		ptMidPoint1.y = (m_ptMeasureCobbPoint1.y + ptPoint2.y) / 2;
	
		ptMidPoint2.x = (m_ptMeasureCobbPoint3.x + ptPoint4.x) / 2;
		ptMidPoint2.y = (m_ptMeasureCobbPoint3.y + ptPoint4.y) / 2;

		if (ndx1 == 0)
		{			
			ptLine1.x = 0;
			ptLine1.y = ptMidPoint1.y;
		}
		else
		{
			if (ndy1 == 0)
			{
				ptLine1.x = ptMidPoint1.x;
				ptLine1.y = 0;
			}
			else
			{
				fk = - ndx1 * 1.0 / ndy1 * 1.0;

				if (fk < 0)
				{
					ptLine1.x = m_ptMeasureCobbPoint1.x;
					ptLine1.y = ptMidPoint1.y + fk * (ptLine1.x - ptMidPoint1.x);
				}
				else
				{
					ptLine1.x = ptPoint2.x;
					ptLine1.y = ptMidPoint1.y + fk * (ptLine1.x - ptMidPoint1.x);
				}
			}
		}

		if (ndx2 == 0)
		{			
			ptLine2.x = 0;
			ptLine2.y = ptMidPoint2.y;
		}
		else
		{
			if (ndy2 == 0)
			{
				ptLine2.x = ptMidPoint2.x;
				ptLine2.y = 0;
			}
			else
			{
				fk = - ndx2 * 1.0 / ndy2 * 1.0;

				if (fk > 0)
				{
					ptLine2.x = m_ptMeasureCobbPoint3.x;
					ptLine2.y = ptMidPoint2.y + fk * (ptLine2.x - ptMidPoint2.x);
				}
				else
				{
					ptLine2.x = ptPoint4.x;
					ptLine2.y = ptMidPoint2.y + fk * (ptLine2.x - ptMidPoint2.x);
				}
			}
		}


		pDC->MoveTo(ptMidPoint1);
		pDC->LineTo(ptLine1);

		pDC->MoveTo(ptMidPoint2);
		pDC->LineTo(ptLine2);

		sText.Format(_T("CobbΩ«∂» = %5.1f°„"), fs);

		pDC->MoveTo(m_ptMeasureCobbPoint1);
		pDC->LineTo(m_ptMeasureCobbPoint2);

		pDC->MoveTo(m_ptMeasureCobbPoint3);
		pDC->LineTo(ptPoint4);

		pDC->SetTextColor(m_pDicomViewer->GetTextColor());
		pDC->SetBkColor(m_pDicomViewer->GetBkColor());
		pDC->SetBkMode(m_pDicomViewer->GetBkMode());	

		pDC->DrawText(sText,&rtBorder,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	pDC->SelectObject(oldpen);
}



void CCellWnd::DrawMeasure(CRect rtBorder,CDC *pDC)
{
	int nMeasureType;

	nMeasureType = m_pDicomViewer->GetMeasureType();

	switch(nMeasureType)
	{
	case 0:	 // Draw Line
	case 99: // Calibrate
		DrawLineMeasure(rtBorder,pDC);
		break;
	case 1:	// Draw Rectangle
		DrawRectangleMeasure(rtBorder,pDC);
		break;
	case 2:	// Draw Ellipse
		DrawEllipseMeasure(rtBorder,pDC);
		break;
	case 3:	// Draw Polygon
		DrawPolygonMeasure(rtBorder,pDC);
		break;
	case 4:	// Draw Angle
		DrawAngleMeasure(rtBorder,pDC);
		break;
	case 5:
		DrawCobbMeasure(rtBorder,pDC);
		break;
	}
}


void CCellWnd::DrawMagnifier(CDC *pDC,CRect rtBorder,CPoint pt,double fZoomFactor)
{
	CPen pen,*oldpen;
	int nLeft,nTop,nImageWidth,nImageHeight,nX0,nY0,nX,nY,nIdx;
	int nWidth,nHeight;
	int nMagnifierSize = 200;
	double fPixelAspectRatio;
	CRect rtImageRect,rtDest;
	CPoint point;

	if (m_pDicomObject == NULL) return;

	if (m_lpBMIH == NULL) return;

	point = pt;

	nImageWidth = m_pDicomObject->GetImageWidth();
	nImageHeight = m_pDicomObject->GetImageHeight();
	
	fPixelAspectRatio = m_pDicomObject->GetPixelAspectRatio();

	rtImageRect = GetImageViewRect();


	if (!rtImageRect.PtInRect(pt)) return;


	rtDest.left = point.x - m_nMagnifierSize;
	rtDest.top  = point.y - m_nMagnifierSize;
	rtDest.right = rtDest.left + m_nMagnifierSize * 2;
	rtDest.bottom = rtDest.top + m_nMagnifierSize * 2;


	nX0 = (int)((point.x - rtImageRect.left) / GetRealZoomFactor()) ;
	nY0 = (int)((point.y - rtImageRect.top) / GetRealZoomFactor());
		
	nIdx = (m_nRotateAngle % 360) / 90;

	switch(nIdx)
	{
	case 0:
		nX = nX0;
		nY = nY0;
		break;
	case 1:
		nX = nY0;
		nY = nImageHeight - nX0;
		break;
	case 2:
		nX = nImageWidth - nX0;
		nY = nImageHeight - nY0;
		break;
	case 3:
		nX = nImageWidth - nY0;
		nY = nX0;
		break;
	}


	///////////////////////////////////////////////////
	if (nImageWidth > nImageHeight)
	{
		nX = (int) (nX / fPixelAspectRatio);
	}
	else
	{
		nY = (int) (nY / fPixelAspectRatio);
	}

	nY = nImageHeight - nY;


	nWidth = rtDest.Width() /  (fZoomFactor);
	nHeight = rtDest.Height() / (fZoomFactor);


	nLeft = nX - nWidth / 2;
	nTop  = nY - nHeight / 2;

	SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE);

	if (m_nRotateAngle > 0)
	{		
		cdxCRot90DC rotDC(pDC, rtDest, (360 - m_nRotateAngle) % 360 );

		if (rotDC.IsCreated())
		{
			CRect rectRotClient = rotDC.GetRotRect();

			SetStretchBltMode(rotDC.GetSafeHdc(), HALFTONE);
			
			StretchDIBits(rotDC.GetSafeHdc(), rectRotClient.left,rectRotClient.top, rectRotClient.Width(), rectRotClient.Height(),
						   nLeft, nTop, nWidth, nHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
                               DIB_RGB_COLORS, SRCCOPY);
			

			rotDC.Finish();
		}
		
	}
	else
	{
		
		StretchDIBits(pDC->GetSafeHdc(), rtDest.left, rtDest.top,  rtDest.Width(),rtDest.Height(),
					   nLeft, nTop, nWidth, nHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
							    DIB_RGB_COLORS, SRCCOPY);
		
	}


	pen.CreatePen(PS_SOLID,1,RGB(0,0,0));

	oldpen = pDC->SelectObject(&pen);

	pDC->MoveTo(rtDest.left,rtDest.top);
	pDC->LineTo(rtDest.right,rtDest.top);
	pDC->LineTo(rtDest.right,rtDest.bottom);
	pDC->LineTo(rtDest.left,rtDest.bottom);
	pDC->LineTo(rtDest.left,rtDest.top);

	pDC->SelectObject(&oldpen);

	DeleteObject(pen);

}



void CCellWnd::DrawObject(CDC *pDC,BOOL bSelected)
{
	CPen pen,*oldpen;
	CRgn rgn;
	RECT rtText;
	int nTextHeight;
	int nMeasureType;
	COLORREF oldColor;
	CString sText;
	double fPixelAspectRatio;	

	rgn.CreateRectRgn(m_rtBorder.left, m_rtBorder.top,m_rtBorder.right,m_rtBorder.bottom);
	
	if (bSelected)
	{
		if (!m_bPrintPreviewMode)
		{
			pen.CreatePen(PS_DOT,1,RGB(0,255,150));
		}
		else
		{
			pen.CreatePen(PS_DOT,1,RGB(255,255,255));
		}
	}
	else
	{
		if (!m_bPrintPreviewMode)
		{
			pen.CreatePen(PS_SOLID,1,RGB(200,150,0));
		}
		else
		{
			pen.CreatePen(PS_SOLID,1,RGB(32,32,32));
		}
	}

	oldpen = pDC->SelectObject(&pen);

	pDC->MoveTo(m_rtBorder.left - 1,m_rtBorder.top - 1);
	pDC->LineTo(m_rtBorder.right + 1,m_rtBorder.top - 1);
	pDC->LineTo(m_rtBorder.right + 1,m_rtBorder.bottom + 1);
	pDC->LineTo(m_rtBorder.left - 1,m_rtBorder.bottom + 1);
	pDC->LineTo(m_rtBorder.left - 1,m_rtBorder.top - 1);

	pDC->SelectClipRgn(&rgn);

	if (m_pDicomObject != NULL)
	{

		fPixelAspectRatio = m_pDicomObject->GetPixelAspectRatio();

		if (m_lpBMIH != NULL)
		{
			CRect rc;

			rc.left = m_rtView.left + m_nShiftX;
			rc.top = m_rtView.top + m_nShiftY;
			rc.right = rc.left + m_rtView.Width();
			rc.bottom = rc.top + m_rtView.Height();

			SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE);

			if (m_nRotateAngle > 0)
			{

				cdxCRot90DC rotDC(pDC, rc, (360 - m_nRotateAngle) % 360 );

				// check whether there is a non-empty visible rectangle
				if (rotDC.IsCreated())
				{
					CRect rectRotClient = rotDC.GetRotRect();
					
					//SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE);

					SetStretchBltMode(rotDC.GetSafeHdc(), HALFTONE);
				
					StretchDIBits(rotDC.GetSafeHdc(), rectRotClient.left,rectRotClient.top, rectRotClient.Width(), rectRotClient.Height(),
						   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
                                DIB_RGB_COLORS, SRCCOPY);

					rotDC.Finish();
				}
				else
				{
					StretchDIBits(pDC->GetSafeHdc(), m_rtView.left + m_nShiftX, m_rtView.top + m_nShiftY, m_rtView.Width(), m_rtView.Height(),
						   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
								    DIB_RGB_COLORS, SRCCOPY);
				}
			}
			else
			{
				StretchDIBits(pDC->GetSafeHdc(), m_rtView.left + m_nShiftX, m_rtView.top + m_nShiftY, m_rtView.Width(), m_rtView.Height(),
					   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
							    DIB_RGB_COLORS, SRCCOPY);
			}

		}
	
		if (m_bDrawOverlayInfo)
		{
			DrawOverlayInfo(m_rtBorder,pDC);
		}

		if (!m_bRegistered)
		{
			pDC->MoveTo(m_rtBorder.left,m_rtBorder.top);
			pDC->LineTo(m_rtBorder.right,m_rtBorder.bottom);
			pDC->MoveTo(m_rtBorder.right,m_rtBorder.top);
			pDC->LineTo(m_rtBorder.left,m_rtBorder.bottom);

			pDC->SetBkMode(TRANSPARENT);

			oldColor = pDC->GetTextColor();

			pDC->SetTextColor(RGB(255,0,0));

			pDC->DrawText(_T("H"),&rtText,DT_CALCRECT);

			nTextHeight = (rtText.bottom - rtText.top + 1) + 4;

			/////////////////////////////////////////////
			//Right_Down Corner
			/////////////////////////////////////////////


			rtText.bottom = m_rtBorder.bottom;
			rtText.top = m_rtBorder.bottom - nTextHeight;
			rtText.left = m_rtBorder.left;
			rtText.right = m_rtBorder.right;
			
			pDC->DrawText(m_sUnregistered,&rtText,DT_RIGHT | DT_VCENTER);

			pDC->SetTextColor(oldColor);

		}


		if (m_nSmoothFilterNo != 0 || m_nSharpFilterNo != 0)
		{

			pDC->SetBkMode(TRANSPARENT);

			oldColor = pDC->GetTextColor();

			pDC->SetTextColor(RGB(255,0,0));

			pDC->DrawText(_T("H"),&rtText,DT_CALCRECT);

			nTextHeight = (rtText.bottom - rtText.top + 1) + 4;

			
			rtText.bottom = m_rtBorder.bottom;
			rtText.top = m_rtBorder.bottom - nTextHeight * 2;
			rtText.left = m_rtBorder.left;
			rtText.right = m_rtBorder.right;

			oldColor = pDC->GetTextColor();

			pDC->SetTextColor(RGB(255,0,0));

			sText.Format(_T("%s"),_T("¬Àæµ“—”√"));
			pDC->DrawText(sText,&rtText,DT_RIGHT | DT_VCENTER);
		
			pDC->SetTextColor(oldColor);
		}

		if (m_bDrawMeasure)
		{
			DrawMeasure(m_rtBorder,pDC);
		}

		if (m_bDrawCTValue)
		{
			nMeasureType = m_pDicomViewer->GetMeasureType();

			if (m_ptMeasurePoint1 == m_ptMeasurePoint2)
			{
				DrawCTValue(m_rtBorder,pDC);
			}
			else
			{
				if (nMeasureType == 1)
				{
					DrawRectangleMeasure(m_rtBorder,pDC);
				}
				else if (nMeasureType == 2)
				{
					DrawEllipseMeasure(m_rtBorder,pDC);
				}
				else
				{
					DrawCTValue(m_rtBorder,pDC);
				}
			}
		}

//		if (m_bDrawProcessView)
//		{
//			DrawProcessView(pDC);
//		}
		
		if (m_bCTMR)
		{
			DrawLocalizer(m_rtBorder,pDC);
		}
	}

	pDC->SelectObject(oldpen);
	pDC->SelectClipRgn(NULL);

	DeleteObject(pen);
	DeleteObject(rgn);

}


void CCellWnd::DrawObjectForExport(CDC *pDC,int nWidth,int nHeight)
{
	CRect rc;
	
	if (m_pDicomObject == NULL) return;

	if (m_lpBMIH == NULL) return;

	rc.left = 0;
	rc.top = 0;
	rc.right = nWidth;
	rc.bottom = nHeight;

	if (m_nRotateAngle > 0)
	{
		cdxCRot90DC rotDC(pDC, rc, (360 - m_nRotateAngle) % 360 );

				// check whether there is a non-empty visible rectangle
		if (rotDC.IsCreated())
		{
			CRect rectRotClient = rotDC.GetRotRect();

			SetStretchBltMode(rotDC.GetSafeHdc(), HALFTONE); //COLORONCOLOR);
				
			StretchDIBits(rotDC.GetSafeHdc(), rectRotClient.left,rectRotClient.top, rectRotClient.Width(), rectRotClient.Height(),
						   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
                                DIB_RGB_COLORS, SRCCOPY);

			rotDC.Finish();
		}
		else
		{
			SetStretchBltMode(pDC->GetSafeHdc(),HALFTONE); //COLORONCOLOR);
			
			StretchDIBits(pDC->GetSafeHdc(), rc.left, rc.top, rc.Width(), rc.Height(),
						   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
								    DIB_RGB_COLORS, SRCCOPY);
		}
	}
	else
	{
		SetStretchBltMode(pDC->GetSafeHdc(), HALFTONE); //COLORONCOLOR);
		
		StretchDIBits(pDC->GetSafeHdc(), rc.left, rc.top, rc.Width(), rc.Height(),
					   0, 0, m_nImageWidth, m_nImageHeight,
							m_lpBits, (LPBITMAPINFO) m_lpBMIH, 
							    DIB_RGB_COLORS, SRCCOPY);
	}

}



void CCellWnd::Reset()
{	
	m_bDrawProcessView = FALSE;

	m_fZoomFactor = 1.0;

	m_nShiftX = 0;
	m_nShiftY = 0;

	m_bHFlip = FALSE;
	m_nRotateAngle = 0;

	m_nSmoothFilterNo = 0;
	m_nSharpFilterNo = 0;

	m_bReversed = FALSE;
	
	m_bUseExtLut = FALSE;

}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDicomViewer

CDicomViewer::CDicomViewer()
{
	int ni;

	m_nRows = 1;
	m_nCols = 1;

	m_nSeriesRows = 3;
	m_nSeriesCols = 3;

	m_bLBDown = FALSE;
	m_bRBDown = FALSE;

	m_bMeasureAngleOk = TRUE;

	m_bOneCellWndMode = FALSE;
	m_bSeriesMode = TRUE;

	m_nLBFuncNo = 1;
	m_nRBFuncNo = 1;

	m_fMagnifierZoomFactor = 2.0;

	m_pSeriesObjectList = NULL;

	m_pSeriesObject = NULL;
	m_pDicomObjectList = NULL;

	m_pCellWndList = new CObList;
	m_pCellWndList->RemoveAll();

	m_pCellWndSelected = NULL;

	m_nButtonDelayFactor = 150;

	m_nCurrentIndex = 0;
	m_nMaxIndex = 0;

	m_bDrawOverlay = FALSE;

	m_hLBFunc1Cursor = NULL;
	m_hLBFunc2Cursor = NULL;
	m_hMeasureCursor = NULL;
	m_hRBFunc1Cursor = NULL;
	m_hRBFunc2Cursor = NULL;

	for (ni = 0; ni < 256; ni ++)
	{
		m_cExtLut[ni]		= ni;
		m_cExtLut[256 + ni] = ni;
		m_cExtLut[512 + ni] = ni;
	}

	m_bUseExtLut = FALSE;

	SetCineSpeed(15);

	m_nCineEscapedTimes = 0;
	m_nBrowseEscapedTimes = 0;

	m_bAutoBrowseMode = FALSE;

	m_nMeasureType = 0;

	m_nTextColor = RGB(0,0,0);
	m_nBkColor = RGB(255,255,255);
	m_nBkMode = OPAQUE;

	m_bPopupMenuWorking = FALSE;

	m_bShowScrollBar = TRUE;

	m_bPrintPreviewMode = FALSE;

	m_bMagnifyActive = FALSE;

	m_bExchangePosMode = FALSE;
	m_pExchangePosWnd1 = NULL;
	m_pExchangePosWnd2 = NULL;


}


CDicomViewer::~CDicomViewer()
{
	CCellWnd *pCellWnd;

	while (m_pCellWndList->GetHeadPosition() != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->RemoveHead();
		delete pCellWnd;
	}

	delete m_pCellWndList;

	if (m_hLBFunc1Cursor)
		DestroyCursor(m_hLBFunc1Cursor);

	if (m_hLBFunc2Cursor)
		DestroyCursor(m_hLBFunc2Cursor);

	if (m_hMeasureCursor)
		DestroyCursor(m_hMeasureCursor);

	if (m_hRBFunc1Cursor)
		DestroyCursor(m_hRBFunc1Cursor);

	if (m_hRBFunc2Cursor)
		DestroyCursor(m_hRBFunc2Cursor);

}


BEGIN_MESSAGE_MAP(CDicomViewer, CWnd)
	//{{AFX_MSG_MAP(CDicomViewer)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_WM_KEYUP()
END_MESSAGE_MAP()

int CDicomViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


BOOL CDicomViewer::ExchangePosAvailable()
{
	BOOL bEnable;

	bEnable = m_pCellWndList->GetCount() > 1;

	if (bEnable)
	{
		if (m_bSeriesMode)
		{
			if (m_pSeriesObjectList != NULL)
			{
				bEnable = (m_pSeriesObjectList->GetCount() > 1);
			}
			else
			{
				bEnable = FALSE;
			}
		}
		else
		{
			if (m_pDicomObjectList != NULL)
			{
				bEnable = (m_pDicomObjectList->GetCount() > 1);
			}
			else
			{
				bEnable = FALSE;
			}
		}
	}

	return bEnable;	
}



void CDicomViewer::SetMatrix(int nRows,int nCols,BOOL bSeriesMode)
{
	if (bSeriesMode)
	{
		m_nSeriesRows = nRows;
		m_nSeriesCols = nCols;
	}
	else
	{
		m_nRows = nRows;
		m_nCols = nCols;
	}

	SetExchangePosMode(FALSE);

}


void CDicomViewer::AdjustMatrix(int nRows,int nCols)
{
	CSeriesObject *pSeriesObj,*pCurSeriesObj;
	CDicomObject *pDicomObj,*pCurDicomObj;
	CCellWnd *pCellWnd;
	int ni,nIndex;
	POSITION pos;

	if (m_bSeriesMode)
	{

		if (m_pCellWndSelected != NULL)
		{
			pCurSeriesObj = (CSeriesObject *) m_pCellWndSelected->GetSeriesObject();
		}
		else 
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();

			pCurSeriesObj = pCellWnd->GetSeriesObject();
		}

		if (pCurSeriesObj == NULL)
		{
			nIndex = 0;
		}
		else
		{
			nIndex = -1;
			ni = 0;
			pos = m_pSeriesObjectList->GetHeadPosition();
			while (pos != NULL && nIndex == -1)
			{
				pSeriesObj = (CSeriesObject *) m_pSeriesObjectList->GetNext(pos);

				if (pSeriesObj == pCurSeriesObj)
				{
					nIndex = ni;
				}

				ni ++;
			}

			if (nIndex == -1)
			{
				nIndex = 0;
			}
			else
			{
				nIndex = nIndex - nRows * nCols + 1;

				if (nIndex < 0) nIndex = 0;
			}

			m_nCurrentIndex = nIndex;
		}


		m_nSeriesRows = nRows;
		m_nSeriesCols = nCols;


		AdjustVScrollBar();

		CreateCellWnds(m_bSeriesMode);

		BindSeriesObject2CellWnd(m_nCurrentIndex,pCurSeriesObj);

		if (m_pCellWndSelected == NULL)
		{
			pos = m_pCellWndList->GetHeadPosition();
			while (pos != NULL && m_pCellWndSelected == NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				if (pCellWnd->GetSeriesObject() != NULL)
				{
					m_pCellWndSelected = pCellWnd;
				}
			}
		}

		SetVScrollBarPos(m_nCurrentIndex);
	}
	else
	{
		if (m_pCellWndSelected != NULL)
		{
			pCurDicomObj = (CDicomObject *) m_pCellWndSelected->GetDicomObject();
		}
		else 
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();

			pCurDicomObj = pCellWnd->GetDicomObject();
		}

		if (pCurDicomObj == NULL)
		{
			nIndex = 0;
		}
		else
		{
			nIndex = -1;
			ni = 0;
			pos = m_pDicomObjectList->GetHeadPosition();
			while (pos != NULL && nIndex == -1)
			{
				pDicomObj = (CDicomObject *) m_pDicomObjectList->GetNext(pos);

				if (pDicomObj == pCurDicomObj)
				{
					nIndex = ni;
				}

				ni ++;
			}

			if (nIndex == -1)
			{
				nIndex = 0;
			}
			else
			{
				nIndex = nIndex - nRows * nCols + 1;

				if (nIndex < 0) nIndex = 0;
			}

			m_nCurrentIndex = nIndex;
		}


		m_nRows = nRows;
		m_nCols = nCols;


		AdjustVScrollBar();

		CreateCellWnds(m_bSeriesMode);

		BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject,pCurDicomObj);

		if (m_pCellWndSelected == NULL)
		{
			pos = m_pCellWndList->GetHeadPosition();
			while (pos != NULL && m_pCellWndSelected == NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				if (pCellWnd->GetDicomObject() != NULL)
				{
					m_pCellWndSelected = pCellWnd;
				}
			}
		}

		SetVScrollBarPos(m_nCurrentIndex);
	}

	SetExchangePosMode(FALSE);
}



void CDicomViewer::SetExchangePosMode(BOOL bValue)
{
	m_bExchangePosMode = bValue;

	m_pExchangePosWnd1 = NULL;
	m_pExchangePosWnd2 = NULL;

}


int CDicomViewer::GetSeriesObjectCount()
{
	int nCount = 0;

	if (m_pSeriesObjectList != NULL)
	{
		nCount = m_pSeriesObjectList->GetCount();
	}

	return nCount;
}


int CDicomViewer::GetDicomObjectCount()
{
	int nCount = 0;

	if (m_pDicomObjectList != NULL)
	{
		nCount = m_pDicomObjectList->GetCount();
	}

	return nCount;
}

void CDicomViewer::DisplaySeriesObjectList(CSeriesObject *pSeriesObject)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	CSeriesObject *pCurObj0,*pCurObj,*pObj;
	int ni,nCount,nIdx;
	unsigned int nID;
	NMHDR nmhdr;

	if (m_pSeriesObjectList == NULL) return;

	if (!m_bSeriesMode)
	{
		m_bSeriesMode = TRUE;

		if (m_bOneCellWndMode)
		{
			m_bOneCellWndMode = FALSE;
			m_nRows = m_nOldRows;
			m_nCols = m_nOldCols;
		}
	}

	pCurObj0 = NULL;

	pCellWnd = m_pCellWndSelected;

	if (pCellWnd != NULL)
	{
		pCurObj0 = pCellWnd->GetSeriesObject();

		if (pCurObj0 != NULL && m_pSeriesObjectList->GetCount() > 0)
		{
			if (m_pSeriesObjectList->Find(pCurObj0) == NULL)
			{
				pCurObj0 = NULL;
			}
		}
		else
		{
			pCurObj0 = NULL;
		}
	}
	
	if (pSeriesObject != NULL)
	{
		pCurObj = pSeriesObject;
	}
	else if (pCurObj0 != NULL)
	{
		pCurObj = pCurObj0;
	}
	else 
	{
		if (m_pSeriesObjectList->GetCount() > 0)
		{
			pCurObj = (CSeriesObject *) m_pSeriesObjectList->GetTail();
		}
		else
		{
			pCurObj = NULL;
		}
	}
	
	m_nCurrentIndex = 0;
	m_pCellWndSelected = 0;

	CreateCellWnds(TRUE);
	AdjustVScrollBar();

	ni = 0;
	nIdx = -1;
	pos = m_pSeriesObjectList->GetHeadPosition();
	while (pos != NULL && nIdx ==  -1)
	{
		pObj = (CSeriesObject *) m_pSeriesObjectList->GetNext(pos);

		if (pObj == pCurObj)
		{
			nIdx = ni;
		}

		ni ++;
	}

	if (nIdx != -1)
	{
		nIdx = nIdx - m_pCellWndList->GetCount() + 1;

		if (nIdx < 0) nIdx = 0;

		m_nCurrentIndex = nIdx;
	}

	BindSeriesObject2CellWnd(m_nCurrentIndex,pCurObj);

	SetVScrollBarPos(m_nCurrentIndex);


	nID = GetDlgCtrlID();

	nmhdr.code = NM_DICOMVIEWERMODECHANGED;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = nID;
	
	GetParent()->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);


	Invalidate();
}


void CDicomViewer::DisplaySeriesObject(CSeriesObject *pSeriesObject)
{
	POINT pt;
	POSITION pos;
	CCellWnd *pCellWnd;
	CDicomObject *pCurObj;
	unsigned int nID;
	NMHDR nmhdr;

	if (pSeriesObject == NULL)
		return;

	pt.x = 10;
	pt.y = 10;

	pCurObj = NULL;

	pCellWnd = m_pCellWndSelected;
	if (pCellWnd != NULL) {

		pt = pCellWnd->GetBorder().CenterPoint();
		pCurObj = pCellWnd->GetDicomObject();

		if (pSeriesObject->m_DicomObjectList.Find(pCurObj) == 0)
			pCurObj = NULL;
	}

	m_bSeriesMode = FALSE;
	m_pSeriesObject = pSeriesObject;
	m_pDicomObjectList = &(pSeriesObject->m_DicomObjectList);

	CreateCellWnds(FALSE);

	AdjustVScrollBar();

	BindDicomObject2CellWnd(0, pSeriesObject,pCurObj);

	nID = GetDlgCtrlID();

	nmhdr.code = NM_DICOMVIEWERMODECHANGED;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = nID;
	
	GetParent()->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
	
	SelectCellWnd(pt);

	Invalidate();
}



void CDicomViewer::SetShowScrollBar(BOOL bShow)
{
	m_bShowScrollBar = bShow;

	AdjustVScrollBar();
}


void CDicomViewer::CreateCellWnds(BOOL bSeriesMode)
{
	int nRow,nCol,nWidth0,nWidth1,nHeight0,nHeight1,nRows,nCols;
	CCellWnd *pCellWnd;
	double fzoom1,fzoom2,fzoom;
	CRect rect;
	POSITION pos;
	CDicomObject *pDicomObject = NULL;
	CSeriesObject *pSeriesObject = NULL;

	nWidth0 = 0;
	nHeight0 = 0;

	m_pCellWndSelected = NULL;
	
	if (m_pCellWndList->GetCount() > 0)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
		rect = pCellWnd->GetBorder();
		nWidth0 = rect.Width();
		nHeight0 = rect.Height();
	}

	if (bSeriesMode)
	{
		nRows = m_nSeriesRows;
		nCols = m_nSeriesCols;
	}
	else
	{
		nRows = m_nRows;
		nCols = m_nCols;
	}

	while (m_pCellWndList->GetHeadPosition() != 0)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->RemoveHead();
		delete pCellWnd;
	}

	for (nRow = 0; nRow < nRows; nRow ++)
	{
		for (nCol = 0; nCol < nCols; nCol ++)
		{
			pCellWnd = new CCellWnd(this);	

			pCellWnd->SetRegister(m_bRegistered);
			pCellWnd->SetSeriesMode(bSeriesMode);
			
			pCellWnd->SetZoomFactor(0);
			pCellWnd->SetDrawOverlay(m_bDrawOverlay);
			pCellWnd->SetFlip(FALSE,FALSE);
			pCellWnd->SetRotateAngle(0);

			//pCellWnd->SetExtLut(m_bUseExtLut,m_cExtLut);

			pCellWnd->SetPrintPreviewMode(m_bPrintPreviewMode);

			m_pCellWndList->AddTail(pCellWnd);
		}
	}

	ResizeCellWnd();

	/*
	if (m_fZoomFactor != 0.0 && nWidth0 != 0 && nHeight0 != 0)
	{

		pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();

		if (pCellWnd  != NULL)
		{
			rect = pCellWnd->GetBorder();
		}
		else
		{
			rect = CRect(0,0,0,0);
		}

		nWidth1 = rect.Width();
		nHeight1 = rect.Height();
		
		fzoom1 = (nWidth1 * m_fZoomFactor) / nWidth0;
		fzoom2 = (nHeight1 * m_fZoomFactor) / nHeight0;

		fzoom = fzoom1;
		if (fzoom > fzoom1) fzoom = fzoom1;

		if (fzoom != m_fZoomFactor)
		{
			m_fZoomFactor = fzoom;

			pos = m_pCellWndList->GetHeadPosition();
			while (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);
				pCellWnd->SetZoomFactor(fzoom);
			}
		}
	}

	*/

	return ;
}

/////////////////////////////////////////////////////////////////////////////

void CDicomViewer::ResizeCellWnd()
{
	CRect rect;
	int nIdx,nRow,nCol,nCellWidth,nCellHeight,nFontPoints;
	int nRows,nCols;
	CCellWnd *pCellWnd;
	POSITION pos;
	float fw,fh,fr;

	GetClientRect(rect);

	if (m_bSeriesMode)
	{
		nRows = m_nSeriesRows;
		nCols = m_nSeriesCols;
	}
	else
	{
		nRows = m_nRows;
		nCols = m_nCols;
	}

	nCellWidth = (rect.Width() - 3 * (nCols + 1)) / nCols;
	nCellHeight = (rect.Height() - 3 * (nRows + 1)) / nRows;

	fw = (float) (nCellWidth / 30.0);
	fh = (float) (nCellHeight / 30.0);

	fr = fw;
	if (fr > fh) fr = fh;

	nFontPoints = (int ) fr;

	if (nFontPoints < 4) nFontPoints = 4;
	if (nFontPoints > 12) nFontPoints = 12;

	nFontPoints = nFontPoints * 10;

	m_font.DeleteObject();

	m_font.CreatePointFont(nFontPoints,_T("Arial Narrow"));
	

	for (nRow = 0; nRow < nRows; nRow ++)
	{
		for (nCol = 0; nCol < nCols; nCol ++)
		{
			nIdx = nRow * nCols + nCol;

			rect.left = 3 * (nCol + 1) + nCol * nCellWidth;
			rect.right = rect.left + nCellWidth - 2;
			rect.top = 3 * (nRow + 1) + nRow * nCellHeight;
			rect.bottom = rect.top + nCellHeight - 2;

			rect.NormalizeRect();
			
			pos = m_pCellWndList->FindIndex(nIdx);

			if (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetAt(pos);
				pCellWnd->SetBorder(rect);
				pCellWnd->SetFont(&m_font);
			}
		}
	}	
}


void CDicomViewer::SetCurrentIndex(int nIdx)
{
	int nCount;

	if (m_bSeriesMode)
	{
		nCount = m_pSeriesObjectList->GetCount();
	}
	else
	{
		nCount = m_pDicomObjectList->GetCount();
	}

	if (nIdx >= 0 && nIdx < nCount)
	{
		m_nCurrentIndex = nIdx;
	}
}


int CDicomViewer::GetCurrentIndex()
{
	return m_nCurrentIndex;
}


void CDicomViewer::SetVScrollBarPos(int nPos)
{
	SCROLLINFO	si ;
	int nSize1,nSize2,nRows,nCols;

	GetScrollInfo(SB_VERT, &si) ;
	
	if (nPos >= si.nMin  && nPos <= si.nMax)
	{
		si.nPos = nPos;
		
		SetScrollInfo(SB_VERT,&si);
	}
}


void CDicomViewer::AdjustVScrollBar()
{
	SCROLLINFO	si ;
	int nSize1,nSize2,nRows,nCols;

	if (m_bSeriesMode)
	{
		nRows = m_nSeriesRows;
		nCols = m_nSeriesCols;
	}
	else
	{
		nRows = m_nRows;
		nCols = m_nCols;
	}

	nSize1 = nRows * nCols;


	if (m_bSeriesMode)
	{
		if (m_pSeriesObjectList != NULL)
		{
			nSize2 = m_pSeriesObjectList->GetCount();
		}
		else
		{
			nSize2 = 0;
		}
	}
	else
	{
		if (m_pDicomObjectList != NULL)
		{
			nSize2 = m_pDicomObjectList->GetCount();
		}
		else
		{
			nSize2 = 0;
		}
	}

	if (nSize2 > nSize1 && !m_bPrintPreviewMode)
	{
		m_nMaxIndex = nSize2 - nSize1;
	
		si.cbSize = sizeof(SCROLLINFO) ;
		si.fMask = SIF_PAGE | SIF_RANGE ;
		si.nPage = 1;
		si.nMax = m_nMaxIndex ;
		si.nMin = 0 ;
		si.nPos = 0;

		SetScrollInfo(SB_VERT, &si) ;		

		if (m_bShowScrollBar)
		{
			ShowScrollBar(SB_VERT,TRUE);
		}
		else
		{
			ShowScrollBar(SB_VERT,FALSE);
		}
	}
	else
	{
		m_nMaxIndex = 0;

		si.cbSize = sizeof(SCROLLINFO) ;
		si.fMask = SIF_PAGE | SIF_RANGE ;
		si.nPage = 1;
		si.nMax = 0 ;
		si.nMin = 0 ;
		si.nPos = 0;

		SetScrollInfo(SB_VERT, &si) ;		

		ShowScrollBar(SB_VERT,FALSE);
	}
}



void CDicomViewer::BindSeriesObject2CellWnd(int nIndex,CSeriesObject *pSeriesObject)
{
	POSITION pos,pos1;
	CCellWnd *pCellWnd;
	CSeriesObject *pObj,*pCurObj;
	int ni,nCount,nPageIndex,nIdx,nMaxIndex,nCellWndCount,nSeriesObjCount;

	if (m_pSeriesObjectList == NULL)
	{
		return;
	}

	nCellWndCount = m_pCellWndList->GetCount();
	nSeriesObjCount = m_pSeriesObjectList->GetCount();

	nMaxIndex = nSeriesObjCount - nCellWndCount;

	if (nMaxIndex < 0) nMaxIndex = 0;

	if (nIndex < 0) 
		m_nCurrentIndex = 0;
	else if (nIndex > nMaxIndex)
		m_nCurrentIndex = nMaxIndex;
	else
		m_nCurrentIndex = nIndex;


	if (pSeriesObject == NULL)
	{
		if (m_pCellWndSelected != NULL)
		{
			pCurObj = m_pCellWndSelected->GetSeriesObject();
		}
		else
		{
			pCurObj = NULL;
		}
	}
	else
	{
		pCurObj = pSeriesObject;
	}

	m_pCellWndSelected = NULL;

	ni = 0;
	pos = m_pCellWndList->GetHeadPosition();
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		pObj = NULL;
		pos1 = m_pSeriesObjectList->FindIndex(m_nCurrentIndex + ni);
		if (pos1 != NULL)
		{
			pObj = (CSeriesObject *) m_pSeriesObjectList->GetAt(pos1);
		}

		pCellWnd->SetSeriesMode(TRUE);
		pCellWnd->SetSeriesObject(pObj);

		if (pObj == pSeriesObject && pObj != NULL)
		{
			m_pCellWndSelected = pCellWnd;
		}

		ni ++;

	}


	SetVScrollBarPos(m_nCurrentIndex);	
}

void CDicomViewer::BindDicomObject2CellWnd(int nIndex,CSeriesObject *pSeriesObject,CDicomObject *pDicomObject)
{
	POSITION pos,pos1;
	CCellWnd *pCellWnd;
	CDicomObject *pObj,*pCurObj;
	int ni,nCount,nPageIndex,nIdx,nMaxIndex,nCellWndCount,nDicomObjCount;

	if (m_pSeriesObject == NULL)
	{
		return;
	}

	nCellWndCount = m_pCellWndList->GetCount();
	nDicomObjCount = pSeriesObject->m_DicomObjectList.GetCount();

	nMaxIndex = nDicomObjCount - nCellWndCount;

	if (nMaxIndex < 0) nMaxIndex = 0;

	if (nIndex < 0) 
		m_nCurrentIndex = 0;
	else if (nIndex > nMaxIndex)
		m_nCurrentIndex = nMaxIndex;
	else
		m_nCurrentIndex = nIndex;


	if (pDicomObject == NULL)
	{
		if (m_pCellWndSelected != NULL)
		{
			pCurObj = m_pCellWndSelected->GetDicomObject();
		}
		else
		{
			pCurObj = NULL;
		}
	}
	else
	{
		pCurObj = pDicomObject;
	}

	m_pCellWndSelected = NULL;

	ni = 0;
	pos = m_pCellWndList->GetHeadPosition();
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		pObj = NULL;
		pos1 = pSeriesObject->m_DicomObjectList.FindIndex(m_nCurrentIndex + ni);
		if (pos1 != NULL)
		{
			pObj = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetAt(pos1);
		}

		pCellWnd->SetSeriesMode(FALSE);
		pCellWnd->SetDicomObject(pSeriesObject,pObj);
	
		if (pObj == pDicomObject && pObj != NULL)
		{
			m_pCellWndSelected = pCellWnd;
		}

		ni ++;

	}


	SetVScrollBarPos(m_nCurrentIndex);	

}


void CDicomViewer::SetDrawOverlay(BOOL bValue)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	
	m_bDrawOverlay = bValue;

	pos = m_pCellWndList->GetHeadPosition();	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);
		pCellWnd->SetDrawOverlay(m_bDrawOverlay);
	}
}


void CDicomViewer::SetReversed()
{
	POSITION pos;
	CCellWnd *pCellWnd = NULL;
	CDicomObject *pDicomObject;
	BOOL bReversed,bCtrlDown = FALSE,bScreenNeeded = FALSE;
	CString sModality;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	
	
	bScreenNeeded = FALSE;

	if (m_bSeriesMode == FALSE)
	{
		pCellWnd = m_pCellWndSelected;

		if (pCellWnd == NULL)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
		}

		if (pCellWnd != NULL)
		{
			pDicomObject = pCellWnd->GetDicomObject();

			if (pDicomObject != NULL)
			{
				sModality = pDicomObject->GetModality();

				if (sModality == "CT" || sModality == "MR")
				{
					bScreenNeeded = TRUE;
				}
			}
		}
	}
		
	pos = m_pCellWndList->GetHeadPosition();	
	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if (((pCellWnd == m_pCellWndSelected) || bCtrlDown || bScreenNeeded ) && pCellWnd != NULL)
		{
			pCellWnd->GetReversed(bReversed);
			
			bReversed = !bReversed;

			pCellWnd->SetReversed(bReversed);

			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}

void CDicomViewer::SetWinLevel(double fcen,double fwid)
{
	CSeriesObject *pSeriesObject;
	CDicomObject *pDicomObject;
	CCellWnd *pCellWnd;
	CString sModality("");
	POSITION pos1,pos2;
	BOOL bCtrlDown,bSeriesNeeded = FALSE,bDone = FALSE;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	

	bDone = FALSE;
	bSeriesNeeded = FALSE;

	if (m_bSeriesMode == FALSE)
	{
		pCellWnd = m_pCellWndSelected;
	
		if (pCellWnd == NULL)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();		
		}

		if (pCellWnd != NULL)
		{
			pDicomObject = pCellWnd->GetDicomObject();

			if (pDicomObject != NULL)
			{
				sModality = pDicomObject->GetModality();
			
				if (sModality == "CT" || sModality == "MR")
				{
					bSeriesNeeded = TRUE;
				}
			}
		}
	}
		
	pos1 = m_pCellWndList->GetHeadPosition();

	while (pos1 != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos1);

		if (bDone)
		{
			if (pCellWnd != NULL) pCellWnd->MakeBitmap(FALSE);
		}
		else
		{
			if ((pCellWnd == m_pCellWndSelected || bCtrlDown || bSeriesNeeded) && pCellWnd != NULL)
			{
				if (m_bSeriesMode || bSeriesNeeded)
				{
					pSeriesObject = pCellWnd->GetSeriesObject();

					if (pSeriesObject != NULL)
					{
						pos2 = pSeriesObject->m_DicomObjectList.GetHeadPosition();

						while (pos2 != NULL)
						{
							pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos2);

							if (pDicomObject != NULL)
							{
								pDicomObject->SetWinLevel(fcen,fwid);
							}
						}

						if (bSeriesNeeded) bDone = TRUE;
					}
				}
				else
				{
				
					pDicomObject = (CDicomObject *) pCellWnd->GetDicomObject();

					if (pDicomObject != NULL)
					{
						pDicomObject->SetWinLevel(fcen,fwid);
					}
				}

				pCellWnd->MakeBitmap(FALSE);
			}
		}
	}
}


BOOL CDicomViewer::GetWinLevel(double &fcen,double &fwid)
{
	CDicomObject *pObj = NULL;

	if (m_pCellWndSelected != NULL)
		pObj = m_pCellWndSelected->GetDicomObject();
	else {
		if (GetSeriesMode()) {
			if (m_pSeriesObject)
				pObj = (CDicomObject *) m_pSeriesObject->m_DicomObjectList.GetHead();
		}
		else {
			if (m_pDicomObjectList) {
				pObj = (CDicomObject *) m_pDicomObjectList->GetHead();
			}
		}
	}
		
	if (pObj == NULL) 
		return FALSE;

	pObj->GetWinLevel(fcen,fwid);

	return TRUE;
}

 


void CDicomViewer::HFlip()
{
	POSITION pos;
	CCellWnd *pCellWnd;
	CDicomObject *pDicomObject;
	CString sModality("");
	BOOL bHFlip,bVFlip,bCtrlDown = FALSE,bScreenNeeded = FALSE;
	int nRotateAngle = 0;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	

	bScreenNeeded = FALSE;

	if (m_bSeriesMode == FALSE)
	{
		pCellWnd = m_pCellWndSelected;

		if (pCellWnd == NULL)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
		}

		if (pCellWnd != NULL)
		{
			pDicomObject = pCellWnd->GetDicomObject();

			if (pDicomObject != NULL)
			{
				sModality = pDicomObject->GetModality();

				if (sModality == "CT" || sModality == "MR")
				{
					bScreenNeeded = TRUE;
				}
			}
		}
	}

	pos = m_pCellWndList->GetHeadPosition();	

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected  || bCtrlDown || bScreenNeeded) && pCellWnd != NULL)
		{
			pCellWnd->GetFlip(bHFlip,bVFlip);

			pCellWnd->GetRotateAngle(nRotateAngle);

			if ((nRotateAngle % 180) == 0)
			{
				bHFlip = !bHFlip;
			}
			else
			{
				bVFlip = !bVFlip;
			}

			pCellWnd->SetFlip(bHFlip,bVFlip);
			pCellWnd->MakeBitmap(FALSE);
		}
	}
}


void CDicomViewer::VFlip()
{
	POSITION pos;
	CCellWnd *pCellWnd;
	CDicomObject *pDicomObject;
	CString sModality("");
	BOOL bHFlip,bVFlip,bCtrlDown,bScreenNeeded;
	int nRotateAngle = 0;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	

	bScreenNeeded = FALSE;

	if (m_bSeriesMode == FALSE)
	{
		pCellWnd = m_pCellWndSelected;

		if (pCellWnd == NULL)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();

		}

		if (pCellWnd != NULL)
		{
			pDicomObject = pCellWnd->GetDicomObject();

			if (pDicomObject != NULL)
			{
				sModality = pDicomObject->GetModality();

				if (sModality == "CT" || sModality == "MR")
				{
					bScreenNeeded = TRUE;
				}
			}
		}
	}

	pos = m_pCellWndList->GetHeadPosition();

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);
	
		if ((pCellWnd == m_pCellWndSelected || bCtrlDown || bScreenNeeded) && pCellWnd != NULL)
		{
			pCellWnd->GetFlip(bHFlip,bVFlip);
			pCellWnd->GetRotateAngle(nRotateAngle);
			
			if ((nRotateAngle % 180) == 0)
			{
				bVFlip = !bVFlip;
			}
			else
			{
				bHFlip = !bHFlip;
			}

			pCellWnd->SetFlip(bHFlip,bVFlip);
			pCellWnd->MakeBitmap(FALSE);
		}
	}
}

void CDicomViewer::RotateAngle90(int nDirection)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	CDicomObject *pDicomObject;
	CString sModality("");
	BOOL bCtrlDown,bScreenNeeded;
	int nRotateAngle = 0;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	
	
	bScreenNeeded = FALSE;

	if (m_bSeriesMode == FALSE)
	{
		pCellWnd = m_pCellWndSelected;

		if (pCellWnd == NULL)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
		}

		if (pCellWnd != NULL)
		{
			pDicomObject = pCellWnd->GetDicomObject();

			if (pDicomObject != NULL)
			{
				sModality = pDicomObject->GetModality();
				
				if (sModality == "CT" || sModality == "MR")
				{
					bScreenNeeded = TRUE;
				}
			}
		}
	}

	pos = m_pCellWndList->GetHeadPosition();

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown || bScreenNeeded) && pCellWnd != NULL)
		{
			pCellWnd->GetRotateAngle(nRotateAngle);

			nRotateAngle = nRotateAngle + ((nDirection > 0) ? 90 : 270);
			nRotateAngle = nRotateAngle % 360;

			pCellWnd->SetRotateAngle(nRotateAngle);
			pCellWnd->MakeBitmap(FALSE);
		}
	}
}



void CDicomViewer::SetExtLut(BOOL bUseExtLut,BYTE *lpLut)
{
	POSITION pos;
	CCellWnd *pCellWnd;

	m_bUseExtLut = bUseExtLut;

	memcpy(m_cExtLut,lpLut,768);

	pos = m_pCellWndList->GetHeadPosition();	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if (pCellWnd != NULL)
		{
			pCellWnd->SetExtLut(m_bUseExtLut,m_cExtLut);
			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}





void CDicomViewer::SetFakeColorSetNo(int nIndex)
{
	HMODULE hInst;
	HRSRC hRes;
	HANDLE hGlobal;
	void *pBuf;
	unsigned int nLen;
	CString sID;
	POSITION pos;
	CCellWnd *pCellWnd;
	BOOL bCtrlDown;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000);

	memset(m_cExtLut,0,768);

	if (nIndex >= 1 && nIndex <= 13)
	{
		sID.Format(_T("IDR_FAKECOLOR%X"),nIndex);
		//hInst = pApp->m_hInstance;//GetModuleHandle(NULL); //For Dll Bug
		hInst = GetCurrentModule();//AfxGetResourceHandle();
		hRes = FindResource(hInst,sID,_T("LUTDATA"));
		
		if (hRes != NULL)
		{		
			hGlobal = LoadResource(hInst,hRes);
		
			if (hGlobal != NULL)
			{
				pBuf = LockResource(hGlobal);
				nLen = ::SizeofResource(hInst,hRes);
			
				if (pBuf != NULL && nLen == 768)
				{
					memcpy(m_cExtLut,pBuf,768);
				}
			}
		}
	}

	pos = m_pCellWndList->GetHeadPosition();	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown) && pCellWnd != NULL)
		{
			pCellWnd->SetExtLut( (nIndex > 0) ? TRUE : FALSE,m_cExtLut);
			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}


void CDicomViewer::SetSmoothFilterNo(int nSmoothFilterNo)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	BOOL bCtrlDown;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	
	
	pos = m_pCellWndList->GetHeadPosition();	

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown) && pCellWnd != NULL)
		{
			pCellWnd->SetSmoothFilterNo(nSmoothFilterNo);
			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}



void CDicomViewer::SetSharpFilterNo(int nSharpFilterNo)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	BOOL bCtrlDown;
	
	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	

	pos = m_pCellWndList->GetHeadPosition();	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown) && pCellWnd != NULL)
		{
			pCellWnd->SetSharpFilterNo(nSharpFilterNo);

			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}



void CDicomViewer::CineMove(int nStep)
{
	CDicomObject *pDicomObject;
	CRect rect;

	if (m_pCellWndSelected == NULL) return;

	if (!m_pCellWndSelected->DicomObjectIsExisting()) return;
	
	pDicomObject = m_pCellWndSelected->GetDicomObject();

	if (pDicomObject == NULL) return;

	if (!pDicomObject->IsMultiFrame()) return;
	
	rect = m_pCellWndSelected->GetBorder();

	pDicomObject->CineMove(nStep);
	
	m_pCellWndSelected->MakeBitmap(FALSE);
	
	InvalidateRect(&rect);
}


void CDicomViewer::ExchangeObjectPos(CCellWnd *pCellWnd1,CCellWnd * pCellWnd2)
{
	CDicomObject *pDicomObject1=NULL,*pDicomObject2=NULL,*pDicomObj;
	POSITION pos,pos1,pos2;
	CSeriesObject *pSeriesObj1=NULL,*pSeriesObj2=NULL,*pSeriesObj;

	if (pCellWnd1 == NULL || pCellWnd2 == NULL || pCellWnd1 == pCellWnd2) return;
	
	pSeriesObj1 = pCellWnd1->GetSeriesObject();
	pSeriesObj2 = pCellWnd2->GetSeriesObject();

	if (m_bSeriesMode)
	{
		if (pSeriesObj1 == NULL || pSeriesObj2 == NULL) return;
	}
	else
	{
		pDicomObject1 = pCellWnd1->GetDicomObject();
		pDicomObject2 = pCellWnd2->GetDicomObject();

		if (pDicomObject1 == NULL || pDicomObject2 == NULL || pSeriesObj1 != pSeriesObj2) return;
	}

	pos1 = NULL;
	pos2 = NULL;

	if (m_bSeriesMode)
	{
		pos = m_pSeriesObjectList->GetHeadPosition();

		while (pos != NULL && (pos1 == NULL || pos2 == NULL))
		{
			pSeriesObj = (CSeriesObject *) m_pSeriesObjectList->GetAt(pos);

			if (pSeriesObj != NULL)
			{
				if (pSeriesObj == pSeriesObj1)
				{
					pos1 = pos;
				}


				if (pSeriesObj == pSeriesObj2)
				{
					pos2 = pos;
				}
			}

			m_pSeriesObjectList->GetNext(pos);
		}

		if (pos1 == NULL || pos2 == NULL) return;

		std::swap(m_pSeriesObjectList->GetAt(pos1),m_pSeriesObjectList->GetAt(pos2));

		pCellWnd1->SetSeriesObject(pSeriesObj2);
		pCellWnd2->SetSeriesObject(pSeriesObj1);


	}
	else
	{
		pos = m_pDicomObjectList->GetHeadPosition();

		while (pos != NULL && (pos1 == NULL || pos2 == NULL))
		{
			pDicomObj = (CDicomObject *) m_pDicomObjectList->GetAt(pos);

			if (pDicomObj != NULL)
			{
				if (pDicomObj == pDicomObject1)
				{
					pos1 = pos;
				}

				if (pDicomObj == pDicomObject2)
				{
					pos2 = pos;
				}
			}

			m_pDicomObjectList->GetNext(pos);
		}

		if (pos1 == NULL || pos2 == NULL) return;

		std::swap(m_pDicomObjectList->GetAt(pos1),m_pDicomObjectList->GetAt(pos2));

		pCellWnd1->SetDicomObject(pSeriesObj1,pDicomObject2);
		pCellWnd2->SetDicomObject(pSeriesObj1,pDicomObject1);
	}

	pCellWnd1->MakeBitmap(FALSE);
	pCellWnd2->MakeBitmap(FALSE);

}

/////////////////////////////////////////////////////////////////////////////
// CDicomViewer message handlers

BOOL CDicomViewer::Create(CWnd* pParentWnd, UINT nID) 
{
	BOOL bReturn;

	bReturn = CWnd::CreateEx(0,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0), pParentWnd, nID, NULL);

	return bReturn;
}


void CDicomViewer::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;

	CWnd::OnSize(nType, cx, cy);

	ResizeCellWnd();	

}

BOOL CDicomViewer::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


void CDicomViewer::OnPaint() 
{
	CRect rect;
	CDC memdc,memdc2;
	CBitmap membmp,*poldbmp,*poldbmp2;
	int nIdx,nWidth,nHeight;
	int nRows,nCols;
	POSITION pos;
	CCellWnd *pCellWnd;
	BOOL bSelected;

	CPaintDC dc(this); // device context for painting


	if (m_pCellWndList == NULL) return;

	if (m_pCellWndList->GetCount() == 0) return;

	GetClientRect(&rect);

	nWidth = rect.Width();
	nHeight = rect.Height();


	if (m_bSeriesMode)
	{
		nRows = m_nSeriesRows;
		nCols = m_nSeriesCols;
	}
	else
	{
		nRows = m_nRows;
		nCols = m_nCols;
	}

	memdc.CreateCompatibleDC(NULL);

	membmp.CreateCompatibleBitmap(&dc,nWidth,nHeight);
	
	poldbmp = memdc.SelectObject(&membmp);
	
	memdc.FillSolidRect(0,0,nWidth,nHeight,RGB(0,0,0));

	pos = m_pCellWndList->GetHeadPosition();

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);
		
		bSelected = (pCellWnd == m_pCellWndSelected);
		pCellWnd->DrawObject(&memdc,bSelected);
	}


	if (m_bMagnifyActive)
	{
		if (m_pCellWndSelected != NULL)
		{
			m_pCellWndSelected->DrawMagnifier(&memdc,rect,m_ptStart,m_fMagnifierZoomFactor);
		} 
	}


	dc.BitBlt(0,0,nWidth,nHeight,&memdc,0,0,SRCCOPY);

	memdc.SelectObject(poldbmp);

	membmp.DeleteObject();

	memdc.DeleteDC();
}



void CDicomViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nFlag,nPos;
	CRect rect;
	CPoint ptTopLeft,ptRightBottom,pt(-1,-1);
	CCellWnd *pOldCellWnd;
	CWnd *pParent;
	NMHDR nmhdr;
	UINT nID;

	m_dwLBDownTickCount = GetTickCount();
	m_bLBDown = TRUE;
	m_bCursor = TRUE;

	m_bMeasureAngleOk = TRUE;

	nFlag = 1;
	nPos = 0;
	
	pOldCellWnd = m_pCellWndSelected;

	SelectCellWnd(point);

	SetCapture();

	SetFocus();

	if (pOldCellWnd != NULL)
	{
		pOldCellWnd->SetMeasureCobbPoint1(pt);
		pOldCellWnd->SetMeasureCobbPoint2(pt);
		pOldCellWnd->SetMeasureCobbPoint3(pt);
		pOldCellWnd->SetMeasureCobbPoint4(pt);
	}

	if (m_pCellWndSelected != NULL && m_pCellWndSelected != pOldCellWnd)
	{
		m_pCellWndSelected->SetMeasureCobbPoint1(pt);
		m_pCellWndSelected->SetMeasureCobbPoint2(pt);
		m_pCellWndSelected->SetMeasureCobbPoint3(pt);
		m_pCellWndSelected->SetMeasureCobbPoint4(pt);
	}



	m_ptStart = point;

	switch(m_nLBFuncNo)
	{
	case 0:

		if (m_bExchangePosMode)
		{
			if (m_pCellWndSelected != NULL)
			{
				pParent = GetParent();

				if (m_pExchangePosWnd1 == NULL)
				{
					m_pExchangePosWnd1 = m_pCellWndSelected;
					
					if (pParent != NULL)
					{
						pParent->SendMessage(WM_SHOWMESSAGE,202);
					}
				}
				else
				{
					m_pExchangePosWnd2 = m_pCellWndSelected;

					if (m_pExchangePosWnd1 != m_pExchangePosWnd2)
					{
						ExchangeObjectPos(m_pExchangePosWnd1,m_pExchangePosWnd2);
					}

					m_bExchangePosMode = FALSE;
					m_pExchangePosWnd1 = NULL;
					m_pExchangePosWnd2 = NULL;

					if (pParent != NULL)
					{
						pParent->SendMessage(WM_SHOWMESSAGE,0);
					}

					Invalidate();
				}
			}
		}

		break;
	case 1:
		break;
	case 2:
		break;
	case 3:	// ≤‚CT÷µ
			// ∑÷µ„°¢æÿ–Œ°¢Õ÷‘∞»˝÷ÿ–Œ◊¥£¨ –Œ◊¥»°◊‘”⁄≤‚¡ø…Ë÷√£¨µ±≤‚¡ø…Ë±∏Œ™≥˝æÿ–ŒªÚÕ÷‘∞“‘Õ‚ ±£¨Œ™µ„≤‚¡ø

		if (m_pCellWndSelected != NULL)
		{
			rect = m_pCellWndSelected->GetImageViewRect();

			if (rect.PtInRect(point))
			{
				m_pCellWndSelected->SetDrawCTValue(TRUE);
				m_pCellWndSelected->SetCTValuePoint(point);
				
				/////////////////////////////////////////////

				m_pCellWndSelected->SetMeasurePoint1(point);

			}
			else
			{
				m_pCellWndSelected->SetDrawCTValue(FALSE);
			}
		}


		Invalidate();
		break;
	case 4:	// ∑≈¥Ûæµ
		
		m_bMagnifyActive = TRUE;

		Invalidate();

		break;
	}

	nID = GetDlgCtrlID();

	nmhdr.code = NM_CELLWNDCLICK;//WM_LBUTTONDOWN;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = nID;
	
	GetParent()->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);


	if (m_nRBFuncNo == 3 && m_nMeasureType == 4)
	{
		if (m_pCellWndSelected != NULL)
		{
			m_pCellWndSelected->SetDrawMeasure(FALSE);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);

}

void CDicomViewer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UINT nID;
	CPoint pt;
	CRect rtBorder;
	NMHDREX nmhdr;
	int nShiftX,nShiftY;
	POSITION pos;
	CCellWnd *pCellWnd;

	m_dwLBUpTickCount = GetTickCount();

	m_bLBDown = FALSE;

	m_bCursor = FALSE;

	::SetCursor(NULL);

	if (!m_bRBDown)
	{
		if ((abs((int)(m_dwLBDownTickCount - m_dwRBDownTickCount)) <= m_nButtonDelayFactor) && 
			(abs((int)(m_dwLBUpTickCount - m_dwRBUpTickCount)) <= m_nButtonDelayFactor))
		{
			nID = GetDlgCtrlID();

			nmhdr.nmhdr.code = NM_MAYBEMINIBAR;
			nmhdr.nmhdr.hwndFrom = m_hWnd;
			nmhdr.nmhdr.idFrom = nID;
			nmhdr.point = point;
	
			GetParent()->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
		}
	}

	ReleaseCapture();

	switch (m_nLBFuncNo)
	{
	case 0:

		break;
	case 1:	//Scroll Images
		InvalidateRect(NULL);
		break;
	case 2:	//Move Images

		if (m_pCellWndSelected != NULL)
		{
			m_pCellWndSelected->GetShift(nShiftX,nShiftY);

			if (nFlags & MK_CONTROL)
			{
				pos = m_pCellWndList->GetHeadPosition();

				while (pos != NULL)
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

					if (pCellWnd != m_pCellWndSelected)
					{
						pCellWnd->SetShift(nShiftX,nShiftY);
					}
				}
			}
		}

		InvalidateRect(NULL);

		break;
	case 3:

		if (m_pCellWndSelected != NULL)
		{
			m_pCellWndSelected->SetDrawCTValue(FALSE);
			

			//rtBorder = m_pCellWndSelected->GetBorder();
			//InvalidateRect(rtBorder);
		}
		break;
	case 4:  // Magnify Glass

		m_bMagnifyActive = FALSE;	     //Flag
		Invalidate(FALSE);				 //Redraw itself

		break;
	}


	CWnd::OnLButtonUp(nFlags, point);
		
}

void CDicomViewer::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	int nFlag,nPos;
	UINT nID;
	NMHDREX nmhdr;
	CWnd *pParentWnd;
	CCellWnd *pOldCellWnd;
	CPoint pt1,pt2,pt3,pt4,pt;
	CDicomObject *pDicomObject;
	BOOL bContinue;
	double fx = 0,fy = 0;

	m_dwRBDownTickCount = GetTickCount();

	pOldCellWnd = m_pCellWndSelected;

	SelectCellWnd(point);

	SetCapture();

	SetFocus();

	m_bRBDown = TRUE;
	m_bCursor = TRUE;

	nFlag = 1;
	nPos = 0;
	
///	AfxGetMainWnd()->SendMessage(WM_VIEWMINIBAR,nFlag,nPos);

	m_ptStart = point;
	m_ptFirstPointAfterRightButtonDown = point;

	switch(m_nRBFuncNo)
	{
	case 0:

		/*
		if (m_pCellWndSelected != NULL)
		{
			
			rect = m_pCellWndSelected->GetImageViewRect();

			if (rect.PtInRect(point))
			{
				nID = GetDlgCtrlID();
				nmhdr.nmhdr.code = NM_CELLWNDRBCLICK;
				nmhdr.nmhdr.hwndFrom = m_hWnd;
				nmhdr.nmhdr.idFrom = nID;
				nmhdr.point = point;
		
				pParentWnd = GetParent();

				if (pParentWnd != NULL) {
					pParentWnd->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
					m_bPopupMenuWorking = TRUE;
				}
			}
		}
		*/

		nID = GetDlgCtrlID();
		nmhdr.nmhdr.code = NM_CELLWNDRBCLICK;
		nmhdr.nmhdr.hwndFrom = m_hWnd;
		nmhdr.nmhdr.idFrom = nID;
		nmhdr.point = point;
		
		pParentWnd = GetParent();

		if (pParentWnd != NULL) {
			pParentWnd->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
			m_bPopupMenuWorking = TRUE;
		}

		break;
	case 1: // ¥∞øÌ¥∞Œªµ˜’˚
		break;
	case 2: // ∑≈¥ÛÀı–°
		break;
	case 3: // ≥§∂»,Ω«∂»”Î√Êª˝µ»≤‚¡ø
		
		if (m_pCellWndSelected != NULL)
		{

			bContinue = TRUE;
			pParentWnd = GetParent();

			if (m_nMeasureType == 99)
			{
				m_bCancelCalibrate = FALSE;

				pDicomObject = m_pCellWndSelected->GetDicomObject();

				if (pDicomObject == NULL)
				{
					bContinue = FALSE;
					m_bCancelCalibrate = TRUE;

					if (pParentWnd != NULL)
					{
						pParentWnd->SendMessage(WM_SHOWMESSAGE,401);
					}					
				}
				else
				{
					pDicomObject->GetCalibrateValueA(fx,fy);

					if (fx > 0 && fy > 0)
					{
						bContinue = FALSE;
						m_bCancelCalibrate = TRUE;
						pParentWnd->SendMessage(WM_SHOWMESSAGE,402);
					}
					else
					{
						pParentWnd->SendMessage(WM_SHOWMESSAGE,0);
					}
				}
			}
			

			rect = m_pCellWndSelected->GetImageViewRect();

			if (rect.PtInRect(point) && bContinue)
			{
				SetCursor(m_hMeasureCursor);

				m_ptMeasurePoint1 = point;

				m_ptMeasurePoint2 = point;

				m_pCellWndSelected->SetMeasurePoint1(point);

				m_pCellWndSelected->ClearMeasurePointsList();
				m_pCellWndSelected->AddMeasurePoint(point);
					
				if (m_nMeasureType == 4)
				{
					m_bMeasureAngleOk = FALSE;

					m_pCellWndSelected->SetMeasureAnglePoint1(point);
				}

				if (m_nMeasureType == 5)
				{					

					pt = CPoint(-1,-1);

					if ((pOldCellWnd != m_pCellWndSelected) && (pOldCellWnd != NULL)) 
					{
						pOldCellWnd->SetMeasureCobbPoint1(pt);
						pOldCellWnd->SetMeasureCobbPoint2(pt);
						pOldCellWnd->SetMeasureCobbPoint3(pt);
						pOldCellWnd->SetMeasureCobbPoint4(pt);
					}

					pt1 = m_pCellWndSelected->GetMeasureCobbPoint1();
					pt2 = m_pCellWndSelected->GetMeasureCobbPoint2();
					pt3 = m_pCellWndSelected->GetMeasureCobbPoint3();
					pt4 = m_pCellWndSelected->GetMeasureCobbPoint4();


					if ((pt1.x == -1 && pt1.y == -1) || (pt4.x != -1 && pt4.y != -1)) 
					{
						m_pCellWndSelected->SetMeasureCobbPoint1(point);
						m_pCellWndSelected->SetMeasureCobbPoint2(pt);
						m_pCellWndSelected->SetMeasureCobbPoint3(pt);
						m_pCellWndSelected->SetMeasureCobbPoint4(pt);						
						
					}
					else if (pt3.x == -1 && pt3.y == -1)
					{
						m_pCellWndSelected->SetMeasureCobbPoint3(point);
						m_pCellWndSelected->SetMeasureCobbPoint4(pt);						
					}

				}

				m_pCellWndSelected->SetDrawMeasure(TRUE);
			}
			else
			{
				m_pCellWndSelected->SetDrawMeasure(FALSE);

			}

		}

		break;
	}

	CWnd::OnRButtonDown(nFlags, point);
	
}

void CDicomViewer::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CPoint pt,pt1,pt2,pt3,pt4;
	UINT nID;
	double fcen,fwid,fzoom;
	double fCalibrateValue = 0,fLength1 = 0,fLength2 = 0;
	POSITION pos,pos1,pos2;
	CString sModality("");
	BOOL bDone = FALSE,bSeriesNeeded = FALSE;
	CDicomObject *pDicomObject;
	CSeriesObject *pSeriesObject;
	CCellWnd *pCellWnd;
	CWnd *pParentWnd;
	CRect rect;
	NMHDREX nmhdr;
	CCalibrateDlg dlgCalibrate;

	m_dwRBUpTickCount = GetTickCount();

	m_bRBDown = FALSE;
	m_bCursor = FALSE;

	m_bPopupMenuWorking = FALSE;

	::SetCursor(NULL);

	if (!m_bLBDown)
	{
		// ∞¥œ¬ Û±Í”“º¸£¨√ª”–“∆∂ØæÕµØø™”“º¸‘Úœ‘ æ”“º¸≤Àµ•

		if (point.x == this->m_ptFirstPointAfterRightButtonDown.x && point.y == this->m_ptFirstPointAfterRightButtonDown.y) {

			nID = GetDlgCtrlID();
			nmhdr.nmhdr.code = NM_CELLWNDRBCLICK;
			nmhdr.nmhdr.hwndFrom = m_hWnd;
			nmhdr.nmhdr.idFrom = nID;
			nmhdr.point = point;
		
			pParentWnd = GetParent();

			if (pParentWnd != NULL) {
				pParentWnd->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
			}

			return;
		}

		if ((abs((int)(m_dwLBDownTickCount - m_dwRBDownTickCount)) <= m_nButtonDelayFactor) && 
			(abs((int)(m_dwLBUpTickCount - m_dwRBUpTickCount)) <= m_nButtonDelayFactor))
		{
			nID = GetDlgCtrlID();

			nmhdr.nmhdr.code = NM_MAYBEMINIBAR;
			nmhdr.nmhdr.hwndFrom = m_hWnd;
			nmhdr.nmhdr.idFrom = nID;
			nmhdr.point = point;
	
			GetParent()->SendMessage(WM_NOTIFY,nID,(LPARAM)&nmhdr);
		}
	}


	ReleaseCapture();

	m_bRBDown = FALSE;

	switch (m_nRBFuncNo)
	{
	case 1:	//¥∞øÌ¥∞Œªµ˜’˚
		
		if (m_pCellWndSelected != NULL)
		{
			pDicomObject = m_pCellWndSelected->GetDicomObject();

			if (pDicomObject != NULL)
			{
				pDicomObject->GetWinLevel(fcen,fwid);

				sModality = pDicomObject->GetModality();

				if (m_bSeriesMode == FALSE && (sModality == "CT" || sModality == "MR"))
				{
					bSeriesNeeded = TRUE;
				}
				else
				{
					bSeriesNeeded = FALSE;
				}

				bDone = FALSE;

				pos1 = m_pCellWndList->GetHeadPosition();

				while (pos1 != NULL)
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos1);

					if (bDone)
					{
						if (pCellWnd != NULL) pCellWnd->MakeBitmap(FALSE);
					}
					else
					{
						if (((pCellWnd == m_pCellWndSelected ) || (nFlags & MK_CONTROL) || bSeriesNeeded ) && (pCellWnd != NULL))
						{
							if (m_bSeriesMode || bSeriesNeeded)
							{
								pSeriesObject = pCellWnd->GetSeriesObject();
							
								if (pSeriesObject != NULL)
								{
									pos2 = pSeriesObject->m_DicomObjectList.GetHeadPosition();

									while (pos2 != NULL)
									{
										pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos2);

										if (pDicomObject != NULL)
										{
											pDicomObject->SetWinLevel(fcen,fwid);
										}
									}

									if (bSeriesNeeded) bDone = TRUE;
								}
							}
							else
							{
								pDicomObject = pCellWnd->GetDicomObject();

								if (pDicomObject != NULL)
								{
									pDicomObject->SetWinLevel(fcen,fwid);
								}
							}

							pCellWnd->MakeBitmap(FALSE);

						}
					}
				}

				Invalidate();
			}
		}

		break;
	case 2:	//∑≈¥ÛªÚÀı–°

		if (m_pCellWndSelected != NULL)
		{			
			m_pCellWndSelected->GetZoomFactor(fzoom);

			pos = m_pCellWndList->GetHeadPosition();

			while (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				if ((pCellWnd != m_pCellWndSelected && (nFlags & MK_CONTROL)) && pCellWnd != NULL)
				{				
					pCellWnd->SetZoomFactor(fzoom);

					rect = pCellWnd->GetBorder();

					pCellWnd->MakeBitmap(FALSE);
				}
			}
		
			Invalidate();
		}
		break;

	case 3:	//≥§∂»,Ω«∂»”Î√Êª˝≤‚¡ø
		if (m_pCellWndSelected != NULL)
		{
			pParentWnd = GetParent();

			if (GetMeasureType() == 4)
			{
				m_ptMeasurePoint2 = point;

				m_pCellWndSelected->SetMeasureAnglePoint2(point);
			}
			else if (GetMeasureType() == 5)
			{
				pt1 = m_pCellWndSelected->GetMeasureCobbPoint1();
				pt2 = m_pCellWndSelected->GetMeasureCobbPoint2();
				pt3 = m_pCellWndSelected->GetMeasureCobbPoint3();
				pt4 = m_pCellWndSelected->GetMeasureCobbPoint4();

				if ((pt2.x == -1 && pt2.y == -1) && (pt1.x != -1 && pt1.y != -1))
				{
					m_pCellWndSelected->SetMeasureCobbPoint2(point);
				}
				else if ((pt4.x == -1 && pt4.y == -1) && (pt3.x != -1 && pt3.y != -1))
				{
					m_pCellWndSelected->SetMeasureCobbPoint4(point);
				}

			} 
			
			else if (GetMeasureType() == 99)
			{
				if (pParentWnd != NULL) pParentWnd->SendMessage(WM_SHOWMESSAGE,0);

				pDicomObject = m_pCellWndSelected->GetDicomObject();

				if (pDicomObject != NULL && m_bCancelCalibrate == FALSE)
				{										
					fLength1 = m_pCellWndSelected->GetLineLengthOfPoint1AndPoint2();

					if (fLength1 > 10)
					{
						if (dlgCalibrate.DoModal() == IDOK)
						{
							fLength2 = dlgCalibrate.GetLineLength();
							fCalibrateValue = fLength2 / fLength1;

							if (fCalibrateValue > 0)
							{
								pDicomObject->SetCalibrateValueM(fCalibrateValue,fCalibrateValue);
							}
						}
					}
				}

				m_nMeasureType = m_nMeasureTypePrev;
				m_pCellWndSelected->SetDrawMeasure(FALSE);
			}
			else
			{
				m_pCellWndSelected->SetDrawMeasure(FALSE);
			}
		}
		

		break;
	}

	CWnd::OnRButtonUp(nFlags, point);
}



void CDicomViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	CString sText;
	CRect rect,rtBorder,rtView;
	CPoint ptLeftTop,ptRightBottom;
	double fcen,fwid,fzoom,flen;
	int nStepX,nStepY,nLen,nCurrentIndex,nBitsStored,nMaxWid,nFactor;
	int nShiftX,nShiftY;
	CDicomObject *pDicomObject;
	CSeriesObject *pSeriesObject;

	if (m_bPrintPreviewMode) return;

	if (m_bLBDown)
	{
		switch(m_nLBFuncNo)
		{
		case 0:

			break;
		case 1:
			if (m_pCellWndSelected != NULL)
			{

				if (m_bCursor)
				{
					m_bCursor = FALSE;
					::SetCursor(m_hLBFunc1Cursor);
				}

				nStepX = (point.x - m_ptStart.x);
				nStepY = (point.y - m_ptStart.y);

				nLen = (int) sqrt((float)(nStepX * nStepX + nStepY * nStepY));

				if (nLen > 4)
				{
					/*
					if ((point.x > m_ptStart.x && point.y > m_ptStart.y) || 
						(point.x > m_ptStart.x && point.y < m_ptStart.y) ||
						(point.x > m_ptStart.x && point.y == m_ptStart.y))
					{
						nLen = 1;
					}
					else if ((point.x < m_ptStart.x && point.y < m_ptStart.y) ||
						(point.x < m_ptStart.x && point.y > m_ptStart.y) ||
						(point.x < m_ptStart.x && point.y == m_ptStart.y))
					{
						nLen = -1;
					}
					else
					{
						nLen = 0;
					}
					*/

					if ((point.y < m_ptStart.y) || (point.y == m_ptStart.y && point.x > m_ptStart.x))
					{
						nLen = -1;
					}
					else
					{
						nLen = 1;
					}

					m_ptStart = point;

				}
				else
				{
					nLen = 0;
				}

				if (nLen != 0)
				{

					if (m_bSeriesMode)
					{

						pSeriesObject = m_pCellWndSelected->GetSeriesObject();

						if (pSeriesObject != NULL)
						{
							nCurrentIndex = pSeriesObject->GetCurrentIndex() + nLen;

							if (nCurrentIndex < 0) nCurrentIndex = 0;

							if (nCurrentIndex >= pSeriesObject->GetDicomObjectCount()) 
								nCurrentIndex = pSeriesObject->GetDicomObjectCount() - 1;

							if (nCurrentIndex != pSeriesObject->GetCurrentIndex())
							{
								pSeriesObject->SetCurrentIndex(nCurrentIndex);
						
								m_pCellWndSelected->SetSeriesObject(pSeriesObject);

								if (m_pCellWndSelected->IsCTMR()) //  && m_pCellWndSelected->HaveLocalizer()) not finished
								{
									Invalidate();
								}
								else
								{
									rtBorder = m_pCellWndSelected->GetBorder();
									InvalidateRect(rtBorder);
								}
							}
						}
					}
					else
					{
						nCurrentIndex = m_nCurrentIndex + nLen;

						if (nCurrentIndex < 0) nCurrentIndex = 0;
	
						if (nCurrentIndex > m_nMaxIndex) nCurrentIndex = m_nMaxIndex;
	
						if (nCurrentIndex != m_nCurrentIndex)
						{
							m_nCurrentIndex = nCurrentIndex;
						
							SetScrollPos(SB_VERT,m_nCurrentIndex);

							BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject);

							Invalidate();

							SelectCellWnd(point);
						}
					}
				}
			}
			
			break;
		case 2:

			if (m_pCellWndSelected != NULL)
			{
				if (m_bCursor)
				{
					m_bCursor = FALSE;
					::SetCursor(m_hLBFunc2Cursor);
				}

				m_pCellWndSelected->GetShift(nShiftX,nShiftY);

				nStepX = (point.x - m_ptStart.x);
				nStepY = (point.y - m_ptStart.y);

				nShiftX = nShiftX + nStepX;
				nShiftY = nShiftY + nStepY;

				m_pCellWndSelected->SetShift(nShiftX,nShiftY);

				rtBorder = m_pCellWndSelected->GetBorder();

				InvalidateRect(rtBorder);

			}

			m_ptStart = point;
			break;
		case 3:	//œ‘ æCT÷µ

			if (m_pCellWndSelected != NULL)
			{
				rect = m_pCellWndSelected->GetImageViewRect();

				if (rect.PtInRect(point))
				{
					m_pCellWndSelected->SetDrawCTValue(TRUE);
					m_pCellWndSelected->SetCTValuePoint(point);

					////////////////////////////////////////////

					m_pCellWndSelected->SetMeasurePoint2(point);

					m_pCellWndSelected->SetMeasurePoint(point);


					Invalidate();
				}
				else
				{
					m_pCellWndSelected->SetDrawCTValue(FALSE);
				}
			}

			break;
		case 4: // Magnify Glass
			
			m_ptStart = point;

			if (m_bMagnifyActive && m_pCellWndSelected != NULL)
			{
				rect = m_pCellWndSelected->GetImageViewRect();

				if (rect.PtInRect(point))
				{
								
					::SetCursor(NULL);
					
					InvalidateRect(NULL);
				}
			}

			break;
		}
	}
	else if (m_bRBDown)
	{
		switch(m_nRBFuncNo)
		{
		case 1:	//¥∞øÌ¥∞Œªµ˜’˚
						
			if (m_pCellWndSelected != NULL)
			{

				pDicomObject = m_pCellWndSelected->GetDicomObject();

				if (pDicomObject != NULL)
				{
					if (m_bCursor)
					{
						m_bCursor = FALSE;
						::SetCursor(m_hRBFunc1Cursor);
					}


					pDicomObject->GetWinLevel(fcen,fwid);
				

					nBitsStored = pDicomObject->GetBitsStored();

					nMaxWid =  (1 << nBitsStored) - 1;

					if (nBitsStored <= 8)
					{
						nFactor = 1;
					}
					else if (nBitsStored <= 10)
					{
						nFactor = 2;
					}
					else if (nBitsStored <= 12)
					{
						nFactor = 3;
					}
					else 
					{
						nFactor = 4;
					}

					fcen = fcen + (point.y - m_ptStart.y) * nFactor * m_nSensitivity ;
					fwid = fwid + (point.x - m_ptStart.x) * nFactor * m_nSensitivity;


					if (fwid < 1) fwid = 1;

					if (fwid > nMaxWid) fwid = nMaxWid;

					pDicomObject->SetWinLevel(fcen,fwid);
				
					m_pCellWndSelected->MakeBitmap(TRUE);

					rtBorder = m_pCellWndSelected->GetBorder();

					InvalidateRect(&rtBorder);
				}
			}

			m_ptStart = point;
	
			break;
		case 2:	//”∞œÒ∑≈¥ÛÀı–°

			if (m_pCellWndSelected != NULL)
			{

				if (m_bCursor)
				{
					m_bCursor = FALSE;
					::SetCursor(m_hRBFunc2Cursor);
				}


				nStepX = (point.x - m_ptStart.x);
				nStepY = (point.y - m_ptStart.y);

				flen = sqrt((float)(nStepX * nStepX + nStepY * nStepY)) / 100.0;

				if ((point.x > m_ptStart.x && point.y > m_ptStart.y) || 
					(point.x > m_ptStart.x && point.y < m_ptStart.y) ||
					(point.x > m_ptStart.x && point.y == m_ptStart.y))
				{

				}
				else if ((point.x < m_ptStart.x && point.y < m_ptStart.y) ||
					(point.x < m_ptStart.x && point.y > m_ptStart.y) ||
					(point.x < m_ptStart.x && point.y == m_ptStart.y))
				{
					flen = -flen;
				}
				else
				{
					flen = 0;
				}

				if (flen != 0)
				{

					fzoom = m_pCellWndSelected->GetRealZoomFactor() + flen;

					if (fzoom < 0.1) fzoom = 0.1;
					if (fzoom > 32.0) fzoom = 32;
											
					m_pCellWndSelected->SetZoomFactor(fzoom);

					rect = m_pCellWndSelected->GetBorder();

					InvalidateRect(rect);
				}

				m_ptStart = point;
			}
			break;		
		case 3:	//≥§∂»,Ω«∂»”Î√Êª˝≤‚¡ø
			
			if (m_ptMeasurePoint1.x != -1 && m_ptMeasurePoint1.y != -1 && m_pCellWndSelected != NULL)
			{
				rect = m_pCellWndSelected->GetImageViewRect();

				if (rect.PtInRect(point))
				{
					m_pCellWndSelected->SetMeasurePoint2(point);
					m_pCellWndSelected->AddMeasurePoint(point);

					m_pCellWndSelected->SetMeasurePoint(point);

				}

				rect = m_pCellWndSelected->GetBorder();

				InvalidateRect(&rect);
			}

			break;
		}
	}
	else   // neither left mouse button down nor right mouse button down
	{
		if (m_nRBFuncNo == 3 && GetMeasureType() == 4 && !m_bMeasureAngleOk) // Angle Measure
		{
			if (m_ptMeasurePoint1.x != -1 && m_ptMeasurePoint1.y != -1 && m_pCellWndSelected != NULL)
			{
				if (m_ptMeasurePoint1 != m_ptMeasurePoint2)
				{
					rect = m_pCellWndSelected->GetImageViewRect();

					if (rect.PtInRect(point) )
					{
						m_pCellWndSelected->SetMeasurePoint(point);
					}

					rect = m_pCellWndSelected->GetBorder();

					InvalidateRect(&rect);
				}
			}
			
		}

	}

	CWnd::OnMouseMove(nFlags, point);
}


void CDicomViewer::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nIdx;
	CSeriesObject *pSeriesObject;
	CDicomObject  *pDicomObj;
	CCellWnd *pCellWnd;

	POSITION pos;

	
	if (m_bSeriesMode)
	{
		SelectCellWnd(point);
		
		if (m_pCellWndSelected  != NULL)
		{
			pSeriesObject = m_pCellWndSelected->GetSeriesObject();
			DisplaySeriesObject(pSeriesObject);

			if (pSeriesObject->m_DicomObjectList.GetCount() == 1) {

				m_bOneCellWndMode = TRUE;					
				m_nOldCurrentIndex = m_nCurrentIndex;

				m_nOldRows = m_nRows;
				m_nOldCols = m_nCols;
					
				AdjustMatrix(1,1);

				Invalidate();
			}
		}
	}
	else
	{

		if (m_wndPrintSCU != NULL && ::IsWindowVisible(m_wndPrintSCU))
		{
			SelectCellWnd(point);

			::SendMessage(m_wndPrintSCU,WM_PRINTSELECT,0,0);

			return;
		}


		if (m_bOneCellWndMode)
		{
			pSeriesObject = GetSeriesObject();

			if (m_nRows == 1 && m_nCols == 1 && pSeriesObject->m_DicomObjectList.GetCount() == 1) {

				m_bOneCellWndMode = FALSE;

				if (InAutoBrowsing())
					ExitBrowseMode();

				if (GetCineMode() != 0)
					SetCineMode(0);

				DisplaySeriesObjectList(pSeriesObject);

				return;
			}


			m_bOneCellWndMode = FALSE;

			if (m_nOldCurrentIndex >= 0 && m_nOldCurrentIndex <= m_nMaxIndex)
				m_nCurrentIndex = m_nOldCurrentIndex;


			if (m_pCellWndSelected != NULL)
				pDicomObj = m_pCellWndSelected->GetDicomObject();
			else
				pDicomObj = NULL;

			m_nRows = m_nOldRows;
			m_nCols = m_nOldCols;

			AdjustVScrollBar();

			CreateCellWnds(m_bSeriesMode);

			BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject,pDicomObj);

			if (m_pCellWndSelected == NULL)
			{
				pos = m_pCellWndList->GetHeadPosition();
				while (pos != NULL && m_pCellWndSelected == NULL)
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

					if (pCellWnd->GetDicomObject() != NULL)
					{
						m_pCellWndSelected = pCellWnd;
					}
				}
			}

			SetVScrollBarPos(m_nCurrentIndex);
			

			Invalidate();
			
		}
		else
		{
			if (m_nRows != 1 || m_nCols != 1)
			{
				if (SelectCellWnd(point))
				{

					m_bOneCellWndMode = TRUE;					
					m_nOldCurrentIndex = m_nCurrentIndex;

					m_nOldRows = m_nRows;
					m_nOldCols = m_nCols;
					
					AdjustMatrix(1,1);

					Invalidate();

				}
			}
			else	// µ±«∞µ•∑˘œ‘ æƒ£ Ω,À´ª˜‘Ú∑µªÿµΩSeries Mode
			{				

				if (InAutoBrowsing())
				{
					ExitBrowseMode();
				}

				if (GetCineMode() != 0)
				{
					SetCineMode(0);
				}

				pSeriesObject = GetSeriesObject();

				DisplaySeriesObjectList(pSeriesObject);
			}
		}
		
	}
	
	//CWnd::OnLButtonDblClk(nFlags, point);
}

void CDicomViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	POSITION pos;
	CDicomObject *pDicomObject;
	CCellWnd *pCellWnd;
	double fcen,fwid;
	int nStep,nBitsStored,nMaxWid;

	if (nChar >= 37 && nChar <= 40 && m_pCellWndSelected != NULL && m_pDicomObjectList != NULL)
	{
		pDicomObject = m_pCellWndSelected->GetDicomObject();

		if (pDicomObject != NULL)
		{

			pDicomObject->GetWinLevel(fcen,fwid);
		
			if ((GetKeyState(VK_CONTROL) & 0x8000) > 0) 
			{
				nStep = 20 * nRepCnt;
			}
			else
			{
				nStep = nRepCnt;
			}

			if (nChar == 37)
			{
				fwid -= nStep;
			}
			else if (nChar == 39)
			{
				fwid += nStep;
			}
			else if (nChar == 38)
			{
				fcen -= nStep;
			}
			else if (nChar == 40)
			{
				fcen += nStep;
			}
			
			nBitsStored = pDicomObject->GetBitsStored();
			nMaxWid = (1 << nBitsStored) - 1;

			if (nMaxWid < 0) nMaxWid = 4096;

			if (fwid < 1) fwid = 1;
			if (fwid > nMaxWid ) fwid = nMaxWid;

			pos = m_pDicomObjectList->GetHeadPosition();

			while (pos != NULL)
			{
				pDicomObject = (CDicomObject *) m_pDicomObjectList->GetNext(pos);
				pDicomObject->SetWinLevel(fcen,fwid);
			}
		
			pos= m_pCellWndList->GetHeadPosition();
			while (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				pCellWnd->MakeBitmap(FALSE);
			}

			Invalidate();
		}
	}
	

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

}


void CDicomViewer::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default


	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}



void CDicomViewer::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_bRBDown)
	{
		m_bRBDown = FALSE;

		::SetCursor(NULL);

		switch(m_nRBFuncNo)
		{
		case 2:

			break;
		}
	}

	if (m_bLBDown)
	{
		m_bLBDown = FALSE;

		//if (m_bMagnify)
		//{
		//	m_bMagnify = FALSE;
		//	m_wndMagnify.ShowWindow(SW_HIDE);
		//}

		::SetCursor(NULL);

	}

}

void CDicomViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int ni, nOldPos,nCurPos,nCellWndCount,nObjCount;
	CSeriesObject *pObjSelected,*pCurObj;
	CDicomObject *pDicomObjSelected,*pDicomCurObj;
	CCellWnd *pCellWnd;
	POSITION pos,pos1;
	BOOL bFound;

	if (m_bPrintPreviewMode) return;

	nCurPos = GetScrollPos(SB_VERT);

	nOldPos = nCurPos;

	switch(nSBCode)
	{
	case SB_LEFT:			
		nCurPos = 0;
		break;
	case SB_RIGHT:
		nCurPos = GetScrollLimit(SB_VERT);
		break;
	case SB_LINELEFT:		
		nCurPos = max(nCurPos - 1, 0);
		break;
	case SB_LINERIGHT:		
		nCurPos = min(nCurPos + 1, GetScrollLimit(SB_VERT));
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nCurPos = nPos;
		break;
	}
			
	m_nCurrentIndex = nCurPos;

	SetScrollPos(SB_VERT,nCurPos);

	nCellWndCount = m_pCellWndList->GetCount();

	pObjSelected = NULL;
	pDicomObjSelected = NULL;

	if (m_bSeriesMode)
	{
		if (m_pCellWndSelected != NULL)
		{
			pObjSelected = m_pCellWndSelected->GetSeriesObject();
		}

		pos = m_pSeriesObjectList->FindIndex(m_nCurrentIndex + nCellWndCount);

		if (pos == NULL) pos = m_pSeriesObjectList->GetTailPosition();

		pCurObj = (CSeriesObject *) m_pSeriesObjectList->GetAt(pos);

		BindSeriesObject2CellWnd(m_nCurrentIndex,pCurObj);

		if (pObjSelected != NULL)
		{
			bFound = FALSE;
			pos = m_pCellWndList->GetHeadPosition();
			while (pos != NULL && !bFound)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetAt(pos);

				if (pCellWnd->GetSeriesObject() == pObjSelected)
				{
					m_pCellWndSelected = pCellWnd;
					bFound = TRUE;
				}

				m_pCellWndList->GetNext(pos);
			}

			if (!bFound)
			{
				if (nOldPos < m_nCurrentIndex)			
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
				}
				else
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetTail();
				}

				if (pCellWnd->GetSeriesObject()  != NULL)
				{
					m_pCellWndSelected = pCellWnd;
				}
			}
		}
	}
	else
	{

		if (m_pCellWndSelected != NULL)
		{
			pDicomObjSelected = m_pCellWndSelected->GetDicomObject();
		}

		pos = m_pDicomObjectList->FindIndex(m_nCurrentIndex + nCellWndCount);

		if (pos == NULL) pos = m_pDicomObjectList->GetTailPosition();

		pDicomCurObj = (CDicomObject *) m_pDicomObjectList->GetAt(pos);

		BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject,pDicomCurObj);

		if (pDicomObjSelected != NULL)
		{
			bFound = FALSE;
			pos = m_pCellWndList->GetHeadPosition();
			while (pos != NULL && !bFound)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetAt(pos);

				if (pCellWnd->GetDicomObject() == pDicomObjSelected)
				{
					m_pCellWndSelected = pCellWnd;
					bFound = TRUE;
				}

				m_pCellWndList->GetNext(pos);
			}

			if (!bFound)
			{
				if (nOldPos < m_nCurrentIndex)			
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
				}
				else
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetTail();
				}

				if (pCellWnd->GetDicomObject()  != NULL)
				{
					m_pCellWndSelected = pCellWnd;
				}
			}
		}
	}

	Invalidate(NULL);

//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDicomViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	POSITION pos;
	CSeriesObject *pSeriesObj;
	CDicomObject *pDicomObj;
	CCellWnd *pCellWnd;
	int nCurPos,nPos,nCurrentIndex;
	CRect rtBorder;
	BOOL bDown,bSkipNext;

	bSkipNext = FALSE;

	if (m_bPrintPreviewMode) return TRUE;

	if (((nFlags & MK_CONTROL) || (nFlags & MK_SHIFT) || (m_nLBFuncNo == 1)) && m_bSeriesMode)
	{

		if ((nFlags & MK_CONTROL) && (nFlags & MK_SHIFT))
		{

			pos = m_pCellWndList->GetHeadPosition();

			while (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				if (pCellWnd != NULL)
				{
					pSeriesObj = pCellWnd->GetSeriesObject();

					if (zDelta != 0 && pSeriesObj != NULL)
					{
						nCurrentIndex = pSeriesObj->GetCurrentIndex() + (zDelta > 0 ? - 1 : 1);

						if (nCurrentIndex < 0) nCurrentIndex = 0;

						if (nCurrentIndex >= pSeriesObj->GetDicomObjectCount()) nCurrentIndex = pCellWnd->GetSeriesObject()->GetDicomObjectCount() - 1;

						if (nCurrentIndex != pSeriesObj->GetCurrentIndex())
						{
							pSeriesObj->SetCurrentIndex(nCurrentIndex);
						
							pCellWnd->SetSeriesObject(pSeriesObj);

							rtBorder = pCellWnd->GetBorder();

							InvalidateRect(rtBorder);
						}
					}
				}
			}

		}
		else
		{

			if (m_pCellWndSelected != NULL)
			{
				pSeriesObj = m_pCellWndSelected->GetSeriesObject();

				if (zDelta != 0 && pSeriesObj != NULL)
				{
					nCurrentIndex = pSeriesObj->GetCurrentIndex() + (zDelta > 0 ? - 1 : 1);

					if (nCurrentIndex < 0) nCurrentIndex = 0;

					if (nCurrentIndex >= pSeriesObj->GetDicomObjectCount()) nCurrentIndex = m_pCellWndSelected->GetSeriesObject()->GetDicomObjectCount() - 1;

					if (nCurrentIndex != pSeriesObj->GetCurrentIndex())
					{
						pSeriesObj->SetCurrentIndex(nCurrentIndex);
						
						m_pCellWndSelected->SetSeriesObject(pSeriesObj);

						rtBorder = m_pCellWndSelected->GetBorder();

						InvalidateRect(rtBorder);
					}
				}

			}
		}

	}
	else
	{
		nCurPos = GetScrollPos(SB_VERT);
	
		if (zDelta > 0) 
		{
			nPos = nCurPos - 1;
			bDown = FALSE;
		}
		else
		{
			nPos = nCurPos + 1;
			bDown = TRUE;
		}

		if (nPos < 0) nPos = 0;
		if (nPos > m_nMaxIndex) nPos = m_nMaxIndex;

		SetScrollPos(SB_VERT,nPos);

		m_nCurrentIndex = nPos;

		if (m_bSeriesMode)
		{
			if (m_pCellWndSelected != NULL)
			{
				pSeriesObj = m_pCellWndSelected->GetSeriesObject();
			}
			else
			{
				pSeriesObj = NULL;
			}

			BindSeriesObject2CellWnd(m_nCurrentIndex,pSeriesObj);
			
			if (m_pCellWndSelected == NULL)
			{
				if (!bDown)
				{
					pos = m_pCellWndList->GetTailPosition();

					while (pos != NULL && m_pCellWndSelected == NULL)
					{
						pCellWnd = (CCellWnd *) m_pCellWndList->GetPrev(pos);

						if (pCellWnd->GetSeriesObject() != NULL)
						{
							m_pCellWndSelected = pCellWnd;
						}
					}
				}
				else
				{
					pos = m_pCellWndList->GetHeadPosition();

					while (pos != NULL && m_pCellWndSelected == NULL)
					{
						pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

						if (pCellWnd->GetSeriesObject() != NULL)
						{
							m_pCellWndSelected = pCellWnd;
						}
					}
				}
			}
		}
		else
		{
			if (m_pCellWndSelected != NULL)
			{
				pDicomObj = m_pCellWndSelected->GetDicomObject();
			}
			else
			{
				pDicomObj = NULL;
			}

			BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject,pDicomObj);
			
			if (m_pCellWndSelected == NULL)
			{
				if (!bDown)
				{
					pos = m_pCellWndList->GetTailPosition();

					while (pos != NULL && m_pCellWndSelected == NULL)
					{
						pCellWnd = (CCellWnd *) m_pCellWndList->GetPrev(pos);

						if (pCellWnd->GetDicomObject() != NULL)
						{
							m_pCellWndSelected = pCellWnd;
						}
					}
				}
				else
				{
					pos = m_pCellWndList->GetHeadPosition();

					while (pos != NULL && m_pCellWndSelected == NULL)
					{
						pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

						if (pCellWnd->GetDicomObject() != NULL)
						{
							m_pCellWndSelected = pCellWnd;
						}
					}
				}
			}
		}

		Invalidate(NULL);

		bSkipNext = TRUE;
	}

	if (!bSkipNext && m_bSeriesMode)
	{
		if (m_pCellWndSelected != NULL)
		{

			if (m_pCellWndSelected->IsCTMR() && !m_pCellWndSelected->IsLocalizer())
			{				
				pos = m_pCellWndList->GetHeadPosition();

				while (pos != NULL)
				{
					pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

					if (pCellWnd != NULL)
					{
						if (pCellWnd->IsLocalizer())
						{
							rtBorder = pCellWnd->GetBorder();

							InvalidateRect(rtBorder);							
						}
					}
				}
			}
		}

	}

	return TRUE;
}


void CDicomViewer::OnTimer(UINT nIDEvent) 
{
	CObList *pDicomObjectList = NULL;
	CSeriesObject *pSeriesObject;
	CDicomObject *pDicomObject;
	CRect rect,rtBorder;
	int nCount,nCurrentIndex, nCineMode;
	POSITION pos;
	CCellWnd *pCellWnd;

	if (nIDEvent == 1)
	{
		m_nCineEscapedTimes ++;

		if (m_nCineEscapedTimes >= m_nCineMaxTimes)
		{
			m_nCineEscapedTimes = 0;

			pos = m_pCellWndList->GetHeadPosition();

			while (pos != NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

				if (pCellWnd != NULL)
				{
					nCineMode = pCellWnd->GetCineMode();

					if (nCineMode > 0 && pCellWnd->DicomObjectIsExisting()) 
					{
						pDicomObject = pCellWnd->GetDicomObject();
						
						if (pDicomObject != NULL)
						{
							if (pDicomObject->IsMultiFrame()) 
							{
								rect = pCellWnd->GetBorder();

								pDicomObject->Cine(nCineMode);
	
								pCellWnd->MakeBitmap(FALSE);
	
								InvalidateRect(&rect);
							}
						}
					}
				}
			}

			/*

			if (m_pCellWndSelected == NULL) 
			{
				Sleep(10);
				return;
			}

			if (!m_pCellWndSelected->DicomObjectIsExisting()) 
			{
				Sleep(10);
				return;
			}
	
			pDicomObject = m_pCellWndSelected->GetDicomObject();
			if (pDicomObject == NULL) return;

			if (!pDicomObject->IsMultiFrame()) 
			{
				Sleep(10);
				return;
			}
	
			rect = m_pCellWndSelected->GetBorder();

			pDicomObject->Cine(m_nCineMode);
	
			m_pCellWndSelected->MakeBitmap(FALSE);
	
			InvalidateRect(&rect);

			*/

			Sleep(5);
		}
		else
		{
			Sleep(5);
		}
	}
	else if (nIDEvent == 2)
	{
	
		
		m_nBrowseEscapedTimes ++;

		if (m_pCellWndSelected != NULL)
		{
			pos = m_pCellWndList->Find(m_pCellWndSelected);
		}
		else
		{
			pos = NULL;
		}


		if (m_bSeriesMode)
		{
			if (m_pCellWndSelected != NULL)
			{
				pSeriesObject = m_pCellWndSelected->GetSeriesObject();

				if (pSeriesObject != NULL)
				{
					pDicomObjectList = & (pSeriesObject->m_DicomObjectList);
				}
			}
		}
		else
		{
			pDicomObjectList = m_pDicomObjectList;
		}


		if ((m_nBrowseEscapedTimes >= m_nBrowseMaxTimes) && pDicomObjectList != NULL )
		{
			m_nBrowseEscapedTimes = 0;

			if (m_bSeriesMode  && !m_pCellWndSelected->IsLocalizer())
			{
				nCount = pDicomObjectList->GetCount();

				if (nCount > 0)
				{
					nCurrentIndex = pSeriesObject->GetCurrentIndex();

					nCurrentIndex ++;

					if (nCurrentIndex >= nCount)
						nCurrentIndex = 0;


					if ( nCurrentIndex != pSeriesObject->GetCurrentIndex())
					{
						pSeriesObject->SetCurrentIndex(nCurrentIndex);
						
						m_pCellWndSelected->SetSeriesObject(pSeriesObject);				

						if (m_pCellWndSelected->IsCTMR())  // && m_pCellWndSelected->HaveLocalizer() not finished
						{
							Invalidate();
						}
						else
						{
							rtBorder = m_pCellWndSelected->GetBorder();

							InvalidateRect(rtBorder);
						}
					}
				}

			}
			else
			{
				nCount = pDicomObjectList->GetCount();

				if (nCount > 0)
				{
					m_nCurrentIndex ++;
				
					if (m_nCurrentIndex > m_nMaxIndex)
						m_nCurrentIndex = 0;

					BindDicomObject2CellWnd(m_nCurrentIndex,m_pSeriesObject);

					if (pos != NULL && m_pCellWndSelected == NULL)
					{
						m_pCellWndSelected = (CCellWnd *) m_pCellWndList->GetAt(pos);
					}

					Invalidate();
				}
			}
		}
		else
		{
			Sleep(5);
		}
	}

	//CWnd::OnTimer(nIDEvent);
}


BOOL CDicomViewer::SelectCellWnd(CPoint point)
{
	BOOL bFound,bOk;
	POSITION pos;
	CCellWnd *pCellWnd;
	
	SetFocus();

	bFound = FALSE;
	bOk = FALSE;
	pos = m_pCellWndList->GetHeadPosition();
	while (pos != NULL && !bFound)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if (pCellWnd != NULL)
		{
			if (pCellWnd->GetBorder().PtInRect(point))
			{
				if (pCellWnd->DicomObjectIsExisting())
				{
					m_pCellWndSelected = pCellWnd;
					bOk = TRUE;
				}

				bFound = TRUE;
			}
		}
	}	

	if (bOk) Invalidate();


	return bOk;
}




void CDicomViewer::ChangeSelectedWindowLevel(int nDeltaCen, int nDeltaWid)
{
	double fcen,fwid;
	CDicomObject *pDicomObject;

	if (m_pCellWndSelected != NULL)
	{
		pDicomObject = m_pCellWndSelected->GetDicomObject();

		if (pDicomObject != NULL)
		{
			pDicomObject->GetWinLevel(fcen,fwid);

			fcen = fcen + nDeltaCen;
			fwid = fwid + nDeltaWid;
		
			if (fwid < 1) fwid = 1;

			pDicomObject->SetWinLevel(fcen,fwid);

			m_pCellWndSelected->MakeBitmap(FALSE);
		}
	}
}

void CDicomViewer::ReBuildBitmap()
{
	POSITION pos;
	CCellWnd *pCellWnd;

	pos = m_pCellWndList->GetHeadPosition();

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		pCellWnd->MakeBitmap(FALSE);
	}
}

void CDicomViewer::SetZoomFactor(double fZoomFactor)
{
	POSITION pos;
	CCellWnd *pCellWnd;
	BOOL bCtrlDown;

	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000);

	pos = m_pCellWndList->GetHeadPosition();

	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown) && pCellWnd != NULL)
		{
			pCellWnd->SetZoomFactor(fZoomFactor);
			pCellWnd->MakeBitmap(FALSE);
		}
	}

	Invalidate();
}



void CDicomViewer::EnterBrowseMode()
{
	KillTimer(1);
	m_nCineMode = 0;
	m_bAutoBrowseMode = TRUE;

	SetTimer(2,15,NULL);
}

void CDicomViewer::ExitBrowseMode()
{
	m_bAutoBrowseMode = FALSE;
	KillTimer(2);
	
}


void CDicomViewer::SetCineMode(int nCineMode)
{
	CCellWnd *pCellWnd;
	int nMode;
	POSITION pos;

	m_nCineMode = nCineMode;

	pCellWnd = GetCellWndSelected();

	if (pCellWnd != NULL)
	{
		pCellWnd->SetCineMode(nCineMode);
	}

	if (nCineMode == 1 || nCineMode == 2 || nCineMode == 3)
	{
		KillTimer(2);
		SetTimer(1,10,NULL);
		m_bAutoBrowseMode = FALSE;
	}	
	else 
	{
		nMode = 0;
		pos = m_pCellWndList->GetHeadPosition();

		while (pos != NULL && nMode == 0)
		{
			pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

			if (pCellWnd != NULL)
			{
				nMode = pCellWnd->GetCineMode();
			}
		}

		if (nMode == 0)
		{
			KillTimer(1);
			m_nCineEscapedTimes = 0;
		}
	}
}


void CDicomViewer::SetCineSpeed(int nCineSpeed)
{
	double fTimes;

	if (nCineSpeed < -2 || nCineSpeed == 0) return;

	m_nCineSpeed = nCineSpeed;

	if (nCineSpeed == -1)
	{
		fTimes = (1000.0 / 0.5);
	}
	else if (nCineSpeed == -2)
	{
		fTimes = (1000.0 / 0.25);
	}
	else
	{
		fTimes = (1000.0 / nCineSpeed);
	}

	fTimes = ( fTimes/15.0 + 0.5);
	
	m_nCineMaxTimes = (int) fTimes;
	m_nBrowseMaxTimes = (int) fTimes;

}


void CDicomViewer::SaveDicomObjectToFile(const TCHAR *lpszDir)
{
	POSITION pos;
	CSeriesObject *pSeriesObject;
	CDicomObject *pDicomObject;
	CCellWnd *pCellWnd;

	pCellWnd = GetCellWndSelected();

	if (pCellWnd != NULL)
	{
		pSeriesObject = pCellWnd->GetSeriesObject();
	}
	else
	{
		pSeriesObject = GetSeriesObject();
	}

	if (pSeriesObject != NULL)
	{
		pos = pSeriesObject->m_DicomObjectList.GetHeadPosition();
		while (pos != NULL)
		{
			pDicomObject = (CDicomObject *) (pSeriesObject->m_DicomObjectList.GetNext(pos));
			if (pDicomObject != NULL)
			{
				pDicomObject->SaveFile(lpszDir);
			}
		}
	}
}


void CDicomViewer::SaveDicomObjectToFileEx(const TCHAR *lpszDir)
{
	POSITION pos;
	CSeriesObject *pSeriesObject;
	CDicomObject *pDicomObject;
	CCellWnd *pCellWnd;

	pCellWnd = GetCellWndSelected();

	if (pCellWnd != NULL)
	{
		pSeriesObject = pCellWnd->GetSeriesObject();
	}
	else
	{
		pSeriesObject = GetSeriesObject();
	}

	if (pSeriesObject != NULL)
	{
		pos = pSeriesObject->m_DicomObjectList.GetHeadPosition();
		while (pos != NULL)
		{
			pDicomObject = (CDicomObject *) (pSeriesObject->m_DicomObjectList.GetNext(pos));
			if (pDicomObject != NULL)
			{
				pDicomObject->SaveFileEx(lpszDir);
			}
		}
	}
}

void CDicomViewer::ResetAll()
{
	POSITION pos,pos1,pos2;
	CDicomObject *pDicomObject;
	CSeriesObject *pSeriesObject;
	CCellWnd *pCellWnd;
	BOOL bCtrlDown,bSeriesNeeded;
	CString sModality("");
	
	bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x8000); 	

	ExitBrowseMode();

	if (bCtrlDown)
	{
		/////////////////////////////////////////////////
		// ∏¥ŒªÀ˘”–”∞œÒµƒ…Ë÷√

		pos1 = m_pSeriesObjectList->GetHeadPosition();
		while (pos1 != NULL)
		{
			pSeriesObject = (CSeriesObject *) m_pSeriesObjectList->GetNext(pos1);

			pos2 = pSeriesObject->m_DicomObjectList.GetHeadPosition();

			while (pos2 != NULL)
			{
				pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos2);

				if (pDicomObject != NULL)
				{
					pDicomObject->Reset();
				}
			}
		}

	}
	else
	{
		/////////////////////////////////////////////////

		// ∏¥Œªµ±«∞À˘—°¥∞∂‘”¶µƒ”∞œÒµƒ…Ë÷√

		bSeriesNeeded = FALSE;

		if (m_bSeriesMode == FALSE)
		{
			pCellWnd = m_pCellWndSelected;

			if (pCellWnd == NULL)
			{
				pCellWnd = (CCellWnd *) m_pCellWndList->GetHead();
			}

			if (pCellWnd != NULL)
			{
				pDicomObject = pCellWnd->GetDicomObject();

				if (pDicomObject != NULL)
				{
					sModality = pDicomObject->GetModality();
					
					if (sModality == "CT" || sModality == "MR")
					{
						bSeriesNeeded = TRUE;
					}
				}
			}
		}

		if (m_bSeriesMode || bSeriesNeeded)
		{
			if (m_pCellWndSelected != NULL)
			{
				pSeriesObject = m_pCellWndSelected->GetSeriesObject();

				pos2 = pSeriesObject->m_DicomObjectList.GetHeadPosition();

				while (pos2 != NULL)
				{
					pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos2);

					if (pDicomObject != NULL)
					{
						pDicomObject->Reset();
					}
				}
			}
		}
		else
		{
			if (m_pCellWndSelected != NULL)
			{
				pDicomObject = m_pCellWndSelected->GetDicomObject();

				if (pDicomObject != NULL)
				{
					pDicomObject->Reset();
				}
			}
		}
	}

	pos = m_pCellWndList->GetHeadPosition();
	
	while (pos != NULL)
	{
		pCellWnd = (CCellWnd *) m_pCellWndList->GetNext(pos);

		if ((pCellWnd == m_pCellWndSelected || bCtrlDown || bSeriesNeeded) && pCellWnd != NULL)
		{
			pCellWnd->SetShift(0,0);
			pCellWnd->SetFlip(FALSE,FALSE);
			pCellWnd->SetRotateAngle(0);
			pCellWnd->SetSmoothFilterNo(0);
			pCellWnd->SetSharpFilterNo(0);
			pCellWnd->SetZoomFactor(0);
			pCellWnd->SetReversed(FALSE);
			pCellWnd->SetExtLut(FALSE,m_cExtLut);

			pCellWnd->MakeBitmap(FALSE);

			InvalidateRect(pCellWnd->GetBorder());

		}
	}
	
}


void CDicomViewer::ExportAllDicomObjectsToJpegFiles(const TCHAR *lpszDir)
{
	POSITION pos;
	CDicomObject *pDicomObject;
	CSeriesObject *pSeriesObject;
	CCellWnd *pCellSelected;

	pCellSelected = GetCellWndSelected();

	if (pCellSelected != NULL)
	{
		pSeriesObject = pCellSelected->GetSeriesObject();

		if (pSeriesObject != NULL)
		{
			pos = pSeriesObject->m_DicomObjectList.GetHeadPosition();
	
			while (pos != NULL)
			{
				pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos);

				if (pDicomObject != NULL)
				{
					pDicomObject->ExportToJpegFile(lpszDir);
				}
			}
		}
	}
}


void CDicomViewer::ExportOneDicomObjectToJpegFile(const TCHAR *lpszFileName)
{
	CDicomObject *pDicomObject;
	CCellWnd *pCellSelected;

	pCellSelected = GetCellWndSelected();

	if (pCellSelected != NULL)
	{
		pDicomObject = pCellSelected->GetDicomObject();

		if (pDicomObject != NULL)
		{
			pDicomObject->ExportToJpegFile(_T(""),lpszFileName);
		}
	}

}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void CDicomViewer::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd *pParentWnd;

	pParentWnd = GetParent();

	if (pParentWnd != NULL)
	{
		::SendMessage(pParentWnd->m_hWnd,WM_MYMESSAGE01,0,0);
	}
	
	CWnd::OnRButtonDblClk(nFlags, point);
}


