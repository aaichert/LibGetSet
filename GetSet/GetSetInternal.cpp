#include "GetSetInternal.h"
#include "GetSetDictionary.h"
#include "GetSet.hxx"

namespace GetSetInternal {

//
// GetSetInternal::Access
//

Access::Access(GetSetDictionary& d) : dictionary(d) {}

Node* Access::getProperty(const std::string& path)
{
	if (path.empty()) return &dictionary;
	return dictionary.getProperty(stringToVector<std::string>(path,'/'),0);
}

void Access::setProperty(const std::string& path, Node* p)
{
	return dictionary.setProperty(stringToVector<std::string>(path,'/'),p,0);
}

void Access::signalChange(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyChange(section,key);
}

void Access::signalCreate(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyCreate(section,key);
}

void Access::signalDestroy(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyDestroy(section,key);
}
void Access::signalUpdateAttrib(const std::string& section, const std::string& key)
{
	typedef std::set<GetSetDictionary::Observer*>::const_iterator ObserverIterator;
	for (ObserverIterator it=dictionary.registered_observers.begin();it!=dictionary.registered_observers.end();++it)
		(*it)->notifyUpdateAttrib(section,key);
}

//
// InputOutput
//

InputOutput::InputOutput(std::istream& input, std::ostream& output) : istr(input), ostr(output) {}

void InputOutput::store(const std::string& section, const std::string& key, GetSetInternal::Node* value)
{
	if (dynamic_cast<GetSetInternal::Section*>(value)) return;
	std::string path=section.empty()?key:section+"/"+key;
	contents[path]=value->attributes;
	contents[path]["Value"]=value->getString();
	contents[path]["Type"]=value->getType();
}

void InputOutput::retreiveAll(GetSetDictionary& dictionary)
{
	for (MapStrMapStrStr::iterator it=contents.begin();it!=contents.end();++it)
	{
		std::string type=it->second["Type"];
		if (type.empty())
			GetSet<>(it->first,dictionary)=it->second["Value"];
		else
		{
			GetSetInternal::Node& p=GetSetInternal::Access::createProperty(dictionary,it->first,type);
			p.attributes=it->second;
			p.setString(p.attributes["Value"]);
			// These two are handled internally and should not be present in the attributes.
			p.attributes.erase(p.attributes.find("Type"));
			p.attributes.erase(p.attributes.find("Value"));
		}
	}
}


//
// Section
//

const Section::PropertyByName& Section::getSection() const
{
	return properties;
}

Section::Section(const std::string& path, GetSetDictionary& dict)
	: Access(dict)
	, absolutePath(path)
{}

void Section::store(InputOutput& file) const
{
	for (PropertyByName::const_iterator it=properties.begin();it!=properties.end();++it)
	{
		Section* subsection=dynamic_cast<Section*>(it->second);
		if (subsection) subsection->store(file);
		else file.store(absolutePath,it->first,it->second);
	}
}

Section::~Section()
{
	for (PropertyByName::iterator it=properties.begin();it!=properties.end();++it)
		delete it->second;
	properties.clear();
}

std::string Section::getString() const
{
	if (properties.empty()) return "<null>";
	PropertyByName::const_iterator it=properties.begin();
	std::string ret=it->first; // it->second->getType()
	for (++it;it!=properties.end();++it)
		ret+=";"+it->first; // it->second->getType()
	return ret;
}

void Section::setProperty(const std::vector<std::string>& path, Node* prop, int i)
{
	// Path up to current level HAS TO BE valid
	std::string key=path[i++];
	// Find the property at the current level of the path
	PropertyByName::iterator it=properties.find(key);
	// The path goes on after this property
	if (i!=path.size())
	{
		Node* p=0x0;
		if (it!=properties.end())
			p=it->second;
		// If the property was found it HAS TO BE a section
		Section* s=dynamic_cast<Section*>(p);
		// The property exists but has a different type => quietly erase it
		if (!s && p)
		{
			signalDestroy(absolutePath,key);
			delete p;
		}
		if (!s)
		{
			// If we just wanted to delete the property we are done.
			if (!prop)
			{
				if (it!= properties.end())
					properties.erase(it);
				return;
			}
			// The property does not exist (anymore)
			if (absolutePath.empty())
				s=new Section(key,dictionary);
			else
				s=new Section(absolutePath+"/"+key,dictionary);
			properties[key]=s;
			signalCreate(s->absolutePath,"");
		}
		// We follow down the rest of the path in Section s
		return s->setProperty(path,prop,i);
	}
	// In case we would like to delete
	if (!prop)
	{
		auto item=properties.find(key);
		if (item!=properties.end())
		{
			// Handle sub-sections individually (DFS)
			Section* section=dynamic_cast<Section*>(item->second);
			if (section)
			{
				while (!section->properties.empty())
				{
					std::vector<std::string> path_to_child(1,section->properties.begin()->first);
					section->setProperty(path_to_child, 0x0, 0);
				}
			}
			delete properties[key];
			properties.erase(item);
			signalDestroy(absolutePath,key);
		}
		// if (properties.empty()) ; // you are leaving an empty section behind.
		return;
	}
	// We have reached the end of the path, so we set the property and are done
	properties[key]=prop;
	signalCreate(absolutePath,key);
}

Node* Section::getProperty(const std::vector<std::string>& path, int i) const
{
	if (path.size()==i) return const_cast<Node*>((const Node*)this);
	if ((int)path.size()<=i) return 0x0;
	// Find the property at the current level of the path
	std::string key=path[i++];
	PropertyByName::const_iterator it=properties.find(key);
	// Property not found, so the path is invalid
	if (it==properties.end())
		return 0x0;
	// The path goes on after this property
	if (i!=path.size())
	{
		// So this property has to be a section itself
		Section* s=dynamic_cast<Section*>(it->second);
		// if it isn't, the path is invalid
		if (!s) return 0x0;
		// else follow down the rest of the path in Section s
		else return s->getProperty(path,i);
	}
	// If we are at the end of the path, we can return the property that we just found
	return it->second;
}

}// namespace GetSetInternal
