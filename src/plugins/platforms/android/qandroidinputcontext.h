/****************************************************************************
**
** Copyright (C) 2012 BogDan Vatra <bogdan@kde.org>
** Copyright (C) 2016 Olivier Goffart <ogoffart@woboq.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ANDROIDINPUTCONTEXT_H
#define ANDROIDINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <jni.h>
#include <qevent.h>

QT_BEGIN_NAMESPACE

class QAndroidInputContext: public QPlatformInputContext
{
    Q_OBJECT
    enum CapsMode
    {
        CAP_MODE_CHARACTERS = 0x00001000,
        CAP_MODE_SENTENCES  = 0x00004000,
        CAP_MODE_WORDS      = 0x00002000
    };

public:
    struct ExtractedText
    {
        ExtractedText() { clear(); }

        void clear()
        {
            partialEndOffset = partialStartOffset = selectionEnd = selectionStart = startOffset = -1;
            text.clear();
        }

        int partialEndOffset;
        int partialStartOffset;
        int selectionEnd;
        int selectionStart;
        int startOffset;
        QString text;
    };

public:
    QAndroidInputContext();
    ~QAndroidInputContext();
    static QAndroidInputContext * androidInputContext();
    bool isValid() const { return true; }

    void reset();
    void commit();
    void update(Qt::InputMethodQueries queries);
    void invokeAction(QInputMethod::Action action, int cursorPosition);
    QRectF keyboardRect() const;
    bool isAnimating() const;
    void showInputPanel();
    void hideInputPanel();
    bool isInputPanelVisible() const;

    bool isComposing() const;
    void clear();
    void setFocusObject(QObject *object);
    void sendShortcut(const QKeySequence &);

    //---------------//
    jboolean beginBatchEdit();
    jboolean endBatchEdit();
    jboolean commitText(const QString &text, jint newCursorPosition);
    jboolean deleteSurroundingText(jint leftLength, jint rightLength);
    jboolean finishComposingText();
    jint getCursorCapsMode(jint reqModes);
    const ExtractedText &getExtractedText(jint hintMaxChars, jint hintMaxLines, jint flags);
    QString getSelectedText(jint flags);
    QString getTextAfterCursor(jint length, jint flags);
    QString getTextBeforeCursor(jint length, jint flags);
    jboolean setComposingText(const QString &text, jint newCursorPosition);
    jboolean setComposingRegion(jint start, jint end);
    jboolean setSelection(jint start, jint end);
    jboolean selectAll();
    jboolean cut();
    jboolean copy();
    jboolean copyURL();
    jboolean paste();

public slots:
    void updateCursorPosition();
    void updateSelectionHandles();
    void handleLocationChanged(int handleId, int x, int y);
    void touchDown(int x, int y);
    void longPress(int x, int y);
    void keyDown();

private slots:
    void showInputPanelLater(Qt::ApplicationState);

private:
    void sendInputMethodEventThreadSafe(QInputMethodEvent *event);
    Q_INVOKABLE void sendInputMethodEventUnsafe(QInputMethodEvent *event);

    QSharedPointer<QInputMethodQueryEvent> focusObjectInputMethodQueryThreadSafe(Qt::InputMethodQueries queries = Qt::ImQueryAll);
    Q_INVOKABLE QInputMethodQueryEvent *focusObjectInputMethodQueryUnsafe(Qt::InputMethodQueries queries);

    Q_INVOKABLE QVariant queryFocusObjectUnsafe(Qt::InputMethodQuery query, QVariant argument);
    QVariant queryFocusObjectThreadSafe(Qt::InputMethodQuery query, QVariant argument);

private:
    ExtractedText m_extractedText;
    QString m_composingText;
    int m_composingTextStart;
    int m_composingCursor;
    QMetaObject::Connection m_updateCursorPosConnection;
    bool m_blockUpdateSelection;
    enum CursorHandleShowMode {
        CursorHandleNotShown,
        CursorHandleShowNormal = 1,
        CursorHandleShowSelection = 2,
        CursorHandleShowPopup = 3
    };
    CursorHandleShowMode m_cursorHandleShown;
    int m_batchEditNestingLevel;
    QObject *m_focusObject;
};

QT_END_NAMESPACE

#endif // ANDROIDINPUTCONTEXT_H
