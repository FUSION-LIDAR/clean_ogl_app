// 3DSModel.h: interface for the C3DSModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSMODEL_H__744EA0CD_6711_4A07_A6C0_F2876502A6D7__INCLUDED_)
#define AFX_3DSMODEL_H__744EA0CD_6711_4A07_A6C0_F2876502A6D7__INCLUDED_

#include "TextureMap.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_TEXTURES 100								// The maximum amount of textures to load
#define	TRIANGLES		0
#define	LINES			1
#define	POINTS			2

//>------ Primary Chunk, at the beginning of each file
#define PRIMARY       0x4D4D

//>------ Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define MAX_VERSION   0x0002				// This gives the version of the .3ds file
#define EDITKEYFRAME  0xB000				// This is the header for all of the key frame info
#define MASTERSCALE	  0x0100

//>------ sub defines of OBJECTINFO
#define MATERIAL	  0xAFFF				// This stored the texture info
#define OBJECT		  0x4000				// This stores the faces, vertices, etc...

//>------ sub defines of MATERIAL
#define MATNAME       0xA000				// This holds the material name
#define MATDIFFUSE    0xA020				// This holds the color of the object/material
#define MATMAP        0xA200				// This is a header for a new material
#define MATMAPFILE    0xA300				// This holds the file name of the texture

#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

//>------ sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// The objects vertices
#define OBJECT_FACES		0x4120			// The objects faces
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color
#define OBJECT_UV			0x4140			// The UV texture coordinates
#define	OBJECT_AXIS			0x4160			// local axis definition

class C3DSModel  
{
	friend class C3DSModelObject;

public:
	// 3DS indicies (.3DS stores 4 unsigned shorts)
	struct tIndices {							
		unsigned short a, b, c, bVisible;		// This will hold point1, 2, and 3 index's into the vertex array plus a visible flag
	};

	// chunk info
	struct tChunk {
		unsigned short int ID;					// The chunk's ID		
		unsigned int length;					// The length of the chunk
		unsigned int bytesRead;					// The amount of bytes read within that chunk
	};

	// 3D point
	struct tVector3 {
		float x, y, z;
	};

	// 2D point
	struct tVector2 {
		float x, y;
	};

	// face
	struct tFace {
		int VertIndex[3];			// indicies for the verts that make up this triangle
		int CoordIndex[3];			// indicies for the tex coords to texture this face
	};

	// material
	struct tMaterial {
		char  strName[255];			// The texture name
		char  strFile[255];			// The texture file name (If this is set it's a texture map)
		BYTE  Color[3];				// The color of the object (R, G, B)
		int   TextureId;			// the texture ID
		float UTile;				// u tiling of texture  (Currently not used)
		float VTile;				// v tiling of texture	(Currently not used)
		float UOffset;			    // u offset of texture	(Currently not used)
		float VOffset;				// v offset of texture	(Currently not used)
	};


public:
	void ScaleToUnitCube();
	void DrawBoundingBox();
	void TranslateToModelCenter(BOOL UseBaseZ = TRUE);
	void DumpToText(LPCTSTR FileName);
	int m_MaterialCount;
	float m_Minx, m_Miny, m_Minz;
	float m_Maxx, m_Maxy, m_Maxz;
	CArray<tMaterial, tMaterial&> m_MaterialList;
	BOOL IsValid();
	void Render(int ViewMode = TRIANGLES);
	BOOL Load(LPCTSTR FileName, BOOL BuildTextures = FALSE);
	void Create();
	void Destroy();
	C3DSModel();
	virtual ~C3DSModel();

private:
	CString m_ModelFile;
	BOOL m_TexturesAreReady;
	int m_TextureMapCount;
	void BuildModelTextures();
	CTextureMap* m_TextureMaps;
	UINT m_TextureMapIDs[MAX_TEXTURES];
	void ReadVertices(C3DSModelObject* pObject, tChunk* pPreviousChunk);
	void ReadVertexIndices(C3DSModelObject* pObject, tChunk* pPreviousChunk);
	void ReadUVCoordinates(C3DSModelObject* pObject, tChunk* pPreviousChunk);
	void ReadObjectMaterial(C3DSModelObject* pObject, tChunk* pPreviousChunk);
	void ReadColorChunk(tMaterial* pMaterial, tChunk* pChunk);
	void ReadChunk(tChunk* pChunk);
	
	void ProcessNextObjectChunk(C3DSModelObject* pObject, tChunk* pPreviousChunk);
	void ProcessNextMaterialChunk(tChunk* pPreviousChunk);
	void ProcessNextChunk(tChunk* pPreviousChunk);
	int GetString(char *pBuffer);
	
	void AddVector(tVector3 vVector1, tVector3 vVector2, tVector3& vResult);
	void Vector(tVector3 vPoint1, tVector3 vPoint2, tVector3& vResult);
	void DivideVectorByScaler(tVector3 vVector1, float Scaler, tVector3& vResult);
	void Cross(tVector3 vVector1, tVector3 vVector2, tVector3& vResult);
	void Normalize(tVector3& vNormal);
	void ComputeNormals();
	
	FILE* m_FilePointer;
	tChunk* m_TempChunk;
	tChunk* m_CurrentChunk;
	int m_ObjectCount;
	CArray<C3DSModelObject, C3DSModelObject&> m_ObjectList;
	BOOL m_Valid;
};

#endif // !defined(AFX_3DSMODEL_H__744EA0CD_6711_4A07_A6C0_F2876502A6D7__INCLUDED_)
