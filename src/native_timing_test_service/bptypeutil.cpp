/**
 * ***** BEGIN LICENSE BLOCK *****
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * The Original Code is BrowserPlus (tm).
 * 
 * The Initial Developer of the Original Code is Yahoo!.
 * Portions created by Yahoo! are Copyright (C) 2006-2008 Yahoo!.
 * All Rights Reserved.
 * 
 * Contributor(s): 
 * ***** END LICENSE BLOCK ***** */

/**
 * bputil.hh -- c++ utilities to make building hierarchies of BPElements
 *              eaiser.  A tool that may be consumed in source form
 *              by a corelet author to simplify mapping into and out of
 *              introspectable corelet API types.
 */

#include "bptypeutil.h"

#include <assert.h>

#include <iostream>
#include <sstream>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

#ifdef WIN32
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

#define FILE_URL_PREFIX "file://"

static std::vector<std::string> 
split(const std::string& str, 
      const std::string& delim)
{
    std::vector<std::string> vsRet;
    
    unsigned int offset = 0;
    unsigned int delimIndex = 0;
    delimIndex = str.find(delim, offset);
    while (delimIndex != std::string::npos) {
        vsRet.push_back(str.substr(offset, delimIndex - offset));
        offset += delimIndex - offset + delim.length();
        delimIndex = str.find(delim, offset);
    }
    vsRet.push_back(str.substr(offset));

    return vsRet;
}

static std::string 
urlEncode(const std::string& s)
{
    std::string out;
    
    char hex[4];

    static const char noencode[] = "!'()*-._";
    static const char hexvals[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
        'A', 'B', 'C', 'D', 'E', 'F'
    };
    
    for (unsigned int i = 0; i < s.length(); i++) {
        if (isalnum(s[i]) || strchr(noencode, s[i]) != NULL) {
            out.append(&s[i], 1);
        } else {
            hex[0] = '%';
            hex[1] = hexvals[(s[i] >> 4) & 0x0F];
            hex[2] = hexvals[s[i] & 0xF];
            hex[3] = 0;
            out.append(hex, strlen(hex));
        }
    }
    return out;
}

static std::string 
urlFromPath(const std::string& s)
{
    // is this already a url?
    if (s.substr(0, strlen(FILE_URL_PREFIX)) == FILE_URL_PREFIX) {
        return s;
    }
    
    std::string delim(PATH_SEPARATOR);
    std::vector<std::string> edges = split(s, delim);
    
    std::string rval(FILE_URL_PREFIX);
    for (unsigned int i = 0; i < edges.size(); i++) {
#ifdef WIN32
        // leave DOS volumes alone
        if (i == 0 && edges[i].length() == 2 && edges[i][1] == ':') {
            rval.append("/");
            rval.append(edges[i]);
            continue;
        }
#endif
        if (edges[i].length() > 0) {
            rval.append("/");
            rval.append(urlEncode(edges[i]));
        }
    }
    return rval;
}


const char *
bp::typeAsString(BPType t)
{
    switch (t) {
        case BPTNull: return "null";
        case BPTBoolean: return "boolean";
        case BPTInteger: return "integer";
        case BPTDouble: return "double";
        case BPTString: return "string";
        case BPTMap: return "map";
        case BPTList: return "list";
        case BPTCallBack: return "callback";
        case BPTPath: return "path";
        case BPTAny: return "any";
    }
    return "unknown";
}


bp::Object::Object(BPType t)
{
    e.type = t;
}

bp::Object::~Object()
{
}

BPType bp::Object::type() const
{
    return e.type;
}

bool
bp::Object::has(const char * path, BPType type) const
{
    const bp::Object * obj = get(path);
    return ((obj != NULL) && obj->type() == type);
}

bool
bp::Object::has(const char * path) const
{
    return (get(path) != NULL);
}

void
bp::Object::attachNode(const char * path,
                       Object * node)
{
    // XXX
}

