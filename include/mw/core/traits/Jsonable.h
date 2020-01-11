#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/serialization/Json.h>

namespace Traits
{
    class IJsonable
    {
    public:
        virtual ~IJsonable() = default;

        virtual json ToJSON() const = 0;
    };
}

template <typename BasicJsonType,
    typename T, typename SFINAE = typename std::enable_if<std::is_base_of<Traits::IJsonable, T>::value>::type>
static void to_json(BasicJsonType& j, const T& value) {
    //// we want to use ADL, and call the correct to_json overload
    //using nlohmann::to_json; // this method is called by adl_serializer,
    //                         // this is where the magic happens
    //to_json(j, value);
    j = value.ToJSON();
}

template <typename BasicJsonType,
    typename T, typename SFINAE = typename std::enable_if<std::is_base_of<Traits::IJsonable, T>::value>::type>
static void from_json(const BasicJsonType& j, T& value) {
    //// same thing here
    //using nlohmann::from_json;

    value = T::FromJSON(j);
}