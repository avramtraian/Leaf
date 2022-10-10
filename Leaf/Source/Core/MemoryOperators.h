// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  YOU SHOULD NEVER INCLUDE THIS FILE  //////////////////////////////////////
////////////////////  ALL MEMORY OPERATORS DECLARATIONS CAN BE ACCESSED FROM 'Core/Memory.h'  ////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* operator new (size_t size)
{
	return Leaf::Memory::Allocate(size);
}

void* operator new (size_t size, const char* file, const char* function, Leaf::uint32 line)
{
	return Leaf::Memory::AllocateTagged(size, file, function, line);
}

// NOTE (Avr): In Debug configuration, VMA also defines this operator, causing a linking error.
#if !LF_CONFIGURATION_DEBUG || !LF_BUILD_CORE_LIBRARY
void* operator new (size_t, void* address) noexcept
{
	return address;
}
#endif

void operator delete(void* block)
{
	return Leaf::Memory::Free(block);
}