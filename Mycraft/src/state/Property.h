#pragma once

#include <list>

template<typename T>
class Property
{
public:
	constexpr Property(std::list<T> values)
	{
		for (T val : values)
			m_values.emplace_back((int)val);
	}

	const std::list<int>& GetValues() const
	{
		return m_values;
	}

protected:
	std::list<int> m_values;
};