/*
 * Configuration parser with metaprogramming
 * Copyright (C) 2016 iTX Technologies
 *
 * This file is part of Cenisys.
 *
 * Cenisys is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Cenisys is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Cenisys.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CENISYS_CONFIGPARSER_H
#define CENISYS_CONFIGPARSER_H

#include <unordered_map>
#include <vector>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/variadic.hpp>
#include <boost/utility/string_ref.hpp>
#include <toml/toml.h>

#define CENISYS_CONFIG_START_NAMESPACE(r, data, ns)                            \
    namespace ns                                                               \
    {
#define CENISYS_CONFIG_END_NAMESPACE(r, data, ns) }
#define CENISYS_CONFIG_NEST_NAMESPACE(r, data, ns) ::ns

#define CENISYS_CONFIG_STRUCT_MEMBER_WRAP(z, n, data)                          \
    CENISYS_CONFIG_STRUCT_MEMBER(z, n, BOOST_PP_TUPLE_ELEM(n, data))
#define CENISYS_CONFIG_STRUCT_MEMBER(z, n, elem)                               \
    BOOST_PP_TUPLE_ELEM(0, elem)                                               \
    BOOST_PP_TUPLE_ELEM(1, elem){BOOST_PP_TUPLE_ELEM(2, elem)};
#define CENISYS_CONFIG_PARSER_ENTRY_WRAP(z, n, data)                           \
    CENISYS_CONFIG_PARSER_ENTRY(                                               \
        z, n, BOOST_PP_TUPLE_ELEM(0, data),                                    \
        BOOST_PP_TUPLE_ELEM(n, BOOST_PP_TUPLE_ELEM(1, data)))
#define CENISYS_CONFIG_PARSER_ENTRY(z, n, struct, elem)                        \
    BOOST_PP_COMMA_IF(n)                                                       \
    {                                                                          \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem))                       \
        , [](const toml::Value &param, struct &val) {                          \
            ConfigParser<BOOST_PP_TUPLE_ELEM(0, elem)>()(                      \
                param, val.BOOST_PP_TUPLE_ELEM(1, elem));                      \
        }                                                                      \
    }

#define CENISYS_DEFINE_CONFIG_PARSER(ns, name, ...)                            \
    BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_START_NAMESPACE, , ns)                \
    struct name                                                                \
    {                                                                          \
        BOOST_PP_REPEAT(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),                   \
                        CENISYS_CONFIG_STRUCT_MEMBER_WRAP,                     \
                        BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))               \
    };                                                                         \
    BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_END_NAMESPACE, , ns)                  \
    namespace Cenisys                                                          \
    {                                                                          \
    template <>                                                                \
    struct ConfigParser<BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_NEST_NAMESPACE, , \
                                              ns)::name>                       \
    {                                                                          \
        void operator()(const toml::Value &value,                              \
                        BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_NEST_NAMESPACE, , \
                                              ns)::name &val)                  \
        {                                                                      \
            std::unordered_map<                                                \
                boost::string_ref,                                             \
                std::add_pointer_t<void(                                       \
                    const toml::Value &value,                                  \
                    BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_NEST_NAMESPACE, ,     \
                                          ns)::name &)>>                       \
                table = {BOOST_PP_REPEAT(                                      \
                    BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),                       \
                    CENISYS_CONFIG_PARSER_ENTRY_WRAP,                          \
                    (BOOST_PP_SEQ_FOR_EACH(CENISYS_CONFIG_NEST_NAMESPACE, ,    \
                                           ns)::name,                          \
                     BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__)))};               \
            for(const auto &i : value.as<toml::Table>())                       \
            {                                                                  \
                auto it = table.find(i.first);                                 \
                if(it == table.end())                                          \
                    throw std::runtime_error("Unknown option"); /*TODO*/       \
                it->second(i.second, val);                                     \
            }                                                                  \
        }                                                                      \
    };                                                                         \
    }

namespace Cenisys
{

template <typename T>
struct is_toml_container
{
    static constexpr bool value = false;
};

template <typename T>
struct is_toml_container<std::vector<T>>
{
    static constexpr bool value = true;
};

template <>
struct is_toml_container<toml::Table>
{
    static constexpr bool value = false;
};

template <typename T>
constexpr bool is_toml_container_v = is_toml_container<T>::value;

template <typename T>
struct ConfigParser
{
    void operator()(const toml::Value &value, T &val) const
    {
        val = value.as<T>();
    }
};

template <typename T>
struct ConfigParser<std::vector<T>>
{
    void operator()(const toml::Value &value, std::vector<T> &val) const
    {
        for(const auto &i : value.as<toml::Array>())
        {
            val.push_back({});
            ConfigParser<T>()(i, val.back());
        }
    }
};

template <typename T>
struct ConfigParser<std::unordered_map<std::string, T>>
{
    void operator()(const toml::Value &value,
                    std::unordered_map<std::string, T> &val) const
    {
        for(const auto &i : value.as<toml::Table>())
        {
            ConfigParser<T>()(i.second,
                              val.insert({i.first, T()}).first->second);
        }
    }
};

} // namespace Cenisys

#endif // CENISYS_CONFIGPARSER_H