const bp::Object *
bp::Object::get(const char * path) const
{
    const Object * obj = NULL;

    if (path == NULL) return obj;
    
    std::vector<std::string> paths = split(std::string(path), "/");

    obj = this;

    for (unsigned int i = 0; i < paths.size(); i++) {
        if (obj->type() != BPTMap) {
            obj = NULL;
            break;
        }
        const Object * oldobj = obj;        
        obj = NULL;
        for (unsigned int j = 0; j < oldobj->e.value.mapVal.size; j++)
        {
            if (!paths[i].compare(oldobj->e.value.mapVal.elements[j].key))
            {
                obj = static_cast<const bp::Map *>(oldobj)->values[j];
                break;
            }
        }
        if (obj == NULL) break;
    }
    
    return obj;
}

const BPElement *
bp::Object::elemPtr() const
{
    return &e;
}

bp::Object *
bp::Object::build(const BPElement * elem)
{
    Object * obj = NULL;

    if (elem != NULL)
    {
        switch (elem->type)
        {
            case BPTNull:
                obj = new bp::Null;
                break;
            case BPTBoolean:
                obj = new bp::Bool(elem->value.booleanVal);
                break;
            case BPTInteger:
                obj = new bp::Integer(elem->value.integerVal);
                break;
            case BPTCallBack:
                obj = new bp::CallBack(elem->value.callbackVal);
                break;
            case BPTDouble:
                obj = new bp::Double(elem->value.doubleVal);
                break;
            case BPTString:
                obj = new bp::String(elem->value.stringVal);
                break;
            case BPTPath:
                obj = new bp::Path(elem->value.pathVal);
                break;
            case BPTMap:
            {
                bp::Map * m = new bp::Map;
                
                for (unsigned int i = 0; i < elem->value.mapVal.size; i++)
                {
                    m->add(elem->value.mapVal.elements[i].key,
                           build(elem->value.mapVal.elements[i].value));
                }

                obj = m;
                break;
            }
            case BPTList:
            {
                bp::List * l = new bp::List;
                
                for (unsigned int i = 0; i < elem->value.listVal.size; i++)
                {
                    l->append(build(elem->value.listVal.elements[i]));
                }

                obj = l;
                break;
            }
            case BPTAny: {
                // invalid
                break;
            }
        }
    }
    
    return obj;
}

const char *
bp::Object::getStringNodeValue( const char * cszPath )
{
    const Object* pNode = get(cszPath);
    if (!pNode || pNode->type() != BPTString)
    {
        return NULL;
    }
    
    return static_cast<const bp::String *>(pNode)->value();
}

bp::Object::operator bool() const 
{
    throw ConversionException("cannot convert to bool");
}

bp::Object::operator std::string() const 
{
    throw ConversionException("cannot convert to string");
}

bp::Object::operator long long() const 
{
    throw ConversionException("cannot convert to long");
}

bp::Object::operator double() const 
{
    throw ConversionException("cannot convert to double");
}

bp::Object::operator std::map<std::string, const bp::Object *>() const
{
    throw ConversionException("cannot convert to map<string, Object*>");
}
    

bp::Object::operator std::vector<const bp::Object *>() const
{
    throw ConversionException("cannot convert to vector<Object*>");
}

const bp::Object &
bp::Object::operator[](const char * key) const
{
    throw ConversionException("cannot apply operator[const char*]");
}

const bp::Object &
bp::Object::operator[](unsigned int index) const
{
    throw ConversionException("cannot apply operator[int]");
}

bp::Null::Null()
    : bp::Object(BPTNull) 
{
}

bp::Null::~Null()
{
}

bp::Object * 
bp::Null::clone() const
{
    return new bp::Null();
}
    
bp::Bool::Bool(bool val)
    : bp::Object(BPTBoolean) 
{
    e.value.booleanVal = val;
}

bp::Bool::~Bool()
{
}

BPBool
bp::Bool::value() const
{
    return e.value.booleanVal;
}

