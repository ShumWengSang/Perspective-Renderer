//
// Created by user on 1/31/2021.
//

#ifndef OPENGLFRAMEWORK_BITMASK_H
#define OPENGLFRAMEWORK_BITMASK_H

// From https://gpfault.net/posts/typesafe-bitmasks.txt.html
template<class option_type, typename = typename std::enable_if<std::is_enum<option_type>::value>::type>
class Bitmask
{
    using underlyingType = typename std::underlying_type<option_type>::type;

    // Avoid having to explicitly set enum values to powers of two
    static constexpr underlyingType mask_value(option_type o)
    {
        return 1 << static_cast<underlyingType>(o);
    }
    // private constructor
    explicit constexpr Bitmask(underlyingType o) : mask_(o){}

public:
    // No options selected
    constexpr Bitmask() : mask_(0) {}

    // Bitmask with one bit set.
    constexpr Bitmask(option_type o) : mask_(mask_value(o)){}

    // Set the bit corresponding to the given option
    const Bitmask operator|(option_type t)
    {
        return Bitmask(mask_ | (mask_value(t)));
    }

    const Bitmask operator^(option_type t)
    {
        return Bitmask(mask_ ^ (mask_value(t)));
    }

    const Bitmask Clear(option_type t)
    {
        return Bitmask(mask_ &= (mask_value(t)));
    }

    // Get the value of the bit corresponding to option
    const bool operator& (option_type t) const{
        return mask_ & mask_value(t);
    }

    bool operator== (Bitmask<option_type> const & other) const
    {
        return mask_ == other.mask_;
    }
private:
    underlyingType mask_ = 0;
};

// Creates a bitmask from two options, convenient for stuff like:
// FunctionExpectingBitmask(Options::Opt1 | Options::Opt2 | Options::Opt3)
template <class option_type,
        typename = typename std::enable_if<std::is_enum<option_type>::value>::type>
constexpr Bitmask<option_type> operator|(option_type lhs, option_type rhs) {
    return Bitmask<option_type>{lhs} | rhs;
}
#endif //OPENGLFRAMEWORK_BITMASK_H
