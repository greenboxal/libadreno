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

#ifndef ADRENOOBJECT_H
#define ADRENOOBJECT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <adreno/helpers.h>
#include <adreno/vm/gc.h>
#include <adreno/vm/value.h>
#include <adreno/utils/memorypool.h>

namespace Adreno
{
	class Arguments
	{
	public:
		Arguments()
		{

		}

		Arguments(const Value &value)
		{
			_Args.push_back(value);
		}

		Arguments(const Value *values, size_t count)
			: _Args(values, values + count)
		{
			
		}

		Arguments(const std::vector<Value> &vec)
			: _Args(vec)
		{
		}

		size_t Count() const
		{
			return _Args.size();
		}

		Value operator[](size_t index) const
		{
			if (index > _Args.size())
				return Value();

			return _Args[index];
		}

	private:
		std::vector<Value> _Args;
	};

	class Object
	{
	public:
		typedef std::unordered_map<String, Value, String::InsensitiveHasher, String::InsensitiveComparer> FieldMap;

		Object();
		virtual ~Object();

		virtual void Construct(const Arguments &args = Arguments());
		virtual bool Destruct();

		virtual Value AddOp(const Value &value);
		virtual Value SubOp(const Value &value);
		virtual Value MultOp(const Value &value);
		virtual Value DivOp(const Value &value);
		virtual Value RemOp(const Value &value);
		virtual Value NegOp();
		
		virtual Value AndOp(const Value &value);
		virtual Value OrOp(const Value &value);
		virtual Value XorOp(const Value &value);
		virtual Value NotOp();
		virtual Value LeftShiftOp(const Value &value);
		virtual Value RightShiftOp(const Value &value);
		
		virtual Value LogAndOp(const Value &value);
		virtual Value LogOrOp(const Value &value);
		virtual Value LogNotOp();
		
		virtual Value EqualOp(const Value &value);
		virtual Value NotEqualOp(const Value &value);
		virtual Value GreaterOp(const Value &value);
		virtual Value GreaterEqOp(const Value &value);
		virtual Value LesserOp(const Value &value);
		virtual Value LesserEqOp(const Value &value);

		virtual Value Indexer(const Value &index);
		
		virtual std::int_fast32_t AsNumber() const
		{
			return 0;
		}

		virtual double AsFloatingNumber() const
		{
			return 0;
		}

		virtual bool AsBoolean() const
		{
			return false;
		}

		virtual String AsString() const
		{
			return "[object]";
		}

		virtual Value Call(const Arguments &args = Arguments());
		
#undef DummyOp2
#undef DummyOp

		Value GetField(const String &name);
		void SetField(const String &name, const Value &value);

		static Reference<Object> New();
		static Reference<Object> CreateFromValue(const Value &value);

		DEFPROP_P_RO_R(public, FieldMap, Fields);
		DEFPROP_P_RO_P(public, GCObject, GCState);
	};

	class FunctionObject : public Object
	{
	public:
		FunctionObject(std::function<Value (const Arguments &)> function)
		{
			_Function = function;
		}

		virtual Value Call(const Arguments &args = Arguments())
		{
			return _Function(args);
		}
		
		static Reference<FunctionObject> New(std::function<Value (const Arguments &)> function);

	private:
		std::function<Value (const Arguments &)> _Function;
	};

	class NumeralObject : public Object
	{
	public:
		NumeralObject(std::int_fast32_t value)
		{
			_Value = value;
		}

		virtual std::int_fast32_t AsNumber() const
		{
			return _Value;
		}

		virtual double AsFloatingNumber() const
		{
			return _Value;
		}

		virtual bool AsBoolean() const
		{
			return _Value != 0;
		}
		 
		virtual String AsString() const
		{
			static char buffer[32];

			sprintf(buffer, "%d", _Value);

			return buffer;
		}

#define BASE_OP(name, oper) virtual Value name##Op() { return Value((std::int_fast32_t)(oper _Value)); }
#define BASE_OP2(name, oper) virtual Value name##Op(const Value &value) { return Value((std::int_fast32_t)(_Value oper value.AsNumber())); }
#define BASE_OP2_BOOL(name, oper) virtual Value name##Op(const Value &value) { return Value((bool)(_Value oper value.AsNumber())); }

		BASE_OP2(Add, +)
		BASE_OP2(Sub, -)
		BASE_OP2(Mult, *)
		BASE_OP2(Div, /)
		BASE_OP2(Rem, %)
		BASE_OP(Neg, -)

