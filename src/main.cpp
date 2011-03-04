
#include <iostream>

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"

#include "tinyxml.h"

#include "json/value.h"

void check_song_values(Json::Value& val);
std::string dehtml_itunes_name(std::string);
std::string get_filename_from_path(std::string);

int main (int argc, char** argv) {
	if (argc < 2)
		return -1;

	TiXmlDocument doc(argv[1]);
	doc.LoadFile();
	
	Json::Value song_db;
	Json::Value& db_array = song_db["songs"];
	unsigned int id_counter = 0;

	TiXmlHandle handle(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = handle.FirstChildElement().Element();

	hRoot = TiXmlHandle(pElem);

	//TiXmlElement* elem  = hRoot.FirstChild("dict").FirstChild("dict").FirstChild("dict").FirstChild().Element();

	TiXmlElement* dict_elem = hRoot.FirstChild("dict").FirstChild("dict").FirstChild("dict").Element();

	for (; dict_elem; dict_elem = dict_elem->NextSiblingElement("dict"), ++id_counter) {

		TiXmlElement* elem = dict_elem->FirstChildElement();

		Json::Value song_value;
	
		song_value["id"] = id_counter;
	
		for (elem; elem; elem = elem->NextSiblingElement()) {
			if (elem->GetText() == NULL) {
				elem->NextSiblingElement();
				continue;
			}

			std::string key = elem->GetText();

			if (key == "Artist") {
				elem = elem->NextSiblingElement();

				if (elem->GetText() == NULL) {
					song_value["artist"] = "Unknown Artist";
					continue;
				}

				song_value["artist"] = elem->GetText();
			} else if (key == "Name") {
				elem = elem->NextSiblingElement();

				if (elem->GetText() == NULL) {
					song_value["title"] = "Unknown Name";
					continue;
				}

				song_value["title"] = elem->GetText();
			} else if (key == "Album") {
				elem = elem->NextSiblingElement();

				if (elem->GetText() == NULL) {
					song_value["album"] = "Unknown Album";
					continue;
				}

				song_value["album"] = elem->GetText();
			} else if (key == "Total Time") {
				elem = elem->NextSiblingElement();

				if (elem->GetText() == NULL) {
					song_value["length"] = 1;
					continue;
				}

				song_value["length"] = static_cast<int>(boost::lexical_cast<int>(elem->GetText()) / 1000);
			}


			/* 
			else if (key == "Location") {
				elem = elem->NextSiblingElement();

				std::cout << elem->GetText() << std::endl;

				song_value["filename"] = dehtml_itunes_name(get_filename_from_path(elem->GetText()));
			}
			*/

		}

		check_song_values(song_value);

		db_array[id_counter] = song_value;

	}

	std::cout << song_db.toStyledString() << std::endl;

	return 0;
}

void check_song_values(Json::Value& val) {
	if (val["artist"].empty())
		val["artist"] = "Unknown Artist";
	else if (val["title"].empty())
		val["title"] = "Unknown Title";
	else if (val["album"].empty())
		val["album"] = "Unknown Album";
	else if (val["length"].empty())
		val["length"] = 1;
}


// TODO: this only removes the %20 from the names, should convert html stuff into unicode
std::string dehtml_itunes_name(std::string htmld) {
	std::vector<std::string> strings;

	boost::algorithm::split_regex(strings, htmld, boost::regex("%20"));

	std::string new_str;

	for (std::vector<std::string>::const_iterator it = strings.begin(); 
		it != strings.end();) {
		new_str += *it;

		++it;

		if (it != strings.end())
			new_str += " ";
	}

	return new_str;
}

std::string get_filename_from_path(std::string path) {
	std::vector<std::string> strings;
	boost::split(strings, path, boost::is_any_of("/"));		

	return strings[strings.size() - 1];
} 
