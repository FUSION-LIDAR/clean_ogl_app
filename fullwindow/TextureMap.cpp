// TextureMap.cpp: implementation of the CTextureMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "TextureMap.h"
#include "image.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		RAND100 (((double) LocalRand01(SCENESPECIALFX)) * 100.0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureMap::CTextureMap()
{
	m_TextureData = NULL;
	m_AlphaPresent = FALSE;
	m_HasOwnAlpha = FALSE;
	m_Valid = FALSE;
}

CTextureMap::~CTextureMap()
{
	Empty();
}

BOOL CTextureMap::LoadTextureMap(LPCTSTR lpszFileName, BOOL AddAlphaChannel, int MaxWidth, int MaxHeight, BOOL Border, BOOL ShowProgress)
{
	if (m_Valid) {
		Empty();
	}

	// attempt to load texture from file
	BOOL DoScaling = FALSE;
	CImage img;
	CSize imgsize;

	// try to read DIB
	if (img.ReadImage(lpszFileName, FALSE, ShowProgress) == TRUE) {
		// get dimensions
		imgsize = img.GetDimensions();
		m_OriginalImageSize = imgsize;

		// check image to make sure it is a valid image for texture...8-bit, 24-bit color, or 32-bit color
		if (img.m_lpBMIH->biBitCount != 8 && img.m_lpBMIH->biBitCount != 24 && img.m_lpBMIH->biBitCount != 32) {
			return(FALSE);
		}

		// no compressed images
		if (img.m_lpBMIH->biCompression != BI_RGB) {
			return(FALSE);
		}

		// make sure sizes are power of 2...if not set flag to trigger scaling
		if (!Border) {
			if (imgsize.cx != 2 &&
				imgsize.cx != 4 &&
				imgsize.cx != 8 &&
				imgsize.cx != 16 &&
				imgsize.cx != 32 &&
				imgsize.cx != 64 &&
				imgsize.cx != 128 &&
				imgsize.cx != 256 &&
				imgsize.cx != 512 &&
				imgsize.cx != 1024 &&
				imgsize.cx != 2048 &&
				imgsize.cx != 4096) {
				DoScaling = TRUE;
			}

			if (imgsize.cy != 2 &&
				imgsize.cy != 4 &&
				imgsize.cy != 8 &&
				imgsize.cy != 16 &&
				imgsize.cy != 32 &&
				imgsize.cy != 64 &&
				imgsize.cy != 128 &&
				imgsize.cy != 256 &&
				imgsize.cy != 512 &&
				imgsize.cy != 1024 &&
				imgsize.cy != 2048 &&
				imgsize.cy != 4096) {
				DoScaling = TRUE;
			}
		}
		else {
			if (imgsize.cx != 4 &&
				imgsize.cx != 6 &&
				imgsize.cx != 10 &&
				imgsize.cx != 18 &&
				imgsize.cx != 34 &&
				imgsize.cx != 66 &&
				imgsize.cx != 130 &&
				imgsize.cx != 258 &&
				imgsize.cx != 514 &&
				imgsize.cx != 1026 &&
				imgsize.cx != 2050 &&
				imgsize.cx != 4098) {
				DoScaling = TRUE;
			}

			if (imgsize.cy != 4 &&
				imgsize.cy != 6 &&
				imgsize.cy != 10 &&
				imgsize.cy != 18 &&
				imgsize.cy != 34 &&
				imgsize.cy != 66 &&
				imgsize.cy != 130 &&
				imgsize.cy != 258 &&
				imgsize.cy != 514 &&
				imgsize.cy != 1026 &&
				imgsize.cy != 2050 &&
				imgsize.cy != 4098) {
				DoScaling = TRUE;
			}
		}

		// if adding alpha channel, we need 4 bytes/pixel; otherwise 3 bytes/pixel
		if (AddAlphaChannel || img.m_lpBMIH->biBitCount == 32)
			m_TextureData = new GLubyte [imgsize.cx * imgsize.cy * 4];
		else
			m_TextureData = new GLubyte [imgsize.cx * imgsize.cy * 3];

		// make sure we got the memory
		if (!m_TextureData) {
			return(FALSE);
		}

		// set flag if 32-bit image...assume alpha is in 4th byte
		if (img.m_lpBMIH->biBitCount == 32)
			m_HasOwnAlpha = TRUE;
		else
			m_HasOwnAlpha = FALSE;

		// set member size
		m_TextureSize = imgsize;

		// set alpha flag
		m_AlphaPresent = AddAlphaChannel;
		if (img.m_lpBMIH->biBitCount == 32)
			m_AlphaPresent = TRUE;

		// set pointer to color table for 8-bit DIB
		LPRGBQUAD pDibQuad;
		if (img.m_lpBMIH->biBitCount == 8)
			pDibQuad = (LPRGBQUAD) img.m_lpvColorTable;

		// transfer pixels to BGRA or BGR format...add alpha element if flag is set
		LPBYTE ce;
		if (AddAlphaChannel || img.m_lpBMIH->biBitCount == 32) {
			// making BGRA image
			int sum;
			for (int j = 0; j < imgsize.cy; j ++) {
				ce = img.m_lpImage + (img.m_dwBytesPerScanLine * j);
				for (int i = 0; i < imgsize.cx; i ++) {
					// move pixel data adding transparency if color is black
					if (img.m_lpBMIH->biBitCount == 8) {
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 0] = pDibQuad[*ce].rgbBlue;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 1] = pDibQuad[*ce].rgbGreen;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 2] = pDibQuad[*ce].rgbRed;
						sum = pDibQuad[*ce].rgbBlue + pDibQuad[*ce].rgbGreen + pDibQuad[*ce].rgbRed;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 3] = sum ? 255 : 0;
						ce++;
					}
					else if (img.m_lpBMIH->biBitCount == 24) {
						sum = *ce;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 0] = *ce++;
						sum += *ce;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 1] = *ce++;
						sum += *ce;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 2] = *ce++;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 3] = sum ? 255 : 0;
					}
					else if (img.m_lpBMIH->biBitCount == 32) {
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 0] = *ce++;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 1] = *ce++;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 2] = *ce++;
						m_TextureData[j * imgsize.cx * 4 + i * 4 + 3] = *ce++;
					}
				}
			}
		}
		else {
			// making BGR image
			for (int j = 0; j < imgsize.cy; j ++) {
				ce = img.m_lpImage + (img.m_dwBytesPerScanLine * j);
				for (int i = 0; i < imgsize.cx; i ++) {
					// move pixel data
					if (img.m_lpBMIH->biBitCount == 8) {
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 0] = pDibQuad[*ce].rgbBlue;
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 1] = pDibQuad[*ce].rgbGreen;
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 2] = pDibQuad[*ce].rgbRed;
						ce++;
					}
					else if (img.m_lpBMIH->biBitCount == 24) {
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 0] = *ce++;
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 1] = *ce++;
						m_TextureData[j * imgsize.cx * 3 + i * 3 + 2] = *ce++;
					}
				}
			}
		}
	}

	if (m_TextureData && DoScaling) {
		// resize image to be power of 2
		m_TextureSize.cx = CalculateScaledDimension(imgsize.cx);
		m_TextureSize.cy = CalculateScaledDimension(imgsize.cy);

		if (MaxWidth && MaxHeight) {
			m_TextureSize.cx = min(MaxWidth, m_TextureSize.cx);
			m_TextureSize.cy = min(MaxHeight, m_TextureSize.cy);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		// get new memory for scaled image
		GLubyte* lpTex;
		if (AddAlphaChannel || img.m_lpBMIH->biBitCount == 32)
			lpTex = new GLubyte [m_TextureSize.cx * m_TextureSize.cy * 4];
		else
			lpTex = new GLubyte [m_TextureSize.cx * m_TextureSize.cy * 3];
		
		// check for memory and scale image
		if (lpTex) {
			if (AddAlphaChannel || img.m_lpBMIH->biBitCount == 32)
				gluScaleImage(GL_BGRA_EXT, imgsize.cx, imgsize.cy, GL_UNSIGNED_BYTE, m_TextureData, m_TextureSize.cx, m_TextureSize.cy, GL_UNSIGNED_BYTE, lpTex);
			else
				gluScaleImage(GL_BGR_EXT, imgsize.cx, imgsize.cy, GL_UNSIGNED_BYTE, m_TextureData, m_TextureSize.cx, m_TextureSize.cy, GL_UNSIGNED_BYTE, lpTex);

			GLenum errcode = glGetError();
			if (errcode == GL_NO_ERROR) {
				// delete original image data and switch pointers
				delete [] m_TextureData;
				m_TextureData = lpTex;
			}
			else {
				// couldn't scale image...clean up
				delete [] lpTex;
				Empty();
			}
		}
		else
			Empty();
	}

	m_Valid = TRUE;
	return(m_Valid);
}

