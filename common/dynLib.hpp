#ifndef HEBY_DYNLIB_HPP
#define HEBY_DYNLIB_HPP

#include <memory>
#include <iostream>
#if _WIN32
#include <Windows.h>
#else
#include <dlfcn.h> //dlopen
#endif

namespace heby
{

	template <class T>
	class DynLib
	{
	public:
		DynLib() = default;
		~DynLib()
		{
			if (handle_)
				closeLib(handle_);
		};

		bool open(const std::string &filename, const char *csym = "create", const char *dsym = "destroy")
		{

			if (!(handle_ = openLib(filename)))
				return printLibError();
			if (!(create_ = (T * (*)()) loadSymbol(handle_, csym)))
				return printLibError();
			if (!(destroy_ = (void(*)(T *))loadSymbol(handle_, dsym)))
				return printLibError();
			return true;
		}

		T* create()
		{
			return (T*)create_();
		}

	private:
		bool printLibError()
		{
			#if _WIN32
				//std::cerr << GetLastError() << std::endl;
			#else
				//std::cerr << dlerror() << std::endl;
			#endif
			return false;
		}

		void *openLib(const std::string &libName)
		{
			#if _WIN32
				return LoadLibraryA((libName + ".dll").c_str());
			#else
				return dlopen((libName + ".so").c_str(), RTLD_LAZY);
			#endif
		}

		int closeLib(void *libHandle)
		{
			#if _WIN32
				return FreeLibrary((HMODULE)libHandle);
			#else
				return dlclose(libHandle);
			#endif
		}

		void *loadSymbol(void *libHandle, const char *sym)
		{
			#if _WIN32
				return (void *)GetProcAddress((HMODULE)libHandle, sym);
			#else
				return dlsym(libHandle, sym);
			#endif
		}

		void *handle_{ nullptr };
		T *(*create_)();
		void(*destroy_)(T *);
	};
}


#endif //HEBY_DYNLIB_HPP