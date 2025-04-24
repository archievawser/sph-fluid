#include <memory>
#include <string>
#include "fbxsdk.h"
#include "core/geometry.h"
#include "core/assetLoader.h"
#include "utils/utils.h"


namespace Core
{
	void FbxUtil::Init()
	{
		mSdkManager = FbxManager::Create();
		mConverter = new FbxGeometryConverter(mSdkManager);
		
		mIsInitialized = true;
	}

	void FbxUtil::PrintNode(FbxNode* node)
	{
		PrintNodeInternal(node, 0);
	}

	void FbxUtil::PrintNodeInternal(FbxNode* node, int depth)
	{
		char* whitespace = new char[depth + 1];

		whitespace[depth] = '\0';
		for (int i = 0; i < depth; i++)
			whitespace[i] = '\t';

		RABID_LOG(whitespace << node->GetName());
		
		for (int childIndex = 0; childIndex < node->GetChildCount(); childIndex++)
			PrintNodeInternal(node->GetChild(childIndex), depth + 1);
	}

	std::shared_ptr<Geometry> FbxUtil::Load(std::string path)
	{
		if (!mIsInitialized)
		{
			RABID_WARNING("Attempted to load FBX file before initializing FbxUtil");
			return nullptr;
		}

		FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
		mSdkManager->SetIOSettings(ios);

		mImporter = FbxImporter::Create(mSdkManager, "");
		bool importerInitSuccess = mImporter->Initialize(path.c_str(), -1, ios);

		if (!importerInitSuccess)
		{
			RABID_WARNING("Attempted to load FBX file before initializing FbxUtil");
			return nullptr;
		}

		FbxScene* scene = FbxScene::Create(mSdkManager, "myScene");
		mImporter->Import(scene);
		mConverter->Triangulate(scene, true);
		mImporter->Destroy();

		FbxMesh* mesh = FindMesh(scene->GetRootNode());

		if (!mesh->IsTriangleMesh())
			RABID_WARNING("Failed to triangulate " << path);

		std::shared_ptr<std::vector<Vertex>> vertices (new std::vector<Vertex>());
		vertices->reserve(mesh->GetPolygonCount() * 3);

		std::shared_ptr<std::vector<uint32_t>> indices (new std::vector<uint32_t>());
		indices->reserve(mesh->GetPolygonCount() * 3);

		for (int p = 0; p < mesh->GetPolygonCount(); p++)
		{
			for (int v = 0; v < mesh->GetPolygonSize(p); v++)
			{
				FbxDouble* xyzw = mesh->GetControlPointAt(mesh->GetPolygonVertex(p, v)).mData;
				Vector3 vertexPos (xyzw[0], xyzw[1], xyzw[2]);
				vertices->emplace_back(Vertex(vertexPos, {}));
				indices->emplace_back(indices->size());
			}
		}

		RABID_LOG(mesh->GetControlPointsCount());
		RABID_LOG(mesh->GetTextureUVCount());

		return std::make_shared<Geometry>(indices, vertices);
	}

	FbxMesh* FbxUtil::FindMesh(FbxNode* currentNode)
	{
		if (currentNode->GetNodeAttribute() != nullptr && currentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			return currentNode->GetMesh();

		for (int i = 0; i < currentNode->GetChildCount(); i++)
		{
			FbxMesh* mesh = FindMesh(currentNode->GetChild(i));

			if (mesh)
				return mesh;
		}

		return nullptr;
	}

	bool FbxUtil::mIsInitialized = false;
	FbxManager* FbxUtil::mSdkManager;
	FbxImporter* FbxUtil::mImporter;
	FbxGeometryConverter* FbxUtil::mConverter;
}