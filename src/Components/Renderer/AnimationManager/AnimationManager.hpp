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

		std::chrono::high_resolution_clock::duration m_AnimationTime;
		std::chrono::high_resolution_clock::time_point m_InternalDelta;

		AnimationFunc_t m_Update;
	};

	class AnimationManager
	{
	public:

		void Run();

		void PushNewAnimation(std::shared_ptr<AnimationBase> Anim);

		void ClearAnimationQueue();

	private:

		std::vector<std::shared_ptr<AnimationBase>> m_Animations;

	}; extern std::shared_ptr<AnimationManager> g_AnimationManager;
}

