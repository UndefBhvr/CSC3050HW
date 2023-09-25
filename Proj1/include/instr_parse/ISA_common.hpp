#ifndef UNDEFBHVR_ISA_COMMON_HPP
#define UNDEFBHVR_ISA_COMMON_HPP

#include <lex/lexer.hpp>
#include <exception>
#include <istream>



namespace common
{

template<typename Data_field>
struct arg_base
{
    virtual Data_field parse(lexer::token_stream::iterator &) = 0;
};

template<typename Data_field, typename... Args>
struct none : arg_base<Data_field>
{
    Data_field parse(lexer::token_stream::iterator &it) override
    {
        return do_parse(it);
    }

    static Data_field do_parse(lexer::token_stream::iterator &it)
    {
        return (Data_field() | ... | Args::parse(it));
    }
};

template<typename>
struct is_none
{
    constexpr static bool value=false;
};

template<typename... Args>
struct is_none<none<Args...>>
{
    constexpr static bool value=true;
};

template<typename T>
constexpr bool is_none_v=is_none<T>::value;

template<typename... Args>
struct first_arg;

template<typename T, typename... Args>
struct first_arg<T, Args...>
{
    using type = T;
};

template<typename... Args>
using first_arg_t = first_arg<Args...>::type;

template<typename Data_field, typename T, typename... Args>
struct args : arg_base<Data_field>
{
    Data_field parse(lexer::token_stream::iterator &it) override
    {
        return do_parse(it);
    }

    static Data_field do_parse(lexer::token_stream::iterator &it)
    {
        Data_field res;
        if constexpr(is_none_v<T>)
        {
            res = T::do_parse(it);
        }
        else
        {
            res = T::parse(it);
        }
        if constexpr (sizeof...(Args) != 0)
        {
            auto [a, b,_] = *it;
            if constexpr (!is_none_v<first_arg_t<Args...>>)
            {
                if (a != lexer::token::SYM || b != ",") std::terminate();
                ++it;
            }
            return res | args<Data_field, Args...>::do_parse(it);
        }
        else
        {
            return res;
        }
    }
    template<typename, typename, typename... A>
    friend struct args;
};



} // namespace common

#endif