bp::Object * 
bp::Bool::clone() const
{
    return new bp::Bool(value());
}

bp::Bool::operator bool() const 
{
    return value();
}

bp::String::String(const char * str)
    : bp::Object(BPTString) 
{
    if (!str) str = "";
    this->str.append(str);
    e.value.stringVal = (char *) this->str.c_str();
}

bp::String::String(const char * str, unsigned int len)
    : bp::Object(BPTString) 
{
    this->str.append(str, len);
    e.value.stringVal = (char *) this->str.c_str();
}

bp::String::String(const std::string & str)
    : bp::Object(BPTString) 
{
    this->str = str;
    e.value.stringVal = (char *) this->str.c_str();
}

bp::String::String(const String & other)
    : bp::Object(BPTString)
{
    str = other.str;
    e.value.stringVal = (char *) this->str.c_str();
}

bp::String &
bp::String::operator= (const String & other)
{
    str = other.str;
    e.value.stringVal = (char *) this->str.c_str();
    return *this;
}

bp::String::~String()
{
}

const BPString
bp::String::value() const
{
    return e.value.stringVal;
}

bp::Object * 
bp::String::clone() const
{
    return new String(*this);
}

bp::String::operator std::string() const 
{
    return std::string(value());
}

bp::Path::Path(const char * str)
    : bp::String(str) 
{
    e.type = BPTPath;
    this->str = urlFromPath(this->str);
    e.value.pathVal = (char *)this->str.c_str();
}

bp::Path::Path(const char * str, unsigned int len)
    : bp::String(str, len)
{
    e.type = BPTPath;
    this->str.clear();
    this->str.append(str, len);
    this->str = urlFromPath(this->str);
    e.value.pathVal = (char *)this->str.c_str();
}

bp::Path::Path(const std::string & str)
    : bp::String(str) 
{
    e.type = BPTPath;
    this->str = urlFromPath(this->str);
    e.value.pathVal = (char *)this->str.c_str();
}

bp::Path::Path(const Path & other)
    : bp::String(other.str)
{
    e.type = BPTPath;
}

bp::Path &
bp::Path::operator= (const Path & other)
{
    str = other.str;
    e.value.pathVal = (char *) str.c_str();
    return *this;
}

bp::Path::~Path()
{
}

bp::Object * 
bp::Path::clone() const
{
    return new Path(this->str);
}

bp::Map::Map() : bp::Object(BPTMap) 
{
    e.value.mapVal.size = 0;
    e.value.mapVal.elements = NULL;
}

bp::Map::Map(const Map & o) : bp::Object(BPTMap) 
{
    e.value.mapVal.size = 0;
    e.value.mapVal.elements = NULL;

    Iterator i(o);
    const char * k;
    while (NULL != (k = i.nextKey())) {
        add(k, o.value(k)->clone());
    }
}

bp::Map &
bp::Map::operator= (const bp::Map & o)
{
    for (unsigned int i = 0; i < values.size(); i++) delete values[i];
    if (e.value.mapVal.elements != NULL) free(e.value.mapVal.elements);
    memset((void *) &e, 0, sizeof(e));
    values.clear(); keys.clear();

    e.type = BPTMap;    
    Iterator i(o);
    const char * k;
    while (NULL != (k = i.nextKey())) {
        add(k, o.value(k)->clone());
    }

    return *this;
}

bp::Object *
bp::Map::clone() const
{
    return new Map(*this);
}

bp::Map::~Map()
{
    for (unsigned int i = 0; i < values.size(); i++)
    {
        delete values[i];
    }
    
    if (e.value.mapVal.elements != NULL) 
    {
        free(e.value.mapVal.elements);
    }
}


unsigned int
bp::Map::size() const
{
    return e.value.mapVal.size;
}


const bp::Object *
bp::Map::value(const char * key) const
{
	if (key == NULL) return NULL;
    unsigned int i;
    std::vector<std::string>::const_iterator it;
    for (i = 0, it = keys.begin(); it != keys.end(); it++, i++) {
        if (!strcmp(key, (*it).c_str()))
            return values[i];
    }
    return NULL;
}

