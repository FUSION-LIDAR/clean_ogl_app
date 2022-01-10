// 3DSModelObject.h: interface for the C3DSModelObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSMODELOBJECT_H__988B1F76_A43D_451E_9082_3853756063E5__INCLUDED_)
#define AFX_3DSMODELOBJECT_H__988B1F76_A43D_451E_9082_3853756063E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C3DSModelObject  
{
public:
	int m_MaterialID;
	BOOL m_HasTexture;
	int m_FaceCount;
	int m_VertexCount;
	int m_TextureVertexCount;
	char m_strName[255];
	CArray<C3DSModel::tFace, C3DSModel::tFace&> m_FaceList;
	CArray<C3DSModel::tVector3, C3DSModel::tVector3&> m_VertexList;
	CArray<C3DSModel::tVector2, C3DSModel::tVector2&> m_TextureVertexList;
	CArray<C3DSModel::tVector3, C3DSModel::tVector3&> m_NormalList;
	void Destroy();
	BOOL IsValid();
	C3DSModelObject& operator=(C3DSModelObject& src);
	C3DSModelObject();
	virtual ~C3DSModelObject();

private:
	BOOL m_Valid;
};

#endif // !defined(AFX_3DSMODELOBJECT_H__988B1F76_A43D_451E_9082_3853756063E5__INCLUDED_)
