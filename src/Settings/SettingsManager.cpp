#include "Common.hpp"

namespace change_me
{
    std::shared_ptr<SettingsManager> g_OptionsSettings;
    std::shared_ptr<SettingsManager> g_UISettings;

    SettingsManager::SettingsManager(std::filesystem::path ConfigFile) : m_ConfigFilePath(ConfigFile)
    {
    }
    void SettingsManager::AddListener(std::shared_ptr<SettingsListener> Listener)
    {
        m_Listeners.push_back(Listener);
    }

    void SettingsManager::Save()
    {
        nlohmann::json Buffer;
        for (auto Listener : m_Listeners)
        {
            Listener->Save();

            if (Listener->m_IsRoot)

                Buffer = Listener->m_Buffer;

            else
                Buffer[Listener->GetName().data()] = Listener->m_Buffer;
        }

        std::ofstream FileStream(m_ConfigFilePath.string(), std::ios::out | std::ios::trunc);
        FileStream << Buffer.dump(0);

        FileStream.close();
        Buffer.clear(); /*just in case*/
    }
    void SettingsManager::Load()
    {   
        auto ConfigFile = File(m_ConfigFilePath);
        
        if (!ConfigFile.DoesFileExist())
            return; /*config file does not exist*/

        std::ifstream FileStream(m_ConfigFilePath.string(), std::ios::in);

        if (!FileStream.is_open()) /*there's no config file or it is empty*/
            return;

        auto Buffer = nlohmann::json::parse(FileStream);

        for (auto& Listener : m_Listeners)
        {
            auto Node = Listener->m_IsRoot ?
                Buffer 
                :
                Buffer[Listener->m_Name.data()];
            
            if (!Node.is_null()) /*if the buffer is null we won't load it since our configs has already the default value*/
                Listener->Load(Node);
        }

        FileStream.close();
        Buffer.clear();
    }
}