const bp::Object &
bp::Map::operator[](const char * key) const
{
    const bp::Object * v = value(key);
    if (v == NULL) {
        throw bp::ConversionException("no such element in map");
    }
    return *v;
}

bool
bp::Map::kill(const char * key)
{
    bool rval = false;
	if (key == NULL) return rval;
    std::vector<std::string>::iterator it;
    std::vector<bp::Object*>::iterator vit;
    for (it = keys.begin(), vit = values.begin();
         it != keys.end() && vit != values.end();
         ++it, ++vit) {
        if (!strcmp(key, (*it).c_str())) {
            keys.erase(it);
            values.erase(vit);
            e.value.mapVal.size--;
            rval = true;
            break;
        }
    }
    
    // if we found and removed key, rebuild BPElements
    if (rval) {
        e.value.mapVal.elements =
            (BPMapElem *) realloc(e.value.mapVal.elements,
                                  sizeof(BPMapElem) * e.value.mapVal.size);
        for (unsigned int ix = 0; ix < e.value.mapVal.size; ix++)
        {
            e.value.mapVal.elements[ix].key = (BPString) keys[ix].c_str();
            e.value.mapVal.elements[ix].value = (BPElement *) values[ix]->elemPtr();

        }
    }
    return rval;
}

void
bp::Map::add(const char * key, bp::Object * value)
{
    assert(value != NULL);
    kill(key);
    unsigned int ix = e.value.mapVal.size;
    e.value.mapVal.size++;
    values.push_back(value);
    e.value.mapVal.elements =
        (BPMapElem *) realloc(e.value.mapVal.elements,
                              sizeof(BPMapElem) * e.value.mapVal.size);
    e.value.mapVal.elements[ix].value = (BPElement *) value->elemPtr();    
    // adding a key may cause some strings to be reallocated (!).  after
    // the addition we must update key ptrs
    keys.push_back(key);
	for (ix = 0; ix < e.value.mapVal.size; ix++)
	{
		e.value.mapVal.elements[ix].key = (BPString) keys[ix].c_str();
	}
}

void
bp::Map::add(const std::string& key, bp::Object* value)
{
    add(key.c_str(), value);
}

bool
bp::Map::getBool(const std::string& sKey, bool& bValue) const
{
    if (has(sKey.c_str(), BPTBoolean)) {
        bValue = dynamic_cast<const bp::Bool*>(get(sKey.c_str()))->value();
        return true;
    }

    return false;
}

bool
bp::Map::getInteger(const std::string& sKey, int& nValue) const
{
    if (has(sKey.c_str(), BPTInteger)) {
        long long int lVal = dynamic_cast<const Integer*>(get(sKey.c_str()))->value();
        nValue = static_cast<int>(lVal);
        return true;
    }

    return false;
}
   
bool
bp::Map::getList(const std::string& sKey, const bp::List*& pList) const
{
    if (has(sKey.c_str(), BPTList)) {
        pList = dynamic_cast<const bp::List*>(get(sKey.c_str()));
        return true;
    }

    return false;
}

bool
bp::Map::getLong(const std::string& sKey, long long int& lValue) const
{
    if (has(sKey.c_str(), BPTInteger)) {
        lValue = dynamic_cast<const bp::Integer*>(get(sKey.c_str()))->value();
        return true;
    }

    return false;
}

bool
bp::Map::getMap(const std::string& sKey, const bp::Map*& pMap) const
{
    if (has(sKey.c_str(), BPTMap)) {
        pMap = dynamic_cast<const bp::Map*>(get(sKey.c_str()));
        return true;
    }

    return false;
}

bool
bp::Map::getString(const std::string& sKey, std::string& sValue) const
{
    if (has(sKey.c_str(), BPTString)) {
        sValue = dynamic_cast<const bp::String*>(get(sKey.c_str()))->value();
        return true;
    }

    return false;
}

