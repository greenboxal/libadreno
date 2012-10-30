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

#ifndef ADRENOVALUE_H
#define ADRENOVALUE_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

#include <adreno/helpers.h>

#include <unordered_map>

enum adrenogcflags
{
	/* None action should be done */
    GC_NONE = 0x00000000,

	/* The GC should free the referenced memory in the value field */
    GC_COLLECT = 0x00000001,

	/* The GC should free the pointer to the AdrenoValue struct using the MemoryPool */
    GC_FREE = 0x00000002,

	/* The GC should free the pointer to the AdrenoValue struct using AdrenoFree */
	GC_FREE_NP = 0x00000004,

	/* Used by constants, must be freed when the scripts is being freed */
	GC_FINAL_FREE = 0x00000008,
};

typedef enum
{
	/* Null Object */
    AT_NULL,

	/* Double word */
    AT_INTEGER,
	
	/* String */
    AT_STRING,

	/* Array */
    AT_ARRAY,
	
	/* Reference */
    AT_REFERENCE,
	
	/* Reference to a function */
    AT_FUNCTION,

	/* Function call return info */
    AT_RETURNINFO,
} AdrenoType;

typedef enum
{
	/* None flag associated with this string, probably a string from the string pool */
	SF_NONE = 0x00000000,

	/* The string should be freed by the GC */
	SF_FREE = 0x00000001,
} AdrenoStringFlags;

struct adrenostring
{
	AdrenoStringFlags Flags;
	char *Value;
	unsigned int Size;
};

struct adrenovmarray
{
	AdrenoType Type;
	AdrenoHashtable Array;
};

struct adrenoretvalue
{
	AdrenoScript *Script;
	AdrenoFunction *Function;
	unsigned int InstructionPointer;
	unsigned int ArgumentsPointer;
	AdrenoValue *Locals;
};

struct adrenovalue
{
	AdrenoType Type;

	AdrenoGCFlags GCFlags;
	unsigned int ReferenceCounter;

	union
	{
		unsigned int I4;
		AdrenoString *String;
		AdrenoVMArray *Array;
		AdrenoValue *Reference;
		AdrenoFunction *Function;
		AdrenoReturnInfo *ReturnInfo;
	} Value;
};

namespace Adreno
{
	enum class ValueType
	{
		Null,
		Number,
		FloatingNumber,
		Boolean,
		String,
		Object
	};

	enum class ReferenceType
	{
		Weak,
		Strong
	};

	struct ReferenceCounter
	{
		size_t StrongRefs;
		size_t WeakRefs;
	};

	class Object;
	class Value
	{
	public:
		union ValueUnion
		{
			size_t Number;
			double FloatingNumber;
			bool Boolean;
			Object *Object;
			struct
			{
				char *Data;
				size_t Size;
			} String;
		};

		Value()
		{
			Type(ValueType::Null);
		}

		Value(size_t value)
		{
			Type(ValueType::Number);
			_Values.Number = value;
		}

		Value(double value)
		{
			Type(ValueType::FloatingNumber);
			_Values.FloatingNumber = value;
		}

		Value(bool value)
		{
			Type(ValueType::Boolean);
			_Values.Boolean = value;
		}

		Value(Object *object)
		{
			Type(ValueType::Object);

			_Values.Object = object;
			if (_Values.Object)
				_Values.Object->Reference(ReferenceType::Strong);
		}

		Value(char *data, size_t size)
		{
			Type(ValueType::String);
			_Values.String.Data = data;
			_Values.String.Size = size;
		}

		~Value()
		{
			if (Type() == ValueType::Object && _Values.Object)
				_Values.Object->Dereference(ReferenceType::Strong);
		}

		void SetNull()
		{
			Type(ValueType::Null);
			DereferenceMe();
		}

		void SetValue(size_t value)
		{
			DereferenceMe();
			Type(ValueType::Number);
			_Values.Number = value;
		}

		void SetValue(double value)
		{
			DereferenceMe();
			Type(ValueType::FloatingNumber);
			_Values.FloatingNumber = value;
		}

