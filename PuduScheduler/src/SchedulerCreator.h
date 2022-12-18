#pragma once
namespace UI
{
	class SchedulerCreator
	{
	public:

		SchedulerCreator(const SchedulerCreator&) = delete;
		static SchedulerCreator& Get()
		{
			static SchedulerCreator instance;
			return instance;
		}

	private:
		SchedulerCreator()
		{

		}


	};
}//namespace UI
