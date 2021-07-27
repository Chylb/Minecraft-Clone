#pragma once

#include <queue>
#include <deque>
#include <vector>

#include "NextTickListEntry.h"

class World;

template<typename T>
class TickList
{
public:
	TickList(World* world);
	void Tick();
	void ScheduleTick(BlockPos pos, const T& type, int tickOffset);
	bool WillTickThisTick(BlockPos pos, const T& t);

private:
	World* m_world;
	std::unordered_set<NextTickListEntry<T>, typename NextTickListEntry<T>::HashFunction> m_tickNextTickSet;
	std::priority_queue<NextTickListEntry<T>, std::vector<NextTickListEntry<T>>, typename NextTickListEntry<T>::CompareTick> m_tickNextTickList;
	std::deque<NextTickListEntry<T>> m_currentlyTicking;
	std::list<NextTickListEntry<T>> m_alreadyTicked;
};

template<typename T>
TickList<T>::TickList(World* world) : m_world(world)
{
}

template<typename T>
void TickList<T>::Tick()
{
	while (!m_tickNextTickList.empty())
	{
		NextTickListEntry<T> entry = m_tickNextTickList.top();

		if (entry.triggerTick > m_world->GetTick())
			break;

		m_tickNextTickList.pop();
		m_tickNextTickSet.erase(entry);
		m_currentlyTicking.push_back(entry);
	}

	while (!m_currentlyTicking.empty())
	{
		auto entry = m_currentlyTicking.front();
		m_world->TickBlock(entry);
		m_alreadyTicked.push_back(entry);
		m_currentlyTicking.pop_front();
	}

	m_currentlyTicking.clear();
	m_alreadyTicked.clear();
}

template<typename T>
void TickList<T>::ScheduleTick(BlockPos pos, const T& type, int tickOffset)
{
	NextTickListEntry<T> entry{ pos, type, m_world->GetTick() + tickOffset };
	if (!m_tickNextTickSet.contains(entry)) {
		m_tickNextTickSet.emplace(entry);
		m_tickNextTickList.emplace(entry);
	}
}

template<typename T>
bool TickList<T>::WillTickThisTick(BlockPos pos, const T& t)
{
	NextTickListEntry<T> entry{ pos, t };
	auto it = std::find(m_currentlyTicking.begin(), m_currentlyTicking.end(), entry);
	return it != m_currentlyTicking.end();
}
