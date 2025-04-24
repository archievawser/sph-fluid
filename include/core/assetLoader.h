#pragma once
#include <memory>
#include <string>
#include "fbxsdk.h"
#include "core/geometry.h"


namespace Core
{
	/// <summary>
	/// Uses the Autodesk FBX SDK (2020.3.4) to load FBX files
	/// </summary>
	class FbxUtil
	{
	public:
		FbxUtil() = delete;

	public:
		static void Init();
		static std::shared_ptr<Geometry> Load(std::string path);
		static void	PrintNode(FbxNode* node);
		static FbxMesh* FindMesh(FbxNode* root);

	private:
		static void PrintNodeInternal(FbxNode* node, int depth);

	private:
		static FbxGeometryConverter* mConverter;
		static FbxManager* mSdkManager;
		static FbxImporter* mImporter;
		static bool mIsInitialized;
	};
}