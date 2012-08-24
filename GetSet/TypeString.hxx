//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andr� Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <string>
#include <vector>

/// Get a c++ type name as string
template <typename T> inline std::string getTypeName()
{
	return typeid(T).name();
}

// Specializations
template<> inline std::string getTypeName<std::string>() {return "string";}
template<> inline std::string getTypeName<std::vector<std::string> >() {return "vector<string>";}

#define _DEFINE_TYPE(X) template<> inline std::string getTypeName<std::vector<X> >() {return "vector<"#X">";}
#include "BaseTypes.hxx"
#define _DEFINE_TYPE(X) template<> inline std::string getTypeName<X>() {return #X;}
#include "BaseTypes.hxx"