BOOL CTextureMap::ActivateTextureMap(int WrapS, int WrapT, int MagFilter, int MinFilter, BOOL UseMipmaps, BOOL UseBorder)
{
	if (!m_Valid)
		return(FALSE);

	GLfloat bordercolor[] = {1.0, 1.0, 1.0, 0.0};

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) WrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) WrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) MinFilter);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);

	// set actual texture image
	if (m_AlphaPresent) {
		if (UseMipmaps) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_TextureSize.cx, m_TextureSize.cy, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_TextureData);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, 4, m_TextureSize.cx, m_TextureSize.cy, UseBorder ? 1:0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_TextureData);
		}
	}
	else {
		if (UseMipmaps) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_TextureSize.cx, m_TextureSize.cy, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_TextureData);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, 3, m_TextureSize.cx, m_TextureSize.cy, UseBorder ? 1:0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_TextureData);
		}
	}

	return(glGetError() == GL_NO_ERROR);
}

BOOL CTextureMap::IsValid()
{
	return(m_Valid);
}

void CTextureMap::Empty()
{
	if (m_Valid) {
		delete [] m_TextureData;
		m_TextureData = NULL;
		m_TextureSize.cx = 0;
		m_TextureSize.cy = 0;
	}
	m_AlphaPresent = FALSE;
	m_HasOwnAlpha = FALSE;
	m_Valid = FALSE;
}

