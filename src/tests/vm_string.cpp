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

#include <UnitTest++.h>
#include <adreno/vm/string.h>
#include <string>

using namespace Adreno;

TEST(Strings)
{
	String str1 = String::Static("test STRING 1");
	String str2 = String::Static("test string 1");
		
	CHECK(str1.Compare(String::Static("test STRING 1")));
	CHECK(str1.Compare(str2, StringCompare::CaseInsensitive));

	CHECK_EQUAL(str2.InsensitiveHash(), str1.InsensitiveHash());
}
