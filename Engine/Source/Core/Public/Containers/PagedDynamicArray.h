#pragma once

namespace CE
{
    // TODO: Implement and finish the paged dynamic array container

    template<typename T, SIZE_T ElementsPerPage = 512, class Allocator = SystemAllocator>
    class PagedDynamicArray
    {
    private:

        using ValueType = T;
        using AllocatorType = Allocator;

        struct Page;

        class iterator;
        class const_iterator;

        friend iterator;
        friend const_iterator;

    public:

        constexpr static SIZE_T PageSize = ElementsPerPage * sizeof(T);

        PagedDynamicArray() = default;

        ~PagedDynamicArray();

    private:

        Page* AddPage();

        AllocatorType allocator{};

        Page* firstPage = nullptr;
        Page* firstAvailablePage = nullptr;

        SIZE_T pageCounter = 0;
        SIZE_T itemCount = 0;
    };

    template<typename T, SIZE_T ElementsPerPage, class Allocator>
    struct PagedDynamicArray<T, ElementsPerPage, Allocator>::Page
    {

        Page();

        ~Page() = default;

        Page* nextPage = nullptr;
        PagedDynamicArray* container = nullptr;
        SIZE_T pageIndex = 0;
        SIZE_T itemCount = 0;
        AlignedStorage<PageSize, alignof(T)> data;
    };

    template<typename T, SIZE_T ElementsPerPage, class Allocator>
    class PagedDynamicArray<T, ElementsPerPage, Allocator>::iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;  // or also value_type*
        using reference = T&;  // or also value_type&

        iterator(pointer ptr) : ptr(ptr) {}

    protected:

        Page* page = nullptr;
        pointer ptr = nullptr;
    };

    template <typename T, SIZE_T ElementsPerPage, class Allocator>
    PagedDynamicArray<T, ElementsPerPage, Allocator>::~PagedDynamicArray()
    {
        static constexpr SIZE_T InvalidPage = NumericLimits<SIZE_T>::Max();
        static constexpr u64 FullBits = NumericLimits<u64>::Max();
        static constexpr SIZE_T NumU64s = ElementsPerPage / 64;

        Page* page = firstPage;

        while (page != nullptr)
        {
            Page* pageToDelete = page;
            allocator.AlignedFree(pageToDelete, sizeof(Page));
            page = page->nextPage;
        }

        firstPage = firstAvailablePage = nullptr;
        pageCounter = itemCount = 0;
    }

    template <typename T, SIZE_T ElementsPerPage, class Allocator>
    typename PagedDynamicArray<T, ElementsPerPage, Allocator>::Page*
        PagedDynamicArray<T, ElementsPerPage, Allocator>::AddPage()
    {
        Page* page = (Page*)allocator.AlignedAlloc(sizeof(Page), alignof(Page));
        if (firstPage == nullptr)
            firstPage = page;
        if (firstAvailablePage == nullptr)
            firstAvailablePage = page;
        page->pageIndex = pageCounter++;
        page->container = this;
        return page;
    }

    template <typename T, SIZE_T ElementsPerPage, class Allocator>
    PagedDynamicArray<T, ElementsPerPage, Allocator>::Page::Page()
    {

    }

} // namespace CE