BOOL CTextureMap::ApplyTransparentColor(COLORREF TransparentColor, BOOL ResetOpaqueAlpha, int OpaqueAlpha, int TransparentAlpha)
{
	if (!m_Valid || !m_AlphaPresent)
		return(FALSE);
	
	int index;
	int rowsize = m_TextureSize.cx * 4;
	int offset;
	for (int j = 0; j < m_TextureSize.cy; j ++) {
		offset = j * rowsize;
		for (int i = 0; i < m_TextureSize.cx; i ++) {
			index = offset + i * 4;
			if (RGB(m_TextureData[index + 2], m_TextureData[index + 1], m_TextureData[index]) == TransparentColor)
				m_TextureData[index + 3] = (GLubyte) TransparentAlpha;
			else if (ResetOpaqueAlpha)
				m_TextureData[index + 3] = (GLubyte) OpaqueAlpha;
		}
	}
	return(TRUE);
}

BOOL CTextureMap::CalculateTransparencyBasedOnIntensity(int TransparentThreshold, int OpaqueThreshold, int TransparentAlpha, int OpaqueAlpha)
{
	if (!m_TextureData || !m_AlphaPresent)
		return(FALSE);
	
	int Alpha;
	int index;
	int rowsize = m_TextureSize.cx * 4;
	int offset;
	double AlphaSpread = (double) (OpaqueAlpha - TransparentAlpha);
	double IntensitySpread = (double) (OpaqueThreshold - TransparentThreshold);
	for (int j = 0; j < m_TextureSize.cy; j ++) {
		offset = j * rowsize;
		for (int i = 0; i < m_TextureSize.cx; i ++) {
			index = offset + i * 4;
			Alpha = (int) ((double) m_TextureData[index + 2] * 0.3 + (double) m_TextureData[index + 1] * 0.59 + (double) m_TextureData[index] * 0.11);
			if (Alpha <= TransparentThreshold)
				m_TextureData[index + 3] = (GLubyte) 0;
//				m_TextureData[index + 3] = (GLubyte) TransparentAlpha;
			else if (Alpha <= OpaqueThreshold)		// interpolate
				m_TextureData[index + 3] = (GLubyte) ((double) TransparentAlpha + (((double) (Alpha - TransparentThreshold) / IntensitySpread) * AlphaSpread));
			else
				m_TextureData[index + 3] = (GLubyte) OpaqueAlpha;
		}
	}
	return(TRUE);
}

