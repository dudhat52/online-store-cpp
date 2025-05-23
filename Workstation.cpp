#include <iostream>
#include "Workstation.h"

namespace seneca
{
	Workstation::Workstation(const std::string& src) : Station(src)
	{ }

	void Workstation::fill(std::ostream& os)
	{
		if (!m_orders.empty())
		{
			m_orders.front().fillItem(*this, os);
		}
	}

	bool Workstation::attemptToMoveOrder()
	{
		bool result = false;


		if (!m_orders.empty() && (m_orders.front().isItemFilled(getItemName()) || !getQuantity()))
		{
			if (m_pNextStation)
			{
				*m_pNextStation += std::move(m_orders.front());
			}
			else
			{
				if (m_orders.front().isOrderFilled())
				{
					g_completed.push_back(std::move(m_orders.front()));

				}
				else
				{
					g_incomplete.push_back(std::move(m_orders.front()));
				}
			}
			m_orders.pop_front();
			result = true;
		}

		return result;
	}

	void Workstation::setNextStation(Workstation* station)
	{
		m_pNextStation = station;
	}

	Workstation* Workstation::getNextStation() const
	{
		return m_pNextStation;
	}

	void Workstation::display(std::ostream& os) const
	{
		os << getItemName() << " --> " << (m_pNextStation != nullptr ? m_pNextStation->getItemName() : "End of Line") << '\n';
	}

	Workstation& Workstation::operator+=(CustomerOrder&& newOrder)
	{
		m_orders.push_back(std::move(newOrder));

		return *this;
	}

}