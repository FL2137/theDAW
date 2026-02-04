#ifndef WAVEVIEW_HPP
#define WAVEVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVulkanWindow>
#include <QVulkanFunctions>
#include <QVulkanExtension>
#include <QVulkanLayer>
#include <gl/GL.h>

class WaveVulkanRenderer : public QVulkanWindowRenderer
{

public:
    WaveVulkanRenderer(QVulkanWindow* vulkanWindow) : m_vulkanWindow(vulkanWindow)
    {
        m_vulkanFunctions = nullptr;

        m_vulkanInstance.create();

        m_vulkanWindow->setVulkanInstance(&m_vulkanInstance);
    }

    void startNextFrame() override {
        VkCommandBuffer cmdBuffer = m_vulkanWindow->currentCommandBuffer();

        // m_vulkanFunctions->vkCmdBeginRenderPass(cmdBuffer, VkRenderPass

        m_vulkanWindow->frameReady();
    }


protected:

private:
    QVulkanWindow* m_vulkanWindow;
    QVulkanDeviceFunctions* m_vulkanFunctions;
    QVulkanInstance m_vulkanInstance;
};

class VulkanWidget : public QVulkanWindow {
public:

    QVulkanWindowRenderer* createRenderer() override {
        return new WaveVulkanRenderer(this);
    }

private:

};

#endif // WAVEVIEW_HPP