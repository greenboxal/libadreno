@echo off
rem	This file is part of libadreno.
rem
rem libadreno is free software: you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem
rem libadreno is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
rem

echo Regenerating the parser...
flex -Pail_ ail.l 
bison -p ail_ -dy ail.y
pause
