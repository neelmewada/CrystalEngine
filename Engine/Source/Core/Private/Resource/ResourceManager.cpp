#include "Core.h"

namespace CE
{

	ResourceManager::ResourceManager()
	{

	}

	ResourceManager::~ResourceManager()
	{

	}

	void ResourceManager::RegisterResource(const String& moduleName, const String& pathToResource, void* data, u32 dataSizeInBytes)
	{
		String path = "/" + moduleName + "/Resources";
		if (!pathToResource.StartsWith("/"))
			path += "/";
		path += pathToResource;
		
		pathTree.AddPath(path, data, dataSizeInBytes);
	}

	void ResourceManager::DeregisterResource(const String& moduleName, const String& pathToResource)
	{
		String path = "/" + moduleName + "/Resources";
		if (!pathToResource.StartsWith("/"))
			path += "/";
		path += pathToResource;

		pathTree.RemovePath(path);
	}

	Resource* ResourceManager::LoadResource(const Name& path, Object* outer)
	{
		PathTreeNode* node = pathTree.GetNode(path);
		if (node == nullptr || node->userData == nullptr || node->userDataSize == 0)
			return nullptr;

		auto components = node->name.GetString().Split('.');
		if (components.GetSize() < 2)
			return nullptr;

		String name = "";

		for (int i = 0; i < components.GetSize() - 1; i++)
		{
			if (i == 0)
				name += components[i];
			else
				name += "_" + components[i];
		}

		name = name.Replace({ '.', ':', ' ' }, '_');

		Resource* resource = CreateObject<Resource>(outer, name);
		resource->dataSize = node->userDataSize;
		resource->data = (u8*)malloc(node->userDataSize);
		memcpy(resource->data, node->userData, node->userDataSize);
		resource->extension = components.GetLast();
		resource->fullPath = path;

		return resource;
	}

	String ResourceManager::LoadTextResource(const Name& path)
	{
		PathTreeNode* node = pathTree.GetNode(path);
		if (node == nullptr || node->userData == nullptr || node->userDataSize == 0)
			return String();

		String string = String(node->userDataSize);
		memcpy(string.GetData(), node->userData, node->userDataSize);
		string[node->userDataSize] = 0;
		string.UpdateLength();
		return string;
	}

} // namespace CE


CE_RTTI_CLASS_IMPL(CORE_API, CE, ResourceManager)
