#include "Common.hpp"

namespace change_me
{
	std::shared_ptr<AnimationManager> g_AnimationManager;

	AnimationBase::AnimationBase(AnimationFunc_t Update,
		std::chrono::high_resolution_clock::duration Time) :
		m_AnimationTime(Time), m_InternalDelta(std::chrono::high_resolution_clock::now()), m_Update(Update)
	{

	}

	void AnimationBase::OnUpdate(float CurrentDelta)
	{
		m_Update(CurrentDelta);
	}

	void AnimationManager::Run()
	{
		auto CurDelta = std::chrono::high_resolution_clock::now();

		for (auto i = 0; i < m_Animations.size(); i++)
		{
			auto& Animation = m_Animations[i];

			if ((CurDelta.time_since_epoch() - Animation->m_InternalDelta.time_since_epoch()).count() >= Animation->m_AnimationTime.count())
			{
				m_Animations.erase(m_Animations.begin() + i);
				continue;
			}
			auto DeltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
				(CurDelta.time_since_epoch() - Animation->m_InternalDelta.time_since_epoch()).count();

			auto CastedTotalDelta = 
				std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(Animation->m_AnimationTime).count();

			Animation->OnUpdate(DeltaTime / CastedTotalDelta);
		}
	}

	void AnimationManager::PushNewAnimation(std::shared_ptr<AnimationBase> Anim)
	{
		m_Animations.push_back(Anim);
	}

	void AnimationManager::ClearAnimationQueue()
	{
		m_Animations.clear();
	}
}