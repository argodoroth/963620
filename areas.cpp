


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 963620

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <sstream>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"
#include "measure.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  TODO: Areas::Areas()

  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() {
  //throw std::logic_error("Areas::Areas() has not been implemented!");
}

/*
  TODO: Areas::setArea(localAuthorityCode, area)

  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
*/

void Areas::setArea(std::string code, Area area){
	auto ar = areas.find(code);
	if (ar!=areas.end()){

		Area& oldArea = ar->second;
		std::map<std::string, Measure> measures = area.getMeasures();
		for (auto it = measures.begin(); it != measures.end();it++){
			oldArea.setMeasure(it->first,it->second);
		}
		std::map<std::string,std::string> names = area.getNames();
		for (auto it2 = names.begin(); it2 != names.end();it2++){
			oldArea.setName(it2->first,it2->second);
		}
	} else {
		areas.insert({code,area});
	}
}
/*
  TODO: Areas::getArea(localAuthorityCode)

  Retrieve an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    ...
    Area area2 = areas.getArea("W06000023");
*/
Area& Areas::getArea(std::string code) {
	auto ar = this->areas.find(code);
	if (ar != this->areas.end()){
		return ar->second;
	} else {
		throw std::out_of_range("No area found matching " + code);
	}
}

AreasContainer Areas::getAreas(){
	return this->areas;
}
/*
  TODO: Areas::size()

  Retrieve the number of Areas within the container. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
const int Areas::size() const noexcept{
	return this->areas.size();
}

/*
  TODO: Areas::populateFromAuthorityCodeCSV(is, cols, areasFilter)

  This function specifically parses the compiled areas.csv file of local
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  For this coursework, you can assume that areas.csv will always have the same
  three columns in the same order.

  Once the data is parsed, you need to create the appropriate Area objects and
  insert them in to a Standard Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/areas.csv");
    auto is = input.open();

    auto cols = InputFiles::AREAS.COLS;

    auto areasFilter = BethYw::parseAreasArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter) {
	std::string english = "eng";
	std::string welsh = "cym";

	std::string line;
	std::vector<std::string> data;
	std::getline(is,line);

	while(std::getline(is,line)){
		int i = 0;
		std::string areaCode;
		std::stringstream lineStream(line);
		while (std::getline(lineStream, line, ',')){
			if (i == 0) {
				areaCode = line;
				Area ar(line);
				setArea(line,ar);
			} else if (i == 1){
				Area& ar = getArea(areaCode);
				ar.setName(english,line);
			} else if (i ==2){
				Area& ar = getArea(areaCode);
				ar.setName(welsh,line);
			}
			i++;
		}
	}
}

/*
  TODO: Areas::populateFromWelshStatsJSON(is,
                                          cols,
                                          areasFilter,
                                          measuresFilter,
                                          yearsFilter)

  Data from StatsWales is in the JSON format, and contains three
  top-level keys: odata.metadata, value, odata.nextLink. value contains the
  data we need. Rather than been hierarchical, it contains data as a
  continuous list (e.g. as you would find in a table). For each row in value,
  there is a mapping of various column headings and their respective vaues.

  Therefore, you need to go through the items in value (in a loop)
  using a JSON library. To help you, I've selected the nlohmann::json
  library that you must use for your coursework. Read up on how to use it here:
  https://github.com/nlohmann/json

  Example of using this library:
    - Reading/parsing in from a stream is very simply using the >> operator:
        json j;
        stream >> j;

    - Looping through parsed JSON is done with a simple for each loop. Inside
      the loop, you can access each using the array syntax, with the key/
      column name, e.g. data["Localauthority_ItemName_ENG"] gives you the
      local authority name:
        for (auto& el : j["value"].items()) {
           auto &data = el.value();
           std::string localAuthorityCode = data["Localauthority_ItemName_ENG"];
           // do stuff here...
        }

  In this function, you will have to parse the JSON datasets, extracting
  the local authority code, English name (the files only contain the English
  names), and each measure by year.

  If you encounter an Area that does not exist in the Areas container, you
  should create the Area object

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  I've provided the column names for each JSON file that you need to parse
  as std::strings in datasets.h. This mapping should be passed through to the
  cols parameter of this function.

  Note that in the JSON format, years are stored as strings, but we need
  them as ints. When retrieving values from the JSON library, you will
  have to cast them to the right type.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populateFromWelshStatsJSON(std::istream &is,
		const BethYw::SourceColumnMapping &cols,
		const StringFilterSet * const areasFilter,
		const StringFilterSet * const measuresFilter,
		const YearFilterTuple * const yearsFilter){
	//opens json stream
	json j;
	is >> j;
	std::string measureCode;
	std::string measureLabel;
	if (cols.count(BethYw::SINGLE_MEASURE_CODE)>0){
		 measureCode = cols.at(BethYw::MEASURE_CODE);
		 measureLabel = cols.at(BethYw::MEASURE_NAME);
	}

		//gets all values from json
	for (auto& el : j["value"].items()) {
	   auto &data = el.value();
	   //gets all needed values from the JSON
	   std::string localAuthorityCode = data[cols.at(BethYw::AUTH_CODE)];
	   std::string localAuthorityName = data[cols.at(BethYw::AUTH_NAME_ENG)];
	   measureCode = data[cols.at(BethYw::MEASURE_CODE)];
	   measureLabel = data[cols.at(BethYw::MEASURE_NAME)];
	   std::string year = data[cols.at(BethYw::YEAR)];
	   int yearInt = stoi(year);
	   double measureData = data[cols.at(BethYw::VALUE)];

	   //Does not retrieve value if not in filters
	   if (!areasFilter->empty()){
		   if(areasFilter->count(localAuthorityCode) <= 0){
			   continue;
		   }
	   }

	   Measure meas(measureCode, measureLabel);
	   meas.setValue(yearInt, measureData);

	   if (!measuresFilter->empty()){
		   if(measuresFilter->count(meas.getCodename()) <= 0){
			   continue;
		   }
	   }

	   //retrieves values from the year tuple
	   int year1 = std::get<0>(*yearsFilter);
	   int year2 = std::get<1>(*yearsFilter);
	   if(year1 != 0 && year2 != 0){
		   if (yearInt < year1 || yearInt > year2){
			   continue;
		   }
	   }

	   //If area doesn't exist creates a new one
	   auto it = areas.find(localAuthorityCode);
	   if (it == areas.end()){
		   Area ar(localAuthorityCode);
		   setArea(localAuthorityCode,ar);
	   }
	   Area& ar = getArea(localAuthorityCode);
	   ar.setName("eng",localAuthorityName);
	   ar.setMeasure(measureCode,meas);
	}
}


/*
  TODO: Areas::populateFromAuthorityByYearCSV(is,
                                              cols,
                                              areasFilter,
                                              yearFilter)

  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  Note that these files do not include the names for areas, instead you
  have to rely on the names already populated through 
  Areas::populateFromAuthorityCodeCSV();

  The datasets that will be parsed by this function are
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/

void Areas::populateFromAuthorityByYearCSV(
		  std:: istream& is,
		  const BethYw::SourceColumnMapping& cols,
		  const StringFilterSet * const areasFilter,
		  const StringFilterSet * const measuresFilter,
		  const YearFilterTuple * yearFilter){
	if (cols.count(BethYw::SINGLE_MEASURE_CODE) <= 0 || cols.count(BethYw::SINGLE_MEASURE_NAME) <= 0){
		throw std::out_of_range("there are not enough columns in cols");
	}

	//Get values for assigning to measure
	std::string line;
	std::string measureCode = cols.at(BethYw::SINGLE_MEASURE_CODE);
	std::string measureName = cols.at(BethYw::SINGLE_MEASURE_NAME);
	std::vector<int> yearsColumns;

	if (!measuresFilter->empty()){
		if (measuresFilter->count(measureCode) <= 0){
			return;
		}
	}

	//iterate over lines
	while(std::getline(is,line)){
		int i = 0;
		std::string areaCode;
		std::stringstream lineStream(line);
		Measure meas(measureCode,measureName);
		bool addCurrentArea;

		//populate years map
		if (yearsColumns.empty()){
			while (std::getline(lineStream, line, ',')){
				if (i != 0){
					yearsColumns.push_back(stoi(line));
				}
				i++;
			}
		}
		//insert value for each year to measure
		while (std::getline(lineStream, line, ',')){
			if (i == 0){
				if (!areasFilter->empty()){
					if(areasFilter->count(line) <= 0){
						addCurrentArea = false;
						break;
					}
				}
				areaCode = line;
				addCurrentArea = true;
			} else {
				meas.setValue(yearsColumns.at(i-1),stod(line));
			}
			i++;
		}

		//only add after measure has been fully populated to save computation
		if (addCurrentArea){
			Area& ar = getArea(areaCode);
			ar.setMeasure(measureCode,meas);
		}
	}
}
/*
  TODO: Areas::populate(is, type, cols)

  Parse data from an standard input stream `is`, that has data of a particular
  `type`, and with a given column mapping in `cols`.

  This function should look at the `type` and hand off to one of the three 
  functions populate?????????() functions.

  The function must check if the stream is in working order and has content.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols);
*/
void Areas::populate(std::istream &is,
                     const BethYw::SourceDataType &type,
                     const BethYw::SourceColumnMapping &cols) {
  if (type == BethYw::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols);
  }
  else {
    throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}

