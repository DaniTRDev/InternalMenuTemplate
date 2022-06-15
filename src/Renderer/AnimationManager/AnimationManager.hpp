#pragma once

namespace change_me
{
	class AnimationManager;

	class AnimationBase
	{
	public:

		friend class AnimationManager;
		using AnimationFunc_t = std::function<void(float)>;

		AnimationBase(AnimationFunc_t Update, std::chrono::high_resolution_clock::duration Time);

		/*CurrentDelta = a value from 0.0f to 1.f that determines the "progression" of the animation*/
		void OnUpdate(float CurrentDelta);

	private:

		std::chrono::high_resolution_clock::duration m_Time;
		bool m_IsTimerInitialized;

		AnimationFunc_t m_Update;
	};

	class AnimationManager : public Singleton<AnimationManager>
	{
	public:

		void Run();

		void PushNewAnimation(std::shared_ptr<AnimationBase> Anim);

		void ClearAnimationQueue();

	private:

		std::vector<std::pair<Timer, std::shared_ptr<AnimationBase>>> m_Animations;

	};
}