int CTextureMap::CalculateScaledDimension(int udim)
{
	int sdim;

	if (udim > 2048)
		sdim = 4096;
	else if (udim > 1024)
		sdim = 2048;
	else if (udim > 512)
		sdim = 1024;
	else if (udim > 256)
		sdim = 512;
	else if (udim > 128)
		sdim = 256;
	else if (udim > 64)
		sdim = 128;
	else if (udim > 32)
		sdim = 64;
	else if (udim > 16)
		sdim = 32;
	else if (udim > 8)
		sdim = 16;
	else if (udim > 4)
		sdim = 8;
	else if (udim > 2)
		sdim = 4;
	else if (udim > 1)
		sdim = 2;

	return(sdim);
}

void CTextureMap::OutputGlError(char* label) 
{
	GLenum errorno = glGetError() ;
	if (errorno != GL_NO_ERROR)
		TRACE("%s had error: #(%d) %s\r\n", label, errorno, gluErrorString(errorno)) ;
}

void CTextureMap::EnableTexturing(int ApplyType, BOOL AlphaBlend, float AlphaThreshold, int TextureHint)
{
	// enable texturing
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ApplyType);

	if (AlphaBlend) {
		glAlphaFunc(GL_GREATER, AlphaThreshold);
		glEnable(GL_ALPHA_TEST);
	}

	// do perspective correction on texture
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, TextureHint);
}

void CTextureMap::DisableTexturing()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

BOOL CTextureMap::SimulateSnowEffect(COLORREF SnowColor, int RandomSnow)
{
	if (!m_Valid || !m_AlphaPresent)
		return(FALSE);
	
	int sum, sumnext;
//	int randnum;
	BYTE SnowRed = GetRValue(SnowColor);
	BYTE SnowGreen = GetGValue(SnowColor);
	BYTE SnowBlue = GetBValue(SnowColor);
	int index, index2, index3;
	int i, j;
	int rowsize = m_TextureSize.cx * 4;
	int Randomtarget = RandomSnow / 3;
	for (i = 0; i < m_TextureSize.cx; i ++) {
		for (j = m_TextureSize.cy - 1; j > 1; j --) {
			index = (j * rowsize) + i * 4;
			index2 = index - rowsize;
			index3 = index2 - rowsize;
			sum = m_TextureData[index + 2] + m_TextureData[index + 1] + m_TextureData[index];

			sumnext = m_TextureData[index2 + 2] + m_TextureData[index2 + 1] + m_TextureData[index2];

			if ((sum == 0 && sumnext > 0) || (sum > 0 && j ==  m_TextureSize.cy - 1)) {
				m_TextureData[index] = SnowBlue;
				m_TextureData[index + 1] = SnowGreen;
				m_TextureData[index + 2] = SnowRed;
				m_TextureData[index + 3] = 96;

				if (j > 2) {
					m_TextureData[index2] = SnowBlue;
					m_TextureData[index2 + 1] = SnowGreen;
					m_TextureData[index2 + 2] = SnowRed;
					m_TextureData[index2 + 3] = 64;
				}
				if (j > 3) {
					m_TextureData[index3] = SnowBlue;
					m_TextureData[index3 + 1] = SnowGreen;
					m_TextureData[index3 + 2] = SnowRed;
					m_TextureData[index3 + 3] = 32;
				}
			}
/*			else if (sum > 0 && sumnext > 0) {
				// not a good idea to use "random" snow because we are changing the colored parts of the image
				// this means that when we use high alpha threshold, we will get holes in the tree that should be "green"
				randnum = (int) RAND100;
				if (randnum < Randomtarget) {
					m_TextureData[index] = SnowBlue;
					m_TextureData[index + 1] = SnowGreen;
					m_TextureData[index + 2] = SnowRed;
					m_TextureData[index + 3] = 96;
				}
				else if (randnum < Randomtarget * 2) {
					m_TextureData[index] = SnowBlue;
					m_TextureData[index + 1] = SnowGreen;
					m_TextureData[index + 2] = SnowRed;
					m_TextureData[index + 3] = 64;
				}
				else if (randnum < Randomtarget * 3) {
					m_TextureData[index] = SnowBlue;
					m_TextureData[index + 1] = SnowGreen;
					m_TextureData[index + 2] = SnowRed;
					m_TextureData[index + 3] = 32;
				}
			}
*/		}
	}
	return(TRUE);
}
