


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 963620

  This file contains the implementation for the Area class. Area is a relatively
  simple class that contains a local authority code, a container of names in
  different languages (perhaps stored in an associative container?) and a series
  of Measure objects (also in some form of container).

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>

#include "area.h"

/*
  TODO: Area::Area(localAuthorityCode)

  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area

  @example
    Area("W06000023");
*/
Area::Area(const std::string& localAuthorityCode): authorityCode(localAuthorityCode) {
  //throw std::logic_error("Area::Area() has not been implemented!");
}

/*
  TODO: Area::getLocalAuthorityCode()

  Retrieve the local authority code for this Area. This function should be 
  callable from a constant context and not modify the state of the instance.
  
  @return
    The Area's local authority code

  @example
    Area area("W06000023");
    ...
    auto authCode = area.getLocalAuthorityCode();
*/
const std::string Area::getLocalAuthorityCode() const{
	return this->authorityCode;
}

/*
  TODO: Area::getName(lang)

  Get a name for the Area in a specific language.  This function should be 
  callable from a constant context and not modify the state of the instance.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);
    ...
    auto name = area.getName(langCode);
*/
const std::string Area::getName(std::string lang) const{
	if (lang.length() != 3){
			throw std::invalid_argument("Area::getName: Language code must be three alphabetical letters only");
		}
		for (size_t i =0; i<lang.length();i++){
			if(isdigit(lang[i])){
				throw std::invalid_argument("Area::getName: Language code must be three alphabetical letters only");
			} else {
				lang[i] = (char) tolower(lang[i]);
			}
		}
	auto it = this->names.find(lang);

	if (it != this->names.end()){
		return it->second;
	} else {
		throw std::out_of_range("Could not find name of language");
	}
}

/*
  TODO: Area::setName(lang, name)

  Set a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code

  @example
    Area area("W06000023");
    std::string langCodeEnglish  = "eng";
    std::string langValueEnglish = "Powys";
    area.setName(langCodeEnglish, langValueEnglish);

    std::string langCodeWelsh  = "cym";
    std::string langValueWelsh = "Powys";
    area.setName(langCodeWelsh, langValueWelsh);
*/
void Area::setName(std::string lang, std::string name){
	if (lang.length() != 3){
		throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
	}
	for (size_t i =0; i<lang.length();i++){
		if(isdigit(lang[i])){
			throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
		} else {
			lang[i] = (char) tolower(lang[i]);
		}
	}
	auto it = names.find(lang);
	if (it != names.end()){
		names.erase(it);
	}
	names.insert({lang,name});

}

/*
  TODO: Area::getMeasure(key)

  Retrieve a Measure object, given its codename. This function should be case
  insensitive when searching for a measure.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>

  @example
    Area area("W06000023");
    Measure measure("Pop", "Population");
    area.setMeasure("Pop", measure);
    ...
    auto measure2 = area.getMeasure("pop");
*/
Measure& Area::getMeasure(std::string key){
	for (size_t i = 0; i<key.length();i++){
		key[i] = (char) tolower(key[i]);
	}
	auto it = this->measures.find(key);

	if (it != this->measures.end()){
		return it->second;
	} else {
		throw std::out_of_range("No measure found matching " + key);
	}
}

/*
  TODO: Area::setMeasure(codename, measure)

  Add a particular Measure to this Area object. Note that the Measure's
  codename should be converted to lowercase.

  If a Measure already exists with the same codename in this Area, overwrite any
  values contained within the existing Measure with those in the new Measure
  passed into this function. The resulting Measure stored inside the Area
  instance should be a combination of the two Measures instances.

  @param codename
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void

  @example
    Area area("W06000023");

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    double value = 12345678.9;
    measure.setValue(1999, value);

    area.setMeasure(codename, measure);
*/
void Area::setMeasure(std::string key, Measure measure){
	for (size_t i = 0; i<key.length();i++){
			key[i] = (char) tolower(key[i]);
	}
	auto meas = this->measures.find(key);
	if (meas==this->measures.end()){
		this->measures.insert({key,measure});
	} else {
		Measure& oldMeasure = meas->second;
		std::map<int,double> vals = measure.getValues();
		for (auto it = vals.begin(); it != vals.end();it++){
			oldMeasure.setValue(it->first,it->second);
		}
	}
}

//Returns the full list of measures for the area
std::map<std::string,Measure> Area::getMeasures() const{
	return this->measures;
}

//Returns full list of names for the area
const std::map<std::string,std::string> Area::getNames() const{
	return this->names;
}
/*
  TODO: Area::size()

  Retrieve the number of Measures we have for this Area. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The size of the Area (i.e., the number of Measures)

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    area.setMeasure(code, measure);
    auto size = area.size();
*/
const int Area::size() const noexcept {
	return this->measures.size();
}

//Will return the amount of names the area has
const int Area::namesSize() const noexcept{
	return this->names.size();
}

/*
  TODO: operator<<(os, area)

  Overload the stream output operator as a free/global function.

  Output the name of the Area in English and Welsh, followed by the local
  authority code. Then output all the measures for the area (see the coursework
  worksheet for specific formatting).

  If the Area only has only one name, output this. If the area has no names,
  output the name "Unnamed".

  Measures should be ordered by their Measure codename. If there are no measures
  output the line "<no measures>" after you have output the area names.

  See the coursework specification for more examples.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream

  @example
    Area area("W06000023");
    area.setName("eng", "Powys");
    std::cout << area << std::endl;
*/
std::ostream& operator<<(std::ostream& os, Area ar){
	os << ar.getName("eng") << " / " << ar.getName("cym") << " ("
			<< ar.getLocalAuthorityCode() << ")\n";
	if (ar.size() > 0){
		std::map<std::string,Measure> measures = ar.getMeasures();
		for (auto it = measures.begin(); it != measures.end();it++){
			os << it->second;
		}
	}else {
		os << "<no measures>\n";
	}
	return os;
}

/*
  TODO: operator==(lhs, rhs)

  Overload the == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param rhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.

  @example
    Area area1("MYCODE1");
    Area area2("MYCODE1");

    bool eq = area1 == area2;
*/
bool operator==(Area lhs, Area rhs){
	if (lhs.getLocalAuthorityCode() != rhs.getLocalAuthorityCode()){
		return false;
	}
	if (lhs.size() != rhs.size()){
		return false;
	}
	if (lhs.getMeasures() != rhs.getMeasures()){
		return false;
	}
	if (lhs.getNames() != rhs.getNames()){
		return false;
	}
	return true;
}