		void SetValue(bool value)
		{
			DereferenceMe();
			Type(ValueType::Boolean);
			_Values.Boolean = value;
		}

		void SetValue(Object *object)
		{
			DereferenceMe();
			Type(ValueType::Object);

			_Values.Object = object;
			if (_Values.Object)
				_Values.Object->Reference(ReferenceType::Strong);
		}

		void SetValue(char *data, size_t size)
		{
			DereferenceMe();
			Type(ValueType::String);
			_Values.String.Data = data;
			_Values.String.Size = size;
		}

		DEFPROP_RO_RC(public, ValueUnion, Values);
		DEFPROP_RO_C(public, ValueType, Type);

	private:
		void DereferenceMe()
		{
			if (_Values.Object)
				_Values.Object->Dereference(ReferenceType::Strong);
		}
	};

	class Object;
	template<typename _Ty = Object>
	class Reference
	{
	public:
		Reference()
		{
			Reset(nullptr);
		}
		
		Reference(const Reference<_Ty> &other)
		{
			Reset(other.Value());
		}

		Reference(_Ty *pointer)
		{
			Reset(pointer);
		}

		~Reference()
		{
			if (Value() != nullptr)
				Value()->Dereference(ReferenceType::Strong);
		}

		void Reset(_Ty *pointer)
		{
			if (Value() != nullptr)
				Value()->Dereference(ReferenceType::Strong);

			Value(pointer);

			if (Value())
				Value()->Reference(ReferenceType::Strong);
		}

		_Ty &operator *()
		{
			return *Value();
		}

		_Ty *opreator ->()
		{
			return Value();
		}

		DEFPROP_RO_P(public, _Ty, Value);
	};

	class Object
	{
	public:
		typedef std::unordered_map<std::string, Reference<Object> > FieldMap;

		Object();
		virtual ~Object();

		void Reference(ReferenceType type = ReferenceType::Strong);
		void Dereference(ReferenceType type = ReferenceType::Strong);

#define DummyOp(name) virtual Value name##Op() { return Value(nullptr); }
#define DummyOp2(name) virtual Value name##Op(const Value &other) { return Value(nullptr); }

		DummyOp2(Add)
		DummyOp2(Sub)
		DummyOp2(Mult)
		DummyOp2(Div)
		DummyOp2(Rem)
		DummyOp(Neg)

		DummyOp2(And)
		DummyOp2(Or)
		DummyOp2(Xor)
		DummyOp(Not)
		DummyOp2(LeftShift)
		DummyOp2(RightShift)

		DummyOp2(LogAnd)
		DummyOp2(LogOr)
		DummyOp2(LogNot)

		DummyOp2(Equal)
		DummyOp2(NotEqual)
		DummyOp2(Greater)
		DummyOp2(GreaterEq)
		DummyOp2(Lesser)
		DummyOp2(LesserEq)
		
#undef DummyOp2
#undef DummyOp

		static Adreno::Reference<Object> CreateFromValue(const Value &value);

		DEFPROP_P_RO_R(public, FieldMap, Fields);
		DEFPROP_P_RO_R(public, ReferenceCounter, References);
	};

	class StringObject : public Object
	{
	public:
		DEFPROP_P_RO_P(public, char, Data);
		DEFPROP_P_RO_C(public, size_t, Size);
	};
}

#ifdef __cplusplus
extern "C"
{
#endif

	extern AdrenoValue *AdrenoValue_GetValue(AdrenoValue *ref);
	extern void AdrenoValue_CreateReference(AdrenoValue *ref, AdrenoValue *value);
	extern void AdrenoValue_Dereference(AdrenoValue *value);
	extern int AdrenoValue_LoadNull(AdrenoValue *value);
	extern int AdrenoValue_LoadInteger(AdrenoValue *value, unsigned int i4);
	extern int AdrenoValue_LoadString(AdrenoValue *value, char *string, unsigned int len, int copy);
	extern int AdrenoValue_LoadArray(AdrenoValue *value);
	extern void AdrenoValue_Free(AdrenoValue *value);

#ifdef __cplusplus
}
#endif

#endif
