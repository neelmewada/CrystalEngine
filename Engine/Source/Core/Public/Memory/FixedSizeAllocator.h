#pragma once

#include "Memory.h"

namespace CE
{
    class CORE_API Memory;

    template<u32 BlockSize, u32 BlockGrowCount>
    class FixedSizeFreeListAllocator
    {
    public:
        explicit FixedSizeFreeListAllocator(u32 initialBlockCount)
        {
            if (initialBlockCount > 0)
            {
                Grow(initialBlockCount);
            }
        }

        // WARNING: This will free all the memory allocated by this free list allocator. Use with caution.
        void FreeAll()
        {
            if (MallocNodeList == nullptr)
                return;

            while (MallocNodeList != nullptr)
            {
                auto ptr = MallocNodeList;
                Memory::Free(ptr->Pointer);
                MallocNodeList = MallocNodeList->NextNode;
                Memory::Free(ptr);
            }

            FreeList = nullptr;
            NumFreeListNodes = 0;
            NumLiveBlocks = 0;
        }

        // Allocates one element from the FreeList. Call Free() to return it.
        void* Allocate()
        {
            if (FreeList == nullptr) // Grow free list if needed
            {
                Grow(BlockGrowCount);
            }

            // Grab element from front of the FreeList
            u8* rawMemory = (u8*)FreeList;
            FreeList = FreeList->NextNode;
            NumLiveBlocks++;
            return rawMemory + sizeof(FreeListNode); // Each block contains the FreeListNode as header
        }

        void Free(void* block)
        {
            if (NumLiveBlocks == 0)
            {
                //LOG_ERROR("TFixedSizeAllocator<%i,%i>: Free() called on a Block that wasn't allocated", BlockSize, BlockGrowCount);
                return;
            }

            // Insert the released node to the front of the FreeList
            FreeListNode* ReleasedNode = (FreeListNode*)((u8*)block - sizeof(FreeListNode));
            ReleasedNode->NextNode = FreeList;
            FreeList = ReleasedNode;
            NumLiveBlocks--;
        }

        void Grow(u32 numBlocks)
        {
            if (numBlocks == 0)
            {
                return;
            }

            SIZE_T actualBlockSize = BlockSize + sizeof(FreeListNode);

            u8* rawMemory = (u8*)Memory::Malloc(actualBlockSize * numBlocks);
            memset(rawMemory, 0, actualBlockSize * numBlocks);

            FreeListNode* newNode = (FreeListNode*)rawMemory;
            FreeListMallocNode* mallocNode = new FreeListMallocNode;

            // Insert `MallocNode` to the front of the MallocNodeList
            mallocNode->NextNode = MallocNodeList;
            mallocNode->Pointer = rawMemory;
            MallocNodeList = mallocNode;

            // Connect all Nodes in a single linked list chain
            for (u32 i = 0; i < numBlocks - 1; ++i, newNode = newNode->NextNode)
            {
                newNode->NextNode = (FreeListNode*)(rawMemory + (i + 1) * actualBlockSize);
            }

            // Insert the chain of 'newNode' to the front of FreeList
            newNode->NextNode = FreeList;
            FreeList = (FreeListNode*)rawMemory;

            NumFreeListNodes += numBlocks;
        }

    private:
        struct FreeListNode
        {
            FreeListNode* NextNode = nullptr;
        };

        // Linked List to store all the root node pointers allocated via Malloc(). It is used when calling Free().
        struct FreeListMallocNode
        {
            void* Pointer = nullptr;
            FreeListMallocNode* NextNode = nullptr;
        };

        void ReleaseMallocList(FreeListMallocNode* rootNode)
        {
            if (rootNode == nullptr)
            {
                return;
            }

            ReleaseMallocList(rootNode->NextNode);

            rootNode->NextNode = nullptr;
            Memory::Free(rootNode->Pointer);
            delete rootNode;
            rootNode = nullptr;
        }

        FreeListNode* FreeList = nullptr;
        FreeListMallocNode* MallocNodeList = nullptr;

        u32 NumFreeListNodes = 0;

        // Number of Blocks that are currently allocated and are not present in FreeList
        u32 NumLiveBlocks = 0;
    };
}
