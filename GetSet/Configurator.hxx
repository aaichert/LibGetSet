//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andr� Aichert (aaichert@gmail.com) and Thomas Koehler 
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

#ifndef __Configurator_h
#define __Configurator_h

#include "GetSet.hxx"

namespace ObjectFactory
{
	/// Configurator for the object factory
	class Configurator : public GetSetPath {
	private:
		Configurator(const Configurator&);
		Configurator();
	public:
		Configurator configure(const std::string& key);

	};

} // namespace ObjectFactory

#endif // __Configurator_h
