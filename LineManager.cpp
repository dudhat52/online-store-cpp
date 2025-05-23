#include <iostream>
#include <fstream>
#include <algorithm>
#include "LineManager.h"
#include "Utilities.h"

namespace seneca
{
	std::deque<CustomerOrder> g_pending{};
	std::deque<CustomerOrder> g_completed{};
	std::deque<CustomerOrder> g_incomplete{};

	LineManager::LineManager(const std::string& file, const std::vector<Workstation*>& stations)
	{
		std::ifstream input(file);
		std::string line;
		std::string current, next;
		Utilities util;
		bool more = true;
		size_t pos = 0;

		if (input)
		{
			while (std::getline(input, line))
			{
				current = util.extractToken(line, pos, more);
				next = more ? util.extractToken(line, pos, more) : "";

				auto station = std::find_if(stations.begin(), stations.end(), [current](const Workstation* s)
					{
						return s->getItemName() == current;
					});

				if (station != stations.end() && !next.empty())
				{
					auto nextStation = std::find_if(stations.begin(), stations.end(), [next](const Workstation* s)
						{
							return s->getItemName() == next;
						});
					if (nextStation != stations.end())
					{
						(*station)->setNextStation(*nextStation);
					}
				}

				m_activeLine.push_back(*station);
			}

			auto firstStation = std::find_if(stations.begin(), stations.end(), [&](const Workstation* s1)
				{
					return std::none_of(stations.begin(), stations.end(), [&](const Workstation* s2)
						{
							return s1 == s2->getNextStation();
						});
				});

			m_firstStation = *firstStation;
			m_cntCustomerOrder = g_pending.size();
		}
	}


	void LineManager::reorderStations()
	{
		std::vector<Workstation*> newLine;
		newLine.push_back(m_firstStation);
		int i = 0;

		while (newLine[i]->getNextStation() != nullptr)
		{
			newLine.push_back(newLine[i]->getNextStation());
			i++;
		}
		m_activeLine = newLine;
	}

	bool LineManager::run(std::ostream& os)
	{
		static int CNT{ 1 };

		os << "Line Manager Iteration: " << CNT++ << std::endl;

		if (!g_pending.empty())
		{
			*m_firstStation += std::move(g_pending.front());
			g_pending.erase(g_pending.begin());
		}

		for (auto& work : m_activeLine)
		{
			work->fill(os);
		}

		for (auto& work : m_activeLine)
		{
			work->attemptToMoveOrder();
		}

		return g_completed.size() + g_incomplete.size() == m_cntCustomerOrder;
	}

	void LineManager::display(std::ostream& os) const
	{
		for (size_t i = 0; i < m_activeLine.size(); i++)
		{
			m_activeLine[i]->display(os);
		}
	}
}