/*
  TODO: Areas::populate(is,
                        type,
                        cols,
                        areasFilter,
                        measuresFilter,
                        yearsFilter)

  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This function should look at the `type` and hand off to one of the three 
  functions you've implemented above.

  The function must check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variables areasFilter, measuresFilter,
    and yearsFilter are created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(
    std::istream &is,
    const BethYw::SourceDataType &type,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter)
     {
  if (type == BethYw::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols, areasFilter);
  } else if (type == BethYw::AuthorityByYearCSV){
	  populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter,yearsFilter);
  } else if (type == BethYw::WelshStatsJSON){
	  populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter,yearsFilter);
  } else {
    throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}

/*
  TODO: Areas::toJSON()

  Convert this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.

  Use the sample JSON library as above to create this. Construct a blank
  JSON object:
    json j;

  Convert this json object to a string:
    j.dump();

  You then need to loop through your areas, measures, and years/values
  adding this data to the JSON object.

  Read the documentation for how to convert your outcome code to JSON:
    https://github.com/nlohmann/json#arbitrary-types-conversions
  
  The JSON should be formatted as such:
    {
    "<localAuthorityCode1>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         ???
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             ???
                                            "<yearN>": <valueN> }
                               },
    "<localAuthorityCode2>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         ???
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             ???
                                            "<yearN>": <valueN> }
                               }
    ...
    "<localAuthorityCodeN>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         ???
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             ???
                                            "<yearN>": <valueN> }
                               }
    }

  An empty JSON is "{}" (without the quotes), which you must return if your
  Areas object is empty.
  
  @return
    std::string of JSON

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {
  json j;
  
  return j.dump();
}

/*
  TODO: operator<<(os, areas)

  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  See the coursework specification for more information, although for reference
  here is a quick example of how output should be formatted:

    <English name of area 1> / <Welsh name of area 1> (<authority code 1>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>


    <English name of area 2> / <Welsh name of area 2> (<authority code 2>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

    ...

    <English name of area y> / <Welsh name of area y> (<authority code y>)
    <Measure 1 name> (<Measure 1 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x codename>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys M??n (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/
std::ostream& operator<<(std::ostream& os, Areas ars){
	std::map<std::string,Area> areasToPrint = ars.getAreas();
	for (auto it = areasToPrint.begin(); it != areasToPrint.end();it++){
		os << it->second;
	}
	return os;
}

