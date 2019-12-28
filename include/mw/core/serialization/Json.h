#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*template<class T>
static json ToJSON(const std::vector<T>& elements)
{
	std::vector<json> jsonElems;
	for (const T& elem)
	{
		jsonElems.push_back(elem.ToJSON());
	}
}*/