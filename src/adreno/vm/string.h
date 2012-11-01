/*	This file is part of libadreno.

    libadreno is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libadreno is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ADRENOSTRING_H
#define ADRENOSTRING_H

#include <unordered_map>
#include <adreno/helpers.h>
#include <string.h>
#include <string>

namespace Adreno
{
	namespace StringCompare
	{
		enum
		{
			CaseSensitive,
			CaseInsensitive
		};
	}

	class Value;
	class String
	{
	public:
		struct InsensitiveHasher
		{
			size_t operator()(const String &str) const
			{
				return str.InsensitiveHash();
			}
		};

		struct InsensitiveComparer
		{
			bool operator()(const String &s1, const String &s2) const
			{
				return s1.Compare(s2, StringCompare::CaseInsensitive);
			}
		};

		String(char *str)
		{
			_Impl = new SharedImpl(str);
			_Impl->IncRef();
		}

		String(const char *str)
		{
			_Impl = SharedImpl::NewStatic(str, strlen(str));
			_Impl->IncRef();
		}

		String(const char *str, size_t size)
		{
			_Impl = SharedImpl::NewStatic(str, size);
			_Impl->IncRef();
		}

		String(char *str, size_t size)
		{
			_Impl = new SharedImpl(str, size);
			_Impl->IncRef();
		}

		String(const std::string &str)
		{
			_Impl = new SharedImpl(str);
			_Impl->IncRef();
		}
		
		String(const String &str)
		{
			_Impl = str._Impl;
			_Impl->IncRef();
		}

		~String()
		{
			_Impl->DecRef();
		}

		const char *Data() const
		{
			return _Impl->Data();
		}

		size_t Size() const
		{
			return _Impl->Size();
		}

		size_t Hash() const
		{
			return _Impl->Hash();
		}

		size_t InsensitiveHash() const
		{
			return _Impl->InsensitiveHash();
		}

		String Append(const String &other) const
		{
			return new SharedImpl(_Impl, other._Impl);
		}

		bool Compare(const String &other, int flags = StringCompare::CaseSensitive) const
		{
			return _Impl->Compare(other._Impl, flags);
		}

		static String Static(const char *str);
		static String Static(const char *str, size_t size);
		static String Sealed(size_t hash, size_t ihash);
		static String Convert(intptr_t value, int base = 10);
		static String Convert(double value);

	private:
		class SharedImpl
		{
		public:
			SharedImpl(char *str);
			SharedImpl(char *str, size_t size);
			SharedImpl(const std::string &str);
			SharedImpl(SharedImpl *s1, SharedImpl *s2);
			SharedImpl(size_t hash, size_t ihash);
			~SharedImpl();

			void IncRef();
			void DecRef();

			bool Compare(String::SharedImpl *other, int flags) const;
			
			static SharedImpl *NewStatic(const char *str, size_t size);

			DEFPROP_RO_P(public, char, Data);
			DEFPROP_RO_C(public, size_t, Size);
			DEFPROP_RO_C(public, size_t, Hash);
			DEFPROP_RO_C(public, size_t, InsensitiveHash);

		private:
			SharedImpl() { _HasHash = false; _ReferenceCount = 0; }

			int _ReferenceCount;
			bool _IsStatic;

			bool _HasHash;
			void MakeHashes();
		};

		String(SharedImpl *impl)
		{
			_Impl = impl;
			_Impl->IncRef();
		}

		SharedImpl *_Impl;

		friend class Value;
	};
}

namespace std
{
	template<>
	struct hash<Adreno::String>
	{
		size_t operator()(const Adreno::String &str) const
		{
			return str.Hash();
		}
	};

	template<>
	struct equal_to<Adreno::String>
	{
		bool operator()(const Adreno::String &s1, const Adreno::String &s2) const
		{
			return s1.Compare(s2, Adreno::StringCompare::CaseSensitive);
		}
	};
}

#endif
