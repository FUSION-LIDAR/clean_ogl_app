// TextureMap.h: interface for the CTextureMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMAP_H__6AFD37E1_CD60_11D2_A124_FC0D08C10B02__INCLUDED_)
#define AFX_TEXTUREMAP_H__6AFD37E1_CD60_11D2_A124_FC0D08C10B02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTextureMap  
{
public:
	BOOL SimulateSnowEffect(COLORREF SnowColor, int RandomSnow);
	BOOL m_HasOwnAlpha;
	CSize m_OriginalImageSize;
	void DisableTexturing();
	void EnableTexturing(int ApplyType, BOOL AlphaBlend = TRUE, float AlphaThreshold = 0.1f, int TextureHint = GL_NICEST);
	CTextureMap();
	virtual ~CTextureMap();

	BOOL LoadTextureMap(LPCTSTR lpszFileName, BOOL AddAlphaChannel = TRUE, int MaxWidth = 0, int MaxHeight = 0, BOOL Border = FALSE, BOOL ShowProgress = FALSE);
	BOOL ApplyTransparentColor(COLORREF TransparentColor, BOOL ResetOpaqueAlpha = TRUE, int OpaqueAlpha = 255, int TransparentAlpha = 0);
	BOOL CalculateTransparencyBasedOnIntensity(int TransparentThreshold, int OpaqueThreshold, int TransparentAlpha = 0, int OpaqueAlpha = 255);
	BOOL ActivateTextureMap(int WrapS, int WrapT, int MagFilter, int MinFilter, BOOL UseMipmaps = FALSE, BOOL UseBorder = FALSE);
	void Empty();
	BOOL IsValid();

private:
	BOOL m_Valid;
	BOOL m_AlphaPresent;
	int CalculateScaledDimension(int udim);
	CSize m_TextureSize;
	GLubyte* m_TextureData;
	void OutputGlError(char* label);
};

#endif // !defined(AFX_TEXTUREMAP_H__6AFD37E1_CD60_11D2_A124_FC0D08C10B02__INCLUDED_)
