#pragma once
#include <cstdint>

namespace MetaP{

    //Check if two types are the same
    /////////////////////////////////////////////////////////////
    template <typename T, typename U>
    struct is_sameType{static constexpr bool value { false };};

    template <typename T>
    struct is_sameType<T, T>{static constexpr bool value { true };};
    //Helper
    template <typename T, typename U>
    constexpr bool is_sameType_v = is_sameType<T, U>::value;
    /////////////////////////////////////////////////////////////
    
    //
    template <typename T, T Val>
    struct constant { static constexpr T value{ Val }; };

    //Return position of a type in a list
    /////////////////////////////////////////////////////////////
    template <typename T, typename... Ts>
    struct pos_type { static_assert(sizeof...(Ts) != 0); };
    //Helper
    template <typename T, typename... Ts>
    constexpr std::size_t pos_type_v = pos_type<T, Ts...>::value;
    //Case: type is 1st in the list
    template <typename T, typename... Ts>
    struct pos_type<T, T, Ts...> : constant<std::size_t, 0>{};
    //Case: not 1st
    template<typename T, typename U, typename... Ts>
    struct pos_type<T, U, Ts...> : constant<std::size_t, 1+pos_type_v<T, Ts...> >{};
    /////////////////////////////////////////////////////////////

    //If types
    /////////////////////////////////////////////////////////////
    template <typename T>
    struct type_id { using type = T; };
    //Condition false
    template <bool Condition, typename cT, typename cF>
    struct IFType : type_id < cF >{};
    //Condition true
    template <typename cT, typename cF>
    struct IFType<true, cT, cF> : type_id<cT>{};
    //Helper
    template <bool Condition, typename cT, typename cF>
    using IFType_t = typename IFType<Condition, cT, cF>::type;
    /////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////
    //CREATE TYPELIST
    /////////////////////////////////////////////////////////////

    template <typename... Ts>
    struct Typelist{
        //Returns the size of the type list
        consteval static std::size_t size() noexcept { return sizeof...(Ts); }
        
        //Check if one type is on the list
        template <typename Type>
        consteval static bool contains() noexcept { return (false || ... || is_sameType_v<Type, Ts>); }

        //Return the type position in the list
        template <typename Type>
        consteval static auto pos() noexcept { 
            static_assert(contains<Type>()); 
            return pos_type_v<Type, Ts...>; 
        }
    };


    //Change typelist with other class
    ///////////////////////////////////////////////////////////
    template <template <typename...> class N, typename List>
    struct replace{};
    template <template <typename...> class N, typename... Ts>
    struct replace<N, Typelist<Ts...>> : type_id <N<Ts...>>{};
    //Helper
    template <template <typename...> class N, typename List>
    using replace_t = typename replace<N, List>::type;
    ///////////////////////////////////////////////////////////

    //Insert template into a class
    ///////////////////////////////////////////////////////////
    template <template <typename...> class C, typename List>
    struct forall_insert_template{};
    template <template <typename...> class C, typename... Ts>
    struct forall_insert_template<C, Typelist<Ts...>> : type_id<Typelist<C<Ts>...> >{};
    //Helper
    template <template <typename...> class C, typename List>
    using forall_insert_template_t = typename forall_insert_template<C, List>::type;
    ///////////////////////////////////////////////////////////

};//End namespace MetaP