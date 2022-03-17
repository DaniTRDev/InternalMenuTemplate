#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<AnimationManager> g_AnimationManager;

	AnimationBase::AnimationBase(AnimationFunc_t Update,
		std::chrono::high_resolution_clock::duration Time) : m_Time(Time), m_Update(Update)
	{

	}

	void AnimationBase::OnUpdate(float CurrentDelta)
	{
		m_Update(CurrentDelta);
	}

	void AnimationManager::Run()
	{
		for (auto i = 0; i < m_Animations.size(); i++)
		{
			auto& Animation = m_Animations[i];

			if (!Animation.second->m_IsTimerInitialized)
			{
				Animation.first.UpdateInternalTime();
				Animation.first.OnUpdate();

				Animation.second->m_IsTimerInitialized = true;
			}
			else if (Animation.first.OnUpdate() && Animation.second->m_IsTimerInitialized)
			{
				m_Animations.erase(m_Animations.begin() + i);
				continue;
			}
			

			Animation.second->OnUpdate(Animation.first.GetDeltaFromDelay());
		}
	}

	void AnimationManager::PushNewAnimation(std::shared_ptr<AnimationBase> Anim)
	{
		m_Animations.push_back(std::make_pair(Timer(Anim->m_Time), Anim));
	}

	void AnimationManager::ClearAnimationQueue()
	{
		m_Animations.clear();
	}
}