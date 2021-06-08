#pragma once

#include <map>
#include <ostream>

#include "Property.h"

#define INT_PROP(prop) (Property<int>*) (void*)prop

template<typename O, typename S>
class StateContainer;

template<typename O, typename S>
struct StateHolder
{
public:
	template <typename T>
	T GetValue(Property<T>* p) const;
	const std::map<Property<int>*, int>& GetValues() const;
	template <typename T>
	const S& SetValue(Property<T>* p, T val) const;

	void PopulateNeighbors(std::map<std::map<Property<int>*, int>, S*> map);

	friend std::ostream& operator<<(std::ostream& os, const S& state)
	{
		os << "StateHolder " << &state << " ";
		for (auto entry : state.m_values) {
			int val = entry.second;
			os << val << " ";
		}
		return os;
	}

protected:
	O* m_owner;
	std::map<std::pair<Property<int>*, int>, S*> m_neighbors;
	std::map<Property<int>*, int> m_values;

	friend class StateContainer<O, S>;
};

template<typename O, typename S>
template<typename T>
inline T StateHolder<O, S>::GetValue(Property<T>* p) const
{
	return (T)m_values.at(INT_PROP(p));
}

template<typename O, typename S>
template<typename T>
inline const S& StateHolder<O, S>::SetValue(Property<T>* p, T val) const
{
	return *m_neighbors.at({ INT_PROP(p), (int)val });
}

template<typename O, typename S>
inline const std::map<Property<int>*, int>& StateHolder<O, S>::GetValues() const
{
	return m_values;
}

template<typename O, typename S>
inline void StateHolder<O, S>::PopulateNeighbors(std::map<std::map<Property<int>*, int>, S*> map)
{
	for (auto entry : m_values) {
		auto property = entry.first;

		for (auto value : property->GetValues()) {
			std::map<Property<int>*, int> neighborValues = m_values;
			neighborValues[property] = value;

			m_neighbors[{property, value}] = map.at(neighborValues);
		}
	}
}