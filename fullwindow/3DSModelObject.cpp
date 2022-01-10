// 3DSModelObject.cpp: implementation of the C3DSModelObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DSModel.h"
#include "3DSModelObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSModelObject::C3DSModelObject()
{
	m_Valid = TRUE;
	m_MaterialID = 0;
	m_HasTexture = FALSE;
	m_FaceCount = 0;
	m_VertexCount = 0;
	m_TextureVertexCount = 0;
	m_strName[0] = '\0';
}

C3DSModelObject::~C3DSModelObject()
{
	Destroy();
}

BOOL C3DSModelObject::IsValid()
{
	return(m_Valid);
}

void C3DSModelObject::Destroy()
{
	if (m_Valid) {
		m_FaceList.RemoveAll();
		m_NormalList.RemoveAll();
		m_TextureVertexList.RemoveAll();
		m_VertexList.RemoveAll();

		m_MaterialID = 0;
		m_HasTexture = FALSE;
		m_FaceCount = 0;
		m_VertexCount = 0;
		m_TextureVertexCount = 0;
	}
}

C3DSModelObject& C3DSModelObject::operator=(C3DSModelObject& src)
{
	m_MaterialID = src.m_MaterialID;
	m_HasTexture = src.m_HasTexture;
	m_FaceCount = src.m_FaceCount;
	m_VertexCount = src.m_VertexCount;
	m_TextureVertexCount = src.m_TextureVertexCount;
	strcpy(m_strName, src.m_strName);
	m_FaceList.Copy(src.m_FaceList);
	m_VertexList.Copy(src.m_VertexList);
	m_TextureVertexList.Copy(src.m_TextureVertexList);
	m_NormalList.Copy(src.m_NormalList);
	m_Valid = src.m_Valid;

	return *this;
}
