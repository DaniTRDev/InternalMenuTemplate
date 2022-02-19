#pragma once

/*	---ReadRel---

offset == num of bytes to ignore ex:

48 89 05 B2 99 8B 04  mov     cs:qword_144F87D00, rax

The first 3 bytes are to identify that we are going to move rax to the qword,
only the last four are the proper relative addr of the qword. They are stored in 
a std::uint64_t, but it's splitted in two std::uint32_ts like this:
{ std::uint32_t Addr, std::uint32_t CurrentInstruction,  }
so the full relative Addr is (Addr + CurrentInstruction), by this we only want to read the first 32bits (4bytes)
that are the relative addr itself, because the CurrentInstruction is currently stored in our m_Ptr as an std::uint64_t.
This means we only need to read the Addr and add it to our current m_Ptr and then add 4 to skip the four bytes of the 
Addr of the qword.
*/

namespace change_me
{
	class PointerMath
	{
	public:

		inline PointerMath(std::uintptr_t Ptr) : m_Ptr(reinterpret_cast<void*>(Ptr))
			{}

		inline PointerMath Add(std::size_t Offset)
		{
			return PointerMath(std::uintptr_t(m_Ptr) + Offset);
		}
		inline PointerMath Sub(std::size_t Offset)
		{
			return PointerMath(std::uintptr_t(m_Ptr) - Offset);
		}

		template<typename T>
		inline std::enable_if_t<std::is_same<T, std::uintptr_t>::value, std::uintptr_t> As() /*only enable if the T is a std::uintptr_t*/
		{
			return std::uintptr_t(m_Ptr);
		}
		
		template<typename T>
		inline std::enable_if_t<std::is_pointer<T>::value, T> As() /*we only want to enable this in case it's a ptr,
															otherwise we will crash*/
		{
			return reinterpret_cast<T>(m_Ptr);
		}

		inline PointerMath ReadRel(std::size_t Offset) 
		{
			auto CurAddr = Add(Offset); /*skip the bytes that are jut to identify the instruction*/
			auto RelAddr = *CurAddr.As<std::int32_t*>();

			return Add(Offset).Add(RelAddr).Add(4);
		}

	private:

		void* m_Ptr;

	};
}