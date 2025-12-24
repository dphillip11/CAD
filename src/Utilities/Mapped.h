#pragma once

template <typename Container, typename Member>
auto Mapped(const Container& c, Member member)
{
    using Value = std::decay_t<decltype(c.front().*member)>;

    std::vector<Value> result;
    result.reserve(c.size());

    for (const auto& elem : c)
        result.push_back(elem.*member);

    return result;
}