bp::Map::Iterator::Iterator(const class bp::Map& m) {
    m_it = m.keys.begin();
    m_m = &m;
}

const char *
bp::Map::Iterator::nextKey()
{
    if (m_it == m_m->keys.end()) return NULL;
    const char * key = (*m_it).c_str();
    m_it++;
    return key;
}

bp::Map::operator std::map<std::string, const bp::Object *>() const
{
    std::map<std::string, const bp::Object *> m;
    Iterator i(*this);
    const char * k;
    while (NULL != (k = i.nextKey())) m[k] = value(k);
    return m;
    
}

bp::Integer::Integer(BPInteger num)
    : Object(BPTInteger)
{
    e.value.integerVal = num;
}

bp::Integer::~Integer()
{
}

BPInteger
bp::Integer::value() const
{
    return e.value.integerVal;
}

bp::Object * 
bp::Integer::clone() const
{
    return new Integer(*this);
}

bp::Integer::operator long long() const 
{
    return value();
}

bp::CallBack::CallBack(BPCallBack cb) : Integer(cb)
{
    e.type = BPTCallBack;
}

bp::Object * 
bp::CallBack::clone() const
{
    return new CallBack(*this);
}

bp::CallBack::~CallBack()
{
}

bp::Double::Double(BPDouble num)
    : Object(BPTDouble)
{
    e.value.doubleVal = num;
}

bp::Double::~Double()
{
}

BPDouble
bp::Double::value() const
{
    return e.value.doubleVal;
}

bp::Object *
bp::Double::clone() const
{
    return new Double(*this);
}


bp::Double::operator double() const 
{
    return value();
}

bp::List::List() : Object(BPTList)
{
    e.value.listVal.size = 0;
    e.value.listVal.elements = NULL;
}

bp::List::List(const List & other) : Object(BPTList)
{
    e.value.listVal.size = 0;
    e.value.listVal.elements = NULL;

    for (unsigned int i = 0; i < other.size(); i++) {
        append(other.value(i)->clone());
    }
}

bp::List &
bp::List::operator= (const List & other)
{
    // release
    for (unsigned int i = 0; i < values.size(); i++) delete values[i];
    if (e.value.listVal.elements != NULL) free(e.value.listVal.elements);

    // reinitialize
    memset((void *) &e, 0, sizeof(e));
    values.clear();
    e.type = BPTList;    

    // populate
    for (unsigned int i = 0; i < other.size(); i++) {
        append(other.value(i)->clone());
    }
    
    return *this;
}

bp::List::~List()
{
    for (unsigned int i = 0; i < values.size(); i++)
    {
        delete values[i];
    }
    
    if (e.value.listVal.elements != NULL) 
    {
        free(e.value.listVal.elements);
    }
}

unsigned int
bp::List::size() const
{
    return e.value.listVal.size;
}

const bp::Object *
bp::List::value(unsigned int i) const
{
    assert(e.value.listVal.size == values.size());
    if (i >= e.value.listVal.size) return NULL;
    return values[i];
}

const bp::Object &
bp::List::operator[](unsigned int index) const
{
    const bp::Object * v = value(index);
    if (v == NULL) {
        throw bp::ConversionException("no such element in list, range error");
    }
    
    return *v;
}

void
bp::List::append(bp::Object * object)
{
    assert(object != NULL);
    values.push_back(object);
    e.value.listVal.size++;
    e.value.listVal.elements = 
        (BPElement **) realloc(e.value.listVal.elements,
                               sizeof(BPElement *) * e.value.listVal.size);
    e.value.listVal.elements[e.value.listVal.size - 1] =
        (BPElement *) object->elemPtr();
}

bp::Object *
bp::List::clone() const
{
    return new bp::List(*this);
}

bp::List::operator std::vector<const bp::Object *>() const
{
    std::vector<const Object *> v;
    for (unsigned int i = 0; i < size(); i++) v.push_back(value(i));
    return v;
}
