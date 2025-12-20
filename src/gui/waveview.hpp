#ifndef WAVEVIEW_HPP
#define WAVEVIEW_HPP

#include <QOpenGLWidget>


class WaveView : public QOpenGLWidget
{


protected:
    void initializeGL() override
    {
        QOpenGLContext* openglCtx = QOpenGLContext::currentContext()->functions();
        
    }
}


#endif // WAVEVIEW_HPP