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

#include "RenderSurface.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QOpenGLFramebufferObject>

#include <QtCore/qmath.h>

RenderSurface::RenderSurface(const int outWidth, const int outHeight)
    : m_update_pending(false)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
    , m_program(0)
    , m_outWidth(outWidth)
    , m_outHeight(outHeight)
{
  //setSurfaceType(QSurface::OpenGLSurface);

  QSurfaceFormat format;
  format.setSamples(16);

  setFormat(format);
  create();
  //resize(640, 480);
  //setVisible(false);
  //show();

  //setAnimating(true);

  renderNow(0.0f);
}

RenderSurface::~RenderSurface()
{
    delete m_device;
}

int RenderSurface::width() const {
  return m_outWidth;
}

int RenderSurface::height() const {
  return m_outHeight;
}

void RenderSurface::render(QPainter *painter)
{
    Q_UNUSED(painter);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(1);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    const QRect drawRect(0, 0, m_outWidth, m_outHeight);
    const QSize drawRectSize = drawRect.size();
    QOpenGLFramebufferObject fbo(drawRectSize, fboFormat);
    fbo.bind();

    glViewport(0, 0, m_outWidth, m_outHeight);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND); 
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);

    m_program->bind();

    int texCount = 0;
    for (size_t i = 0; i < sp.size(); ++i) {
      if (sp[i].bind == ShaderParameter::BindTextureSampler) {
        textures[sp[i].name].obj->bind(texCount);
        m_program->setUniformValue(sp[i].name.c_str(), texCount);
        texCount++;
      } else if (sp[i].bind == ShaderParameter::BindTextureResolution) {
        const QImage& image = textures[sp[i].texture].image;
        m_program->setUniformValue(sp[i].name.c_str(), QVector2D(image.width(),image.height()));
      } else if (sp[i].bind == ShaderParameter::BindGlobalTime) {
        m_program->setUniformValue(sp[i].name.c_str(), m_globalTime);
      } else if (sp[i].bind == ShaderParameter::BindOutputResolution) {
        m_program->setUniformValue(sp[i].name.c_str(), QVector2D(m_outWidth,m_outHeight));
      }
    }

    GLfloat vertices[] = {
      -1.0f,  -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f
    };

    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);

    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);

    m_program->release();

    imageFrame = fbo.toImage();
}

static const char *vertexShaderSource =
    "attribute vec3 pos;\n"
    "//varying vec2 fragCoord;\n"
    "void main() {\n"
    "  //fragCoord = pos.xy;\n"
    "  gl_Position = vec4(pos, 1.0);\n"
    "}\n";

static const char *fragmentShaderSource =
    "highp float;\n"
    "uniform float iGlobalTime;\n"
    "uniform vec2 iResolution;\n"
    "//varying vec2 fragCoord;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
    "}\n";

void RenderSurface::initialize()
{
  setShaderCode(fragmentShaderSource);
}

void RenderSurface::render()
{
    if (!m_device)
        m_device = new QOpenGLPaintDevice;

    glClear(GL_COLOR_BUFFER_BIT);

    m_device->setSize(size());

    QPainter painter(m_device);
    render(&painter);
}

void RenderSurface::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool RenderSurface::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow(0.0f);
        return true;
    default:
        return QOffscreenSurface::event(event);
    }
}

void RenderSurface::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    //if (isExposed())
        //renderNow();
}

void RenderSurface::renderNow(const float time)
{
    m_globalTime = time;

    //if (!isExposed())
        //return;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

void RenderSurface::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

bool RenderSurface::setShaderCode(const std::string& code) {
  m_program = new QOpenGLShaderProgram(this);
  bool ret = m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  if (!ret) {
    m_programError = m_program->log().toStdString();
    return false;
  }
  std::string userCode = "#line 0\n" + code; /* begin at line 1 */
  ret = m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, userCode.c_str());
  if (!ret) {
    m_programError = m_program->log().toStdString();
    return false;
  }
  ret = m_program->link();
  if (!ret) {
    m_programError = m_program->log().toStdString();
    return false;
  }
  m_posAttr = m_program->attributeLocation("pos");
  return true;
}

GLuint RenderSurface::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}
