#pragma once

#include <list>

template<typename T>
class Property
{
public:
	constexpr Property(std::list<T> values) : m_values(values) {}
	const std::list<T>& GetValues() const
	{
		return m_values;
	}

protected:
	std::list<T> m_values;
};