		BASE_OP2(And, &)
		BASE_OP2(Or, |)
		BASE_OP2(Xor, ^)
		BASE_OP(Not, ~)
		BASE_OP2(LeftShift, <<)
		BASE_OP2(RightShift, >>)

		BASE_OP2(LogAnd, &&)
		BASE_OP2(LogOr, ||)
		BASE_OP(LogNot, !)
		
		BASE_OP2_BOOL(Equal, ==)
		BASE_OP2_BOOL(NotEqual, !=)
		BASE_OP2_BOOL(Greater, >)
		BASE_OP2_BOOL(GreaterEq, >=)
		BASE_OP2_BOOL(Lesser, <)
		BASE_OP2_BOOL(LesserEq, <=)
		
#undef BASE_OP2_BOOL
#undef BASE_OP2
#undef BASE_OP

		static Reference<NumeralObject> New(std::int_fast32_t value);

	private:
		std::int_fast32_t _Value;
	};

	class FloatingNumeralObject : public Object
	{
	public:
		FloatingNumeralObject(double value)
		{
			_Value = value;
		}

		virtual std::int_fast32_t AsNumber() const
		{
			return (std::int_fast32_t)_Value;
		}

		virtual double AsFloatingNumber() const
		{
			return _Value;
		}

		virtual bool AsBoolean() const
		{
			return _Value != 0;
		}
		 
		virtual String AsString() const
		{
			static char buffer[32];

			sprintf(buffer, "%d", _Value);

			return buffer;
		}

#define BASE_OP(name, oper) virtual Value name##Op() { return Value((double)(oper _Value)); }
#define BASE_OP2(name, oper) virtual Value name##Op(const Value &value) { return Value((double)(_Value oper value.AsFloatingNumber())); }
#define BASE_OP2_BOOL(name, oper) virtual Value name##Op(const Value &value) { return Value((bool)(_Value oper value.AsFloatingNumber())); }

		BASE_OP2(Add, +)
		BASE_OP2(Sub, -)
		BASE_OP2(Mult, *)
		BASE_OP2(Div, /)
		BASE_OP(Neg, -)

		BASE_OP2(LogAnd, &&)
		BASE_OP2(LogOr, ||)
		BASE_OP(LogNot, !)

		BASE_OP2_BOOL(Equal, ==)
		BASE_OP2_BOOL(NotEqual, !=)
		BASE_OP2_BOOL(Greater, >)
		BASE_OP2_BOOL(GreaterEq, >=)
		BASE_OP2_BOOL(Lesser, <)
		BASE_OP2_BOOL(LesserEq, <=)
		
#undef BASE_OP2_BOOL
#undef BASE_OP2
#undef BASE_OP
		
		static Reference<FloatingNumeralObject> New(double value);

	private:
		double _Value;
	};

	class BooleanObject : public Object
	{
	public:
		BooleanObject(bool value)
		{
			_Value = value;
		}

		virtual std::int_fast32_t AsNumber() const
		{
			return _Value ? 1 : 0;
		}

		virtual double AsFloatingNumber() const
		{
			return _Value ? 1 : 0;
		}

		virtual bool AsBoolean() const
		{
			return _Value;
		}

		virtual String AsString() const
		{
			return _Value ? "true" : "false";
		}

		virtual Value EqualOp(const Value &value)
		{
			return _Value == value.AsBoolean();
		}

		virtual Value NotEqualOp(const Value &value)
		{
			return _Value != value.AsBoolean();
		}

		static Reference<BooleanObject> New(bool value);

	private:
		bool _Value;
	};

	class StringObject : public Object
	{
	public:
		StringObject(const String &str)
			: _Value(str)
		{
		}

		virtual std::int_fast32_t AsNumber() const
		{
			return atoi(_Value.Data());
		}

		virtual double AsFloatingNumber() const
		{
			return atof(_Value.Data());
		}

		virtual bool AsBoolean() const
		{
			return _Value.Compare("true", StringCompare::CaseInsensitive);
		}

		virtual String AsString() const
		{
			return _Value;
		}

		virtual Value AddOp(const Value &value)
		{
			return _Value.Append(value.AsString());
		}

		virtual Value EqualOp(const Value &value)
		{
			return _Value.Compare(value.AsString());
		}

		virtual Value NotEqualOp(const Value &value)
		{
			return !_Value.Compare(value.AsString());
		}

		static Reference<StringObject> New(const String &value);

	private:
		String _Value;
	};
}

#endif
