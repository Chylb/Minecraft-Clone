#pragma once

#include <vector>
#include <map>

#include "../state/Property.h"
#include "StateHolder.h"

template<typename O, typename S>
class StateContainer
{
public:
	class Builder
	{
	public:
		Builder(O* block)
		{
			m_owner = block;
		}

		template <typename T>
		Builder& AddProperty(Property<T>* property)
		{
			void* voidPtr = (void*)property;
			m_properties.push_back(static_cast<Property <int>*>(voidPtr));
			return *this;
		}

		StateContainer Create()
		{
			return StateContainer(m_owner, m_properties);
		}

	private:
		O* m_owner;
		std::vector<Property<int>*> m_properties;
	};

	StateContainer() {};
	StateContainer(O* owner, std::vector<Property<int>*> properties);

	S* Any() const;
	const std::vector<S*>& GetPossibleStates() const;

private:
	O* m_owner;
	std::vector<S*> m_states;
	std::vector<Property<int>*> m_properties;
};


template<typename O, typename S>
inline StateContainer<O, S>::StateContainer(O* owner, std::vector<Property<int>*> properties)
{
	m_owner = owner;

	for (auto prop : properties) {
		m_properties.push_back(prop);
	}

	std::vector<S> previousStates = { {} };
	std::vector<S> states;

	for (auto property : properties) {
		states.clear();

		for (auto value : property->GetValues()) {
			for (auto prevState : previousStates) {
				auto state = prevState;
				state.m_values[property] = value;
				states.push_back(state);
			}
		}
		previousStates = states;
	}

	for (auto& prevState : previousStates) {
		S* state = new S(prevState);
		state->m_owner = m_owner;
		m_states.push_back(state);
	}

	std::map<std::map<Property<int>*, int>, S*> statesMap;
	for (auto state : m_states) {
		statesMap.emplace(state->GetValues(), state);
	}

	for (auto state : m_states) {
		state->PopulateNeighbors(statesMap);
	}
}

template<typename O, typename S>
inline S* StateContainer<O, S>::Any() const
{
	return m_states.at(0);
}

template<typename O, typename S>
inline const std::vector<S*>& StateContainer<O, S>::GetPossibleStates() const
{
	return m_states;
}



