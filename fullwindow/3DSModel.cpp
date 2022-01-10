// 3DSModel.cpp: implementation of the C3DSModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "filespec.h"
#include "3DSModel.h"
#include "3DSModelObject.h"

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSModel::C3DSModel()
{
	for (int i = 0; i < MAX_TEXTURES; i ++)
		m_TextureMapIDs[i] = 0;

	m_Valid = FALSE;
	m_TexturesAreReady = FALSE;
	m_MaterialCount = 0;
	m_ObjectCount = 0;
	m_TextureMapCount = 0;
	m_ModelFile.Empty();
}

C3DSModel::~C3DSModel()
{
	Destroy();
}

BOOL C3DSModel::Load(LPCTSTR FileName, BOOL BuildTextures)
{
	char strMessage[255] = {0};

	if (m_Valid)
		Destroy();

	m_Valid = FALSE;

	// Open the 3DS file
	m_FilePointer = fopen(FileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if (m_FilePointer) {
		m_CurrentChunk = new tChunk;
		m_TempChunk = new tChunk;

		// Read the first chuck of the file to see if it's a 3DS file
		ReadChunk(m_CurrentChunk);

		// Make sure this is a 3DS file
		if (m_CurrentChunk->ID == PRIMARY) {
			// Now we actually start reading in the data.  ProcessNextChunk() is recursive

			// Begin loading objects, by calling this recursive function
			ProcessNextChunk(m_CurrentChunk);

			// After we have read the whole 3DS file, we want to calculate our own vertex normals.
			ComputeNormals();

			m_ModelFile = _T(FileName);

			// create textures
			if (BuildTextures)
				BuildModelTextures();

			m_Valid = TRUE;
		
			// scan for min max coords
			for (int i = 0; i < m_ObjectCount; i ++) {
				for (int j = 0; j < m_ObjectList[i].m_VertexCount; j ++) {
					if (i == 0 && j == 0) {
						m_Minx = m_ObjectList[i].m_VertexList[j].x;
						m_Maxx = m_ObjectList[i].m_VertexList[j].x;
						m_Miny = m_ObjectList[i].m_VertexList[j].y;
						m_Maxy = m_ObjectList[i].m_VertexList[j].y;
						m_Minz = m_ObjectList[i].m_VertexList[j].z;
						m_Maxz = m_ObjectList[i].m_VertexList[j].z;
					}
					else {
						if (m_ObjectList[i].m_VertexList[j].x < m_Minx)
							m_Minx = m_ObjectList[i].m_VertexList[j].x;
						if (m_ObjectList[i].m_VertexList[j].x > m_Maxx)
							m_Maxx = m_ObjectList[i].m_VertexList[j].x;
						if (m_ObjectList[i].m_VertexList[j].y < m_Miny)
							m_Miny = m_ObjectList[i].m_VertexList[j].y;
						if (m_ObjectList[i].m_VertexList[j].y > m_Maxy)
							m_Maxy = m_ObjectList[i].m_VertexList[j].y;
						if (m_ObjectList[i].m_VertexList[j].z < m_Minz)
							m_Minz = m_ObjectList[i].m_VertexList[j].z;
						if (m_ObjectList[i].m_VertexList[j].z > m_Maxz)
							m_Maxz = m_ObjectList[i].m_VertexList[j].z;
					}
				}
			}

//			DumpToText("object.txt");
		}
		// Clean up after everything
		fclose(m_FilePointer);						// Close the current file pointer
		delete m_CurrentChunk;						// Free the current chunk
		delete m_TempChunk;							// Free our temporary chunk
	}	

	return(m_Valid);
}

void C3DSModel::Render(int ViewMode)
{
	const int vm[] = {GL_TRIANGLES, GL_LINE_STRIP};

	if (!m_TexturesAreReady)
		BuildModelTextures();

	// Since we know how many objects our model has, go through each of them.
	for (int i = 0; i < m_ObjectCount; i++) {
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if (m_ObjectList.GetSize() <= 0) 
			break;

		if (ViewMode == POINTS) {
			if (m_ObjectList[i].m_VertexCount) {
				// Turn off texture mapping and turn on color
				glDisable(GL_TEXTURE_2D);

				// Reset the color to normal again
				if (m_ObjectList[i].m_MaterialID >= 0 && m_ObjectList[i].m_MaterialID < m_MaterialCount)
					glColor3ub(m_MaterialList[m_ObjectList[i].m_MaterialID].Color[0], m_MaterialList[m_ObjectList[i].m_MaterialID].Color[1], m_MaterialList[m_ObjectList[i].m_MaterialID].Color[2]);
				else
					glColor3ub(255, 0, 0);

				glPointSize(2.0);

				glBegin(GL_POINTS);
				for (int j = 0; j < m_ObjectList[i].m_VertexCount; j ++) {
					glVertex3f(m_ObjectList[i].m_VertexList[j].x, m_ObjectList[i].m_VertexList[j].y, m_ObjectList[i].m_VertexList[j].z);
				}
				glEnd();
			}
		}
		else {
			// Check to see if this object has a texture map, if so bind the texture to it.
			if (m_ObjectList[i].m_HasTexture && ViewMode == TRIANGLES) {

				// Turn on texture mapping and turn off color
				glEnable(GL_TEXTURE_2D);

				// Reset the color to normal again
				glColor3ub(255, 255, 255);

				// Bind the texture map to the object by it's materialID
				glBindTexture(GL_TEXTURE_2D, m_TextureMapIDs[m_MaterialList[m_ObjectList[i].m_MaterialID].TextureId]);
			} 
			else {
				// Turn off texture mapping and turn on color
				glDisable(GL_TEXTURE_2D);

				// Reset the color to normal again
				if (m_ObjectList[i].m_MaterialID >= 0 && m_ObjectList[i].m_MaterialID < m_MaterialCount)
					glColor3ub(m_MaterialList[m_ObjectList[i].m_MaterialID].Color[0], m_MaterialList[m_ObjectList[i].m_MaterialID].Color[1], m_MaterialList[m_ObjectList[i].m_MaterialID].Color[2]);
				else
					glColor3ub(255, 0, 0);
			}

			BOOL Normalize = glIsEnabled(GL_NORMALIZE);
			glEnable(GL_NORMALIZE);

			// This determines if we are in wireframe or normal mode
			glBegin(vm[ViewMode]);

				// Go through all of the faces (polygons) of the object and draw them
				for (int j = 0; j < m_ObjectList[i].m_FaceCount; j ++) {
					// Go through each corner of the triangle and draw it.
					for (int whichVertex = 0; whichVertex < 3; whichVertex ++) {
						// Get the index for each point of the face
						int index = m_ObjectList[i].m_FaceList[j].VertIndex[whichVertex];
				
						// Give OpenGL the normal for this vertex.
						glNormal3f(m_ObjectList[i].m_NormalList[index].x, m_ObjectList[i].m_NormalList[index].y, m_ObjectList[i].m_NormalList[index].z);
					
						// If the object has a texture associated with it, give it a texture coordinate.
						if (m_ObjectList[i].m_HasTexture) {

							// Make sure there was a UVW map applied to the object or else it won't have tex coords.
							if (m_ObjectList[i].m_TextureVertexCount) {
								glTexCoord2f(m_ObjectList[i].m_TextureVertexList[index].x, m_ObjectList[i].m_TextureVertexList[index].y);
							}
						}

						// Pass in the current vertex of the object (Corner of current face)
						glVertex3f(m_ObjectList[i].m_VertexList[index].x, m_ObjectList[i].m_VertexList[index].y, m_ObjectList[i].m_VertexList[index].z);
					}
				}

			glEnd();								// End the drawing

			if (!Normalize)
				glDisable(GL_NORMALIZE);
			
			if (m_ObjectList[i].m_HasTexture) {
				// Turn off texture mapping and turn off color
				glDisable(GL_TEXTURE_2D);
			}
		}
	}
}

void C3DSModel::Create()
{

}

void C3DSModel::Destroy()
{
	int i;

	// delete objects
	for (i = 0; i < m_ObjectCount; i ++) {
		m_ObjectList[i].Destroy();
	}
	m_ObjectList.RemoveAll();

	m_MaterialList.RemoveAll();

	if (m_TextureMapCount) {
		for (i = 0; i < m_TextureMapCount; i ++)
			m_TextureMaps[i].Empty();

		delete [] m_TextureMaps;
		glDeleteTextures(m_TextureMapCount, m_TextureMapIDs);
	}
	
	for (i = 0; i < MAX_TEXTURES; i ++)
		m_TextureMapIDs[i] = 0;

	m_Valid = FALSE;
	m_TexturesAreReady = FALSE;
	m_MaterialCount = 0;
	m_ObjectCount = 0;
	m_TextureMapCount = 0;
	m_ModelFile.Empty();
}

BOOL C3DSModel::IsValid()
{
	return(m_Valid);
}

int C3DSModel::GetString(char *pBuffer)
{
	int index = 0;

	// Read 1 byte of data which is the first letter of the string
	fread(pBuffer, 1, 1, m_FilePointer);

	// Loop until we get NULL
	while (*(pBuffer + index++) != 0) {

		// Read in a character at a time until we hit NULL.
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}

	// Return the string length, which is how many bytes we read in (including the NULL)
	return ((int) (strlen(pBuffer)) + 1);
}

void C3DSModel::ProcessNextChunk(tChunk *pPreviousChunk)
{
	C3DSModelObject newObject;					// This is used to add to our object list
	tMaterial newTexture = {0};					// This is used to add to our material list
	unsigned short version = 0;					// This will hold the file version
	float	scale = 1.0;						// drawing scale?
	int buffer[50000] = {0};					// This is used to read past unwanted data

	m_CurrentChunk = new tChunk;				// Allocate a new chunk				

	// Below we check our chunk ID each time we read a new chunk.  Then, if
	// we want to extract the information from that chunk, we do so.
	// If we don't want a chunk, we just read past it.  

	// Continue to read the sub chunks until we have reached the length.
	// After we read ANYTHING we add the bytes read to the chunk and then check
	// check against the length.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read next Chunk
		ReadChunk(m_CurrentChunk);

		// Check the chunk ID
		switch (m_CurrentChunk->ID)
		{
		case MAX_VERSION:							// This holds the version of the file
			
			// This chunk has an unsigned short that holds the file version.
			// Since there might be new additions to the 3DS file format in 4.0,
			// we give a warning to that problem.

			// Read the file version and add the bytes read to our bytesRead variable
			m_CurrentChunk->bytesRead += (int) fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);

			// If the file version is over 3, give a warning that there could be a problem
			if (version > 0x03)
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;

		case OBJECTINFO:						// This holds the version of the mesh
			
			// This chunk holds the version of the mesh.  It is also the head of the MATERIAL
			// and OBJECT chunks.  From here on we start reading in the material and object info.

			// Read the next chunk
			ReadChunk(m_TempChunk);

			// Get the version of the mesh
			m_TempChunk->bytesRead += (int) fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

			// Increase the bytesRead by the bytes read from the last chunk
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(m_CurrentChunk);
			break;

		case MATERIAL:							// This holds the material information

			// This chunk is the header for the material info chunks

			// Increase the number of materials
			m_MaterialCount ++;

			// Add a empty texture structure to our texture list.
			m_MaterialList.Add(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(m_CurrentChunk);
			break;

		case OBJECT:							// This holds the name of the object being read
				
			// This chunk is the header for the object info chunks.  It also
			// holds the name of the object.

			// Increase the object count
			m_ObjectCount ++;

			// Add a new tObject node to our list of objects (like a link list)
			m_ObjectList.Add(newObject);
			
			// Get the name of the object and store it, then add the read bytes to our byte counter.
			m_CurrentChunk->bytesRead += GetString(m_ObjectList[m_ObjectCount - 1].m_strName);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(&(m_ObjectList[m_ObjectCount - 1]), m_CurrentChunk);
			break;

		case EDITKEYFRAME:

			// Because I wanted to make this a SIMPLE tutorial as possible, I did not include
			// the key frame information.  This chunk is the header for all the animation info.
			// In a later tutorial this will be the subject and explained thoroughly.

			//ProcessNextKeyFrameChunk(pModel, m_CurrentChunk);

			// Read past this chunk and add the bytes read to the byte counter
			m_CurrentChunk->bytesRead += (int) fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MASTERSCALE:
			// Read the file version and add the bytes read to our bytesRead variable
			m_CurrentChunk->bytesRead += (int) fread(&scale, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		default: 
			
			// If we didn't care about a chunk, then we get here.  We still need
			// to read past the unknown or ignored chunk and add the bytes read to the byte counter.
			m_CurrentChunk->bytesRead += (int) fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void C3DSModel::ProcessNextMaterialChunk(tChunk *pPreviousChunk)
{
	int buffer[50000] = {0};					// This is used to read past unwanted data

	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length) {
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read in
		switch (m_CurrentChunk->ID)
		{
		case MATNAME:							// This chunk holds the name of the material
			
			// Here we read in the material name
			m_CurrentChunk->bytesRead += (int) fread(m_MaterialList[m_MaterialCount - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:						// This holds the R G B color of our object
			ReadColorChunk(&(m_MaterialList[m_MaterialCount - 1]), m_CurrentChunk);
			break;
		
		case MATMAP:							// This is the header for the texture info
			
			// Proceed to read in the material information
			ProcessNextMaterialChunk(m_CurrentChunk);
			break;

		case MATMAPFILE:						// This stores the file name of the material

			// Here we read in the material's file name
			m_CurrentChunk->bytesRead += (int) fread(m_MaterialList[m_MaterialCount - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		
		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += (int) fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void C3DSModel::ProcessNextObjectChunk(C3DSModelObject *pObject, tChunk *pPreviousChunk)
{
	int buffer[50000] = {0};					// This is used to read past unwanted data

	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read
		switch (m_CurrentChunk->ID)
		{
		case OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pObject, m_CurrentChunk);
			break;

		case OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case OBJECT_MATERIAL:				// This holds the material name that the object has
			
			// This chunk holds the name of the material that the object has assigned to it.
			// This could either be just a color or a texture map.  This chunk also holds
			// the faces that the texture is assigned to (In the case that there is multiple
			// textures assigned to one object, or it just has a texture on a part of the object.
			// Since most of my game objects just have the texture around the whole object, and 
			// they aren't multitextured, I just want the material name.

			// We now will read the name of the material assigned to this object
			ReadObjectMaterial(pObject, m_CurrentChunk);			
			break;

		case OBJECT_UV:						// This holds the UV texture coordinates for the object

			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		case OBJECT_AXIS:
			float val[6];
			m_CurrentChunk->bytesRead += (int) fread(val, sizeof(float), 6, m_FilePointer) * sizeof(float);

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += (int) fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			
			break;

		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += (int) fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

void C3DSModel::ReadChunk(tChunk *pChunk)
{
	// This reads the chunk ID which is 2 bytes.
	// The chunk ID is like OBJECT or MATERIAL.  It tells what data is
	// able to be read in within the chunks section.  
	pChunk->bytesRead = (int) fread(&pChunk->ID, 1, 2, m_FilePointer);

	// Then, we read the length of the chunk which is 4 bytes.
	// This is how we know how much to read in, or read past.
	pChunk->bytesRead += (int) fread(&pChunk->length, 1, 4, m_FilePointer);
}

void C3DSModel::ReadColorChunk(tMaterial *pMaterial, tChunk *pChunk)
{
	// Read the color chunk info
	ReadChunk(m_TempChunk);

	// Read in the R G B color (3 bytes - 0 through 255)
	m_TempChunk->bytesRead += (int) fread(pMaterial->Color, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

	// Add the bytes read to our chunk
	pChunk->bytesRead += m_TempChunk->bytesRead;
}

void C3DSModel::ReadObjectMaterial(C3DSModelObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};			// This is used to hold the objects material name
	int buffer[50000] = {0};				// This is used to read past unwanted data

	// *What is a material?*  - A material is either the color or the texture map of the object.
	// It can also hold other information like the brightness, shine, etc... Stuff we don't
	// really care about.  We just want the color, or the texture map file name really.

	// Here we read the material name that is assigned to the current object.
	// strMaterial should now have a string of the material name, like "Material #2" etc..
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Now that we have a material name, we need to go through all of the materials
	// and check the name against each material.  When we find a material in our material
	// list that matches this name we just read in, then we assign the materialID
	// of the object to that material index.  You will notice that we passed in the
	// model to this function.  This is because we need the number of textures.
	// Yes though, we could have just passed in the model and not the object too.

	// Go through all of the textures
	for (int i = 0; i < m_MaterialCount; i++) {
		// If the material we just read in matches the current texture name
		if (strcmp(strMaterial, m_MaterialList[i].strName) == 0) {
			// Set the material ID to the current index 'i' and stop checking
			pObject->m_MaterialID = i;

			// Now that we found the material, check if it's a texture map.
			// If the strFile has a string length of 1 and over it's a texture
			if(strlen(m_MaterialList[i].strFile) > 0) {
				// Set the object's flag to say it has a texture map to bind.
				pObject->m_HasTexture = true;
			}	
			break;
		}
	}

	// Read past the rest of the chunk since we don't care about shared vertices
	// You will notice we subtract the bytes already read in this chunk from the total length.
	pPreviousChunk->bytesRead += (int) fread(buffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}

void C3DSModel::ReadUVCoordinates(C3DSModelObject* pObject, tChunk *pPreviousChunk)
{
	// In order to read in the UV indices for the object, we need to first
	// read in the amount there are, then read them in.

	// Read in the number of UV coordinates there are (int)
	pPreviousChunk->bytesRead += (int) fread(&pObject->m_TextureVertexCount, 1, 2, m_FilePointer);

	// Allocate memory to hold the UV coordinates
	pObject->m_TextureVertexList.SetSize(pObject->m_TextureVertexCount);

	// Read in the texture coodinates (an array 2 float)
	float x, y;
	for (int i = 0; i < pObject->m_TextureVertexCount; i ++) {
		pPreviousChunk->bytesRead += (int) fread(&x, 1, sizeof(float), m_FilePointer);
		pPreviousChunk->bytesRead += (int) fread(&y, 1, sizeof(float), m_FilePointer);
		pObject->m_TextureVertexList[i].x = x;
		pObject->m_TextureVertexList[i].y = y;
	}
}

void C3DSModel::ReadVertexIndices(C3DSModelObject* pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;					// This is used to read in the current face index

	// In order to read in the vertex indices for the object, we need to first
	// read in the number of them, then read them in.  Remember,
	// we only want 3 of the 4 values read in for each face.  The fourth is
	// a visibility flag for 3D Studio Max that doesn't mean anything to us.

	// Read in the number of faces that are in this object (int)
	pPreviousChunk->bytesRead += (int) fread(&pObject->m_FaceCount, 1, 2, m_FilePointer);

	// Alloc enough memory for the faces and initialize the structure
	pObject->m_FaceList.SetSize(pObject->m_FaceCount);

	// Go through all of the faces in this object
	for (int i = 0; i < pObject->m_FaceCount; i++) {
		// Next, we read in the A then B then C index for the face, but ignore the 4th value.
		// The fourth value is a visibility flag for 3D Studio Max, we don't care about this.
		for (int j = 0; j < 4; j++) {
			// Read the first vertice index for the current face 
			pPreviousChunk->bytesRead += (int) fread(&index, 1, sizeof(index), m_FilePointer);

			if (j < 3) {
				// Store the index in our face structure.
				pObject->m_FaceList[i].VertIndex[j] = index;
			}
		}
	}
}

void C3DSModel::ReadVertices(C3DSModelObject* pObject, tChunk *pPreviousChunk)
{
	// Like most chunks, before we read in the actual vertices, we need
	// to find out how many there are to read in.  Once we have that number
	// we then fread() them into our vertice array.

	// Read in the number of vertices (int)
	pPreviousChunk->bytesRead += (int) fread(&(pObject->m_VertexCount), 1, 2, m_FilePointer);

	// Allocate the memory for the verts and initialize the structure
	pObject->m_VertexList.SetSize(pObject->m_VertexCount);

	// Read in the array of vertices (an array of 3 floats)
	float x, y, z;
	for (int i = 0; i < pObject->m_VertexCount; i ++) {
		pPreviousChunk->bytesRead += (int) fread(&x, 1, sizeof(float), m_FilePointer);
		pPreviousChunk->bytesRead += (int) fread(&y, 1, sizeof(float), m_FilePointer);
		pPreviousChunk->bytesRead += (int) fread(&z, 1, sizeof(float), m_FilePointer);
		pObject->m_VertexList[i].x = x;
		pObject->m_VertexList[i].y = y;
		pObject->m_VertexList[i].z = z;
	}
}

// This adds 2 vectors together and returns the result
void C3DSModel::AddVector(tVector3 vVector1, tVector3 vVector2, tVector3& vResult)
{
//	tVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

//	return vResult;								// Return the resultant vector
}

// This calculates a vector between 2 points and returns the result
void C3DSModel::Vector(tVector3 vPoint1, tVector3 vPoint2, tVector3& vVector)
{
	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's
}

// This divides a vector by a single number (scalar) and returns the result
void C3DSModel::DivideVectorByScaler(tVector3 vVector1, float Scaler, tVector3& vResult)
{
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler
}

// This returns the cross product between 2 vectors
void C3DSModel::Cross(tVector3 vVector1, tVector3 vVector2, tVector3& vCross)
{
	// Get the X value
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	// Get the Y value
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	// Get the Z value
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));
}

// This returns the normal of a vector
void C3DSModel::Normalize(tVector3& vNormal)
{
	double Magnitude;							// This holds the magitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude
}

void C3DSModel::ComputeNormals()
{
	int i;
	tVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if (m_ObjectCount <= 0)
		return;

	// What are vertex normals?  And how are they different from other normals?
	// Well, if you find the normal to a triangle, you are finding a "Face Normal".
	// If you give OpenGL a face normal for lighting, it will make your object look
	// really flat and not very round.  If we find the normal for each vertex, it makes
	// the smooth lighting look.  This also covers up blocky looking objects and they appear
	// to have more polygons than they do.    Basically, what you do is first
	// calculate the face normals, then you take the average of all the normals around each
	// vertex.  It's just averaging.  That way you get a better approximation for that vertex.

	// Go through each of the objects to calculate their normals
	for (int index = 0; index < m_ObjectCount; index ++) {
		// Get the current object
		C3DSModelObject& pObject = m_ObjectList[index];

		// Here we allocate all the memory we need to calculate the normals
		tVector3 *pNormals		= new tVector3 [pObject.m_FaceCount];
		tVector3 *pTempNormals	= new tVector3 [pObject.m_FaceCount];
		pObject.m_NormalList.SetSize(pObject.m_VertexCount);

		// Go though all of the faces of this object
		for (i = 0; i < pObject.m_FaceCount; i++) {												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject.m_VertexList[pObject.m_FaceList[i].VertIndex[0]];
			vPoly[1] = pObject.m_VertexList[pObject.m_FaceList[i].VertIndex[1]];
			vPoly[2] = pObject.m_VertexList[pObject.m_FaceList[i].VertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

			Vector(vPoly[0], vPoly[2], vVector1);		// Get the vector of the polygon (we just need 2 sides for the normal)
			Vector(vPoly[2], vPoly[1], vVector2);		// Get a second vector of the polygon

			Cross(vVector1, vVector2, vNormal);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			Normalize(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}

		//////////////// Now Get The Vertex Normals /////////////////

		tVector3 vSum = {0.0, 0.0, 0.0};
		tVector3 vZero = vSum;
		int shared = 0;

		// Go through all of the vertices
		for (i = 0; i < pObject.m_VertexCount; i ++) {
			// Go through all of the triangles
			for (int j = 0; j < pObject.m_FaceCount; j++) {
				// Check if the vertex is shared by another face
				if (pObject.m_FaceList[j].VertIndex[0] == i || pObject.m_FaceList[j].VertIndex[1] == i || pObject.m_FaceList[j].VertIndex[2] == i) {
					AddVector(vSum, pTempNormals[j], vSum);// Add the un-normalized normal of the shared face
					shared ++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			DivideVectorByScaler(vSum, float(-shared), pObject.m_NormalList[i]);

			// Normalize the normal for the final vertex normal
			Normalize(pObject.m_NormalList[i]);	

			vSum = vZero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
	
		// Free our memory and start over on the next object
		delete [] pTempNormals;
		delete [] pNormals;
	}
}


void C3DSModel::BuildModelTextures()
{
	int i;

	// count the materials that use texture maps
	m_TextureMapCount = 0;
	for (i = 0; i < m_MaterialCount; i++) {
		// Check to see if there is a file name to load in this material
		if (strlen(m_MaterialList[i].strFile) > 0) {
			m_TextureMapCount ++;
		}
	}

	// create array for texture maps
	if (m_TextureMapCount) {
		m_TextureMaps = new CTextureMap[m_TextureMapCount];

		CFileSpec ModelFileSpec(m_ModelFile);
		CFileSpec TextureFileSpec;
		// Go through all the materials
		for (i = 0; i < m_MaterialCount; i++) {
			// Check to see if there is a file name to load in this material
			if (strlen(m_MaterialList[i].strFile) > 0) {
				TextureFileSpec.SetFullSpec(m_MaterialList[i].strFile);
				// assume texture files are located with model file
				TextureFileSpec.SetDirectory(ModelFileSpec.Directory());
				// Use the name of the texture file to load the bitmap, with a texture ID (i)
				if (m_TextureMaps[i].LoadTextureMap(TextureFileSpec.GetFullSpec(), FALSE)) {
					glGenTextures(1, &m_TextureMapIDs[i]);
					glBindTexture(GL_TEXTURE_2D, m_TextureMapIDs[i]);

					// use nearest pixel when magnifying the texture to prevent blending of transparent color and alpha values
					// near the edge of the tree image
					m_TextureMaps[i].ActivateTextureMap(GL_CLAMP, GL_CLAMP, GL_NEAREST, GL_NEAREST, FALSE);
					m_MaterialList[i].TextureId = i;
				}
				else
					m_MaterialList[i].TextureId = 0;
			}
		}
	}
	m_TexturesAreReady = TRUE;
}

void C3DSModel::DumpToText(LPCTSTR FileName)
{
	int i;

	FILE* f;
	f = fopen(FileName, "wt");

	if (f) {
		// dump model info
		fprintf(f, "Min coord: %.4f, %.4f, %.4f\n", m_Minx, m_Miny, m_Minz);
		fprintf(f, "Max coord: %.4f, %.4f, %.4f\n", m_Maxx, m_Maxy, m_Maxz);
		// dump materials
		for (i = 0; i < m_MaterialCount; i ++) {
			fprintf (f, "Material %i\n", i + 1);
			fprintf(f, "\tName: %s\n", m_MaterialList[i].strName);
			fprintf(f, "\tFile name: %s\n", m_MaterialList[i].strFile);
			fprintf(f, "\tColor: R-%3i G-%3i B-%3i\n", m_MaterialList[i].Color[0], m_MaterialList[i].Color[1], m_MaterialList[i].Color[2]);
			fprintf(f, "\tTexture offset: %.4f, %.4f\n", m_MaterialList[i].UOffset, m_MaterialList[i].VOffset);
			fprintf(f, "\tTexture tiling flags: %f %f\n\n", m_MaterialList[i].UTile, m_MaterialList[i].VTile);
		}

		// dump objects
		for (i = 0; i < m_ObjectCount; i ++) {
			fprintf(f, "Object %i\n", i + 1);
			fprintf(f, "\tName: %s\n", m_ObjectList[i].m_strName);
			if (m_MaterialCount)
				fprintf(f, "\tMaterial: %s\n", m_MaterialList[m_ObjectList[i].m_MaterialID].strName);
			fprintf(f, "\tTexture: %i\n", m_ObjectList[i].m_HasTexture);
			fprintf(f, "\tNumber of faces: %i\n", m_ObjectList[i].m_FaceCount);
			fprintf(f, "\tNumber of vertices: %i\n", m_ObjectList[i].m_VertexCount);
			fprintf(f, "\tNumber of texture vertices: %i\n\n", m_ObjectList[i].m_TextureVertexCount);
		}
		fclose(f);
	}
}

void C3DSModel::TranslateToModelCenter(BOOL UseBaseZ)
{
	double cx = (m_Minx + m_Maxx) / 2.0;
	double cy = (m_Miny + m_Maxy) / 2.0;
	double cz = (m_Minz + m_Maxz) / 2.0;

	if (UseBaseZ)
		cz = m_Minz;

	glTranslated(-cx, -cy, -cz);

}

void C3DSModel::DrawBoundingBox()
{
	glColor3ub(192, 192, 192);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_Minx, m_Miny, m_Minz);
	glVertex3f(m_Maxx, m_Miny, m_Minz);
	glVertex3f(m_Maxx, m_Maxy, m_Minz);
	glVertex3f(m_Minx, m_Maxy, m_Minz);
	glVertex3f(m_Minx, m_Miny, m_Minz);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_Minx, m_Miny, m_Maxz);
	glVertex3f(m_Maxx, m_Miny, m_Maxz);
	glVertex3f(m_Maxx, m_Maxy, m_Maxz);
	glVertex3f(m_Minx, m_Maxy, m_Maxz);
	glVertex3f(m_Minx, m_Miny, m_Maxz);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(m_Minx, m_Miny, m_Minz);
	glVertex3f(m_Minx, m_Miny, m_Maxz);
	glVertex3f(m_Maxx, m_Miny, m_Minz);
	glVertex3f(m_Maxx, m_Miny, m_Maxz);
	glVertex3f(m_Maxx, m_Maxy, m_Minz);
	glVertex3f(m_Maxx, m_Maxy, m_Maxz);
	glVertex3f(m_Minx, m_Maxy, m_Minz);
	glVertex3f(m_Minx, m_Maxy, m_Maxz);
	glEnd();
}

void C3DSModel::ScaleToUnitCube()
{
	double w = m_Maxx - m_Minx;
	double l = m_Maxy - m_Miny;
	double h = m_Maxz - m_Minz;
	double s = max(w, l);
	s = max(s, h);
	s = 1.0 / s;

	glScaled(s, s, s);
}
