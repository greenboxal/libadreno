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

#ifndef AIL_H
#define AIL_H

#include <iostream>
#include <unordered_map>
#include <adreno/helpers.h>
#include <adreno/vm/emit.h>

namespace Adreno
{
	class AilParserContext 
	{
	public:
		AilParserContext(std::istream &in);
		~AilParserContext();
	
		std::istream &Input()
		{
			return _Input;
		}

		void SetDebugInfo(bool debugInfo)
		{
			_DebugInfo = debugInfo;
		}

		AssemblyBuilder &GetAssemblyBuilder()
		{
			return _Assembly;
		}

		DEFPROP_RO_P(public, void, Scanner);

	private:
		std::istream &_Input;
		bool _DebugInfo;

		AssemblyBuilder _Assembly;
		FunctionEmitter *_CurrentFunction;

		std::unordered_map<String, Label *> _Labels;

		friend class APC_Detail;
	};
}

int AILC_parse(Adreno::AilParserContext *context);

#endif
