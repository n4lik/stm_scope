#include "stm32f4xx.h"
#include "core_cm4.h"

#include "STick.h"

extern "C" {

void SysTick_Handler()
{
	using namespace util;

	auto callbacks = STick::Instance().Callbacks();

	for (auto it = callbacks->begin(); it != callbacks->end(); ++it) {

		if (STick::Instance().Count() >= (*it)->callTime_) {

			(*it)->cb_((*it)->data_);

			if ((*it)->renew_)
				(*it)->callTime_ += (*it)->time_;
			else
				it = callbacks->erase(it);
		}
	}

	++STick::Instance();
}

}

namespace util {

STick &STick::Instance()
{

	static STick instance;
	return instance;
}

STick::STick()
{
	SysTick_Config(SystemCoreClock/1000);
}

STick::~STick()
{
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}


std::list<util::Timer *> *STick::Callbacks()
{
	return &callbacks_;
}

void STick::RegisterCallback(util::Timer *timer)
{
	timer->callTime_ = count_ + timer->time_;

	callbacks_.push_back(timer);
}

void STick::UnregisterCallback(util::Timer *timer)
{
	callbacks_.remove(timer);
}

void STick::operator++()
{
	++count_;
}

unsigned STick::Count()
{
	return count_;
}

} // util
