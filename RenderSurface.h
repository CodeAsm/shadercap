/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef __RENDER_SURFACE_H__
#define __RENDER_SURFACE_H__

#include <QOffscreenSurface>
#include <QtGui/QOpenGLFunctions>
#include <QExposeEvent>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>
#include <QImage>
#include <map>
#include "ShaderParameter.h"

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

class RenderSurface : public QOffscreenSurface, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit RenderSurface(const int outWidth, const int outHeight);
    ~RenderSurface();

    int width() const;
    int height() const;

    virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

    QImage getImage() { return imageFrame; }

    bool setShaderCode(const std::string& code);

    std::string getShaderError() const {
      return m_programError;
    }

    void setShaderParameters(const ShaderParameters& shaderParameters) {
      sp = shaderParameters;
    }

    void addTexture(const std::string& name, const QImage& texture) {
      textures[name] = texture;
    }

public slots:
    void renderLater();
    void renderNow(const float time);

private:
    bool event(QEvent *event);

    void exposeEvent(QExposeEvent *event);

    GLuint loadShader(GLenum type, const char *source);

private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;

    GLuint m_posAttr;
    GLuint m_timeAttr;

    QOpenGLShaderProgram *m_program;
    std::string m_programError;
    int m_frame;

    std::string shaderCode;
    QImage imageFrame;

    int m_outWidth;
    int m_outHeight;
    float m_globalTime;

    class Texture {
    public:
      ~Texture() {
        delete obj;
      }
      Texture() : obj(0) {
      }
      Texture(const QImage& image) : image(image) {
        obj = fromImage(image);
      }
      Texture(const Texture& src) : obj(0) {
        *this = src;
      }
      Texture& operator =(const Texture& src) {
        delete obj;
        image = src.image;
        obj = fromImage(image);
        return *this;
      }
    private:
      static QOpenGLTexture* fromImage(const QImage& src) {
        QOpenGLTexture* tex = new QOpenGLTexture(src);
        tex->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::LinearMipMapLinear);
        return tex;
      }
    public:
      QImage image;
      QOpenGLTexture* obj;
    };

    ShaderParameters sp;
    std::map<std::string, Texture> textures;
};

#endif // __RENDER_SURFACE_H__
