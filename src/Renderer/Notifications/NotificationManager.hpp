#pragma once

namespace change_me
{

	class NotificationManager : public Singleton<NotificationManager>
	{
	public:

		NotificationManager();

		void PushNotification(std::string Tittle, std::string Text);
		void ClearNotifications();

		void Run();

	private:

		std::size_t m_CurrentNotification;
		std::size_t m_RenderingNotifications; /*used to get how many notifications are we rendering at a time*/

		std::vector<std::shared_ptr<AnimationBase>> m_Notifications;

	};
}

