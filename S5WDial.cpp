/*

    S5WDial.cpp - part of DialWidget
    Copyright © 2007 Michael Pyne <michael.pyne@kdemail.net>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#include <QPrinter>
#include <QMouseEvent>
#include <QRegion>
#include <QRectF>
#include <QFontMetrics>
#include <QtDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QFont>
#include <QColor>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QLinearGradient>

#include <math.h>
#include <stdlib.h>

#include "stackblur.h"
#include "math-support.h"
#include "S5WDial.h"

// This is probably broken due to the changes implemented for caching and other
// speedups.
//#define SHOW_BEZIER // Debugging code to quickly grab coords.

// Flesh out the constants.
const double S5WDial::FULL_RANGE = 260.0;
const double S5WDial::FULL_SCALE = 90 - FULL_RANGE / 2;
const double S5WDial::ZERO_SCALE = FULL_SCALE + FULL_RANGE;
const QRectF S5WDial::FACE_RECT(-98.0, -98.0, 196.0, 196.0);
const QRectF S5WDial::TICK_MARKS_RECT(-90.0, -90.0, 180.0, 180.0);
const double S5WDial::MAX_NEEDLE_SPEED = FULL_RANGE; // i.e. FULL_RANGE in 1 second.
const int S5WDial::FRAMES_PER_SECOND = 30; // Hopefully this will work fine.

struct S5WDial::Private {
    // Control points for any bezier curves, see renderDrawing() for more info.
    // Basically these get drawn and can be altered with left and right mouse button
    // if SHOW_BEZIER is defined in S5WDial.cpp
    QPoint m_ctrl1, m_ctrl2;

    // Caches the background for speed
    QPixmap background, needleCoverPixmap;

    // Base needle and cover paths on the 200x200 coordinate system, with the
    // needle pointing straight up (i.e. not rotated).
    QPainterPath needle, needleCover;

    QPainterPath positionedNeedle; // Scaled/translated/rotated

    bool m_shown;

    Private() : m_ctrl1(-30, -30), m_ctrl2(30, -30)
    {
        // The path for the needle.
        needle.moveTo(10, -5);
        needle.lineTo(10, -5);

        needle.addEllipse(-10,-10,20,20);

    }
};

// This class is used to help S5WDial draw the needle shadow as a wrapper.
class S5WDialPainterWrapper
{
public:
    typedef void (S5WDial::*paintFn)(QPainter&, const QRect &);
    S5WDialPainterWrapper(S5WDial *p, paintFn fn) : _p(p), _fn(fn)
    { }

    void operator()(QPainter &p, const QRect &r) { (_p->*_fn)(p, r); }

private:
    S5WDial *_p;
    paintFn _fn;
};

S5WDial::S5WDial(QWidget *parent, double divider) : QWidget(parent), m_divider(divider),
    m_tickStep(5.0), m_greenLow(-1), m_greenHigh(-1), m_redValue(-1),
    m_redIsHigher(true),
    m_antialiasingEnabled(true), m_dropShadowEnabled(true),
    m_animationEnabled(false), m_animationTimer(new QTimer(this)),
    m_dblValue(50.0), m_velocity(0.0), m_minValue(50.0), m_maxValue(100.0),
    m_label("No Sensor"),
    d(new Private)
{
    
    setBackgroundRole(QPalette::Light);

    connect(m_animationTimer, SIGNAL(timeout()), SLOT(moveNeedle()));

//  This function will save a PDF of the widget to /tmp/dial-printout.pdf, with
//    graphing lines attached as well.
//    QTimer::singleShot(0, this, SLOT(printOut()));

    //qDebug() << "IN here";
}

void S5WDial::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    renderDrawing(painter);
}

void S5WDial::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
    d->m_shown = false;
}

void S5WDial::showEvent(QShowEvent *e)
{
    d->m_shown = true;
    d->positionedNeedle = scaledNeedlePath(currentRotation());
    update(); // update face is not necessary here.

    QWidget::showEvent(e);
}

double S5WDial::currentRotation() const
{
    // m_dblValue used to account for animation.
    // But if m_dblValue ends up greater than FULL_RANGE / 2 + 5 or so
    // in either direction, we need to clamp it to prevent it from going
    // over-range.
    return clamp(degreesFromValue(m_dblValue), -(FULL_RANGE / 2 + 5), FULL_RANGE / 2 + 5);
}

QSize S5WDial::sizeHint() const
{
    return QSize(150, 150);
}

void S5WDial::drawTickMarks(QPainter &painter)
{
    // Wouldn't this suck? :)
    if(maximum() == minimum())
        return;

    QFont f("DejaVu Sans Condensed", 8.5, QFont::Light);
    painter.setFont(f);

    qreal tickSeparation;
    tickSeparation = FULL_RANGE * m_tickStep / (maximum() - minimum());

    int j = 0;

    QPen thickLinePen(Qt::black, 2);
    QPen blackLine(Qt::black);
    thickLinePen.setCapStyle(Qt::FlatCap);

    for(double i = minimum(); i <= maximum(); i += m_tickStep, ++j) {
        painter.save();

        qreal rotation = -FULL_RANGE / 2 + j * tickSeparation;
        painter.rotate(rotation);

        painter.setPen(thickLinePen);
        painter.drawLine(0, -98, 0, -90);
        painter.setPen(blackLine);

        // Now translate "up" the line, rotate back to normal, draw text, and
        // we're done.
        painter.save(); // 2

        QString text = QString::number(i);
        QRectF br = painter.fontMetrics().boundingRect(text);
        double offset = br.width() / 2 * qAbs(sin(qAbs(rotation * M_PI / 180.0))) +
                        br.height() / 2 * qAbs(cos(qAbs(rotation * M_PI / 180.0))) ;

        // Move the point of rotation to the center of the text box, this is
        // a great shortcut to that feat. :)
        br.moveCenter(QPoint(0,0));

        painter.translate(0, -89 + offset);
        painter.rotate(-rotation);

        // If on the left side of the dial, use left-aligned text.
        // If on the top side, use centered.
        // If on the right side, use right-aligned text.
        int flags = Qt::AlignCenter; // middle/top case

        if(rotation > 45.0) { // left side
            flags = Qt::AlignLeft | Qt::AlignVCenter;
        }
        else if(rotation < 45.0) { // right side
            flags |= Qt::AlignRight | Qt::AlignVCenter;
        }

        painter.drawText(br, flags, text);
        painter.restore(); // 2

        if((i + m_tickStep / 2) < maximum()) {
            for(int k = 0; k < 4; ++k) {
                painter.rotate(tickSeparation / 5);
                painter.drawLine(0, -90, 0, -95);
            }
        }

        painter.restore();
    }
}

double S5WDial::deltaToDegrees(double delta) const
{
    return delta * FULL_RANGE / (maximum() - minimum());
}

double S5WDial::degreesToDelta(double degrees) const
{
    return degrees * (maximum() - minimum()) / FULL_RANGE;
}

void S5WDial::setValue(int value)
{
    setValue(static_cast<double>(value));
}

void S5WDial::display(double value)
{
    setValue(value/m_divider);
}

void S5WDial::setValue(double val)
{
    if(val == value() && val == m_dblValue)
        return;

    if(m_setValue != val) {
        emit valueChanged(val);
        emit valueChanged(static_cast<int>(val));
    }

    m_setValue = val;

    if(!animationEnabled() || !d->m_shown) {
        m_dblValue = value();

        if(d->m_shown) {
            d->positionedNeedle = scaledNeedlePath(currentRotation());
            update(); // update face is not necessary here.
        }
    }
    else {
        double delta = m_dblValue - m_setValue;
        double overRange = degreesToDelta(FULL_RANGE / 2 + 5);
        m_dblValue = qBound(minimum() - overRange, m_dblValue, maximum() + overRange);

        double maxNeedleDelta = degreesToDelta(MAX_NEEDLE_SPEED) / FRAMES_PER_SECOND;
        m_needleDelta = qMin(qAbs(delta) / FRAMES_PER_SECOND, maxNeedleDelta);
        m_animationTimer->start(1000 / FRAMES_PER_SECOND);
    }
}

void S5WDial::setGreenBand(double low, double high)
{
    m_greenLow = low;
    m_greenHigh = high;

    updateFace();
}

void S5WDial::moveNeedle()
{
    // The way this is currently implemented is to kind of simulate the
    // momentum of the needle.  So, we keep track of the velocity of the needle,
    // in degrees/frame to make it simple.  Then, we make the acceleration on
    // the needle proportional to the distance between the endpoint and the
    // current position.  We adjust the velocity based on current acceleration,
    // and then adjust the current position.
    double diff = (m_setValue - m_dblValue) / 6;

    m_velocity = diff * deltaToDegrees(qAbs(m_setValue - m_dblValue)) / 80;
    double maxDelta = qMin(qAbs(degreesToDelta(FULL_RANGE) / FRAMES_PER_SECOND), qAbs(m_dblValue - value()));
    m_velocity = clamp(m_velocity, -maxDelta, maxDelta);

    // Convert degrees per second into the value-units/second scale.
    m_dblValue += m_velocity;// * (maximum() - minimum()) / FULL_RANGE;

    if(deltaToDegrees(qAbs(m_dblValue - value())) < 1) {
        m_dblValue = value();
        m_animationTimer->stop();
        m_velocity = 0;
    }

    d->positionedNeedle = scaledNeedlePath(currentRotation());
    update();
}

void S5WDial::setRedBand(double value, bool higherIsWorse)
{
    m_redValue = value;
    m_redIsHigher = higherIsWorse;

    updateFace();
}

void S5WDial::drawBand(QPainter &painter, QColor color, double l, double r)
{
    double delta = maximum() - minimum();
    if(delta == 0.0)
        return;

    int rBound = 16 * (FULL_SCALE + FULL_RANGE * (maximum() - r) / delta);
    int lBound = 16 * (FULL_SCALE + FULL_RANGE * (maximum() - l) / delta);

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawPie(FACE_RECT, rBound, lBound - rBound);

    /* DONAL */
    painter.setBrush(Qt::darkGray);
    painter.drawPie(TICK_MARKS_RECT, rBound - 16, lBound - rBound + 32);
}

void S5WDial::setTickInterval(double interval)
{
    m_tickStep = interval;
    updateFace();
}

void S5WDial::drawNeedleToPainter(QPainter &p, const QRect &)
{
    p.setPen(Qt::black);
    p.setBrush(Qt::white);
    p.drawPath(d->needleCover);
}

template<typename T>
QImage S5WDial::createShadowMask(T &fn, int radius)
{
    QImage cover(size(), QImage::Format_ARGB32_Premultiplied);
    QColor trans(Qt::transparent);
    cover.fill(trans.rgba());

    QPainter coverPainter;
    coverPainter.begin(&cover);
    coverPainter.setWorldMatrix(translationMatrix(width(), height()));
    coverPainter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);

    fn(coverPainter, QRect(-100, -100, 200, 200));

    coverPainter.end();
    stackBlur(cover, radius);

    return cover;
}

void drawFaceToPainter(QPainter &p, const QRect &r)
{
    p.setPen(QPen(Qt::black, 2));
    p.setBrush(Qt::NoBrush); // Fill with something
    p.drawEllipse(S5WDial::TICK_MARKS_RECT.adjusted(-1, 0, 1, 1));
}

void S5WDial::renderDrawingBackground(QPainter &painter, bool expand)
{
    painter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);


    painter.setBrush(Qt::darkGray);
    painter.drawEllipse(FACE_RECT);

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::black);

    // Draw green band.
    if(m_greenHigh > m_greenLow) {
        double l = qMax(minimum(), m_greenLow);
        double h = qMin(maximum(), m_greenHigh);

        drawBand(painter, Qt::green, l, h);
    }

    // Draw red band.
    if(m_redValue != -1 && m_redValue < maximum() && m_redValue > minimum()) {
        if(m_redIsHigher)
            drawBand(painter, Qt::red, m_redValue, maximum());
        else
            drawBand(painter, Qt::red, minimum(), m_redValue);
    }

    // Draw shadow of the face ring.
    QImage faceShadow = createShadowMask(drawFaceToPainter, 2);

    painter.setWorldMatrixEnabled(false);

    {
        QMatrix m(translationMatrix(width(), height()));
        QRect r = m.mapRect(S5WDial::TICK_MARKS_RECT.toRect());
        QPainterPath clipPie;

        clipPie.moveTo(m.map(QPoint(0, 0)));
        clipPie.arcTo(r, -45.5, 180 + (45.5 * 2));
        clipPie.closeSubpath();

        painter.setClipPath(clipPie);
    }

    painter.drawImage(rect(), faceShadow);
    painter.setClipping(false);
    painter.setWorldMatrixEnabled(true);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawEllipse(FACE_RECT); // Fill in edges of band

    drawTickMarks(painter);

    // Draw the inner circle part of the dial, where tick marks and needle
    // touch.
    


    // Clip future drawing to the face of the dial.
    QPainterPath clipPath;
    clipPath.addEllipse(FACE_RECT);
    // Paths don't seem to play nice with clip regions. :(
    painter.setClipPath(clipPath);


    // Draw the label.
    QRectF textRect(-98.0, 30, 2 * 98.0, 50);
    painter.setFont(QFont("monospace", 8));
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop,
                     m_label);
//                     QString::number(m_dblValue, 'g', 3));

    // Let's add a few screws.  Done last since we'll be moving the origin
    // around.

}

void S5WDial::renderDrawing(QPainter &painter, bool expand)
{
    int w = painter.device()->width();
    int h = painter.device()->height();

    painter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);
    QMatrix m = translationMatrix(w, h);

    // Expand used if printing.
    if(expand) {
        // Printing, the cached background will not be useful.
        m.scale(0.75, 0.75);
        painter.setWorldMatrix(m);
        renderDrawingBackground(painter, expand);
    }
    else {
        // Draw in cached background.
        painter.drawPixmap(rect(), d->background, d->background.rect());
    }

    // Get ready to draw the needle. We need to draw into a QImage for the
    // drop shadow so let's make yet another painter.

    // Rotation amount determined by value.
    double rotation = currentRotation();
    m.rotate(rotation);

    if(dropShadowEnabled()) {
        // this QImage needs to be size() large so we can draw into it without
        // transforming.
        QImage needleImage(size(), QImage::Format_ARGB32_Premultiplied);
        QColor trans(Qt::transparent);
        needleImage.fill(trans.rgba());

        QPainter needlePainter;

        needlePainter.begin(&needleImage);
        needlePainter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);

        needlePainter.setBrush(Qt::black);
        needlePainter.setPen(Qt::white);
        needlePainter.drawPath(d->positionedNeedle);

        needlePainter.end();

        // But we don't want to stackBlur an image this large, so let's copy
        // out the goodies.

    }

    // Draw the needle
    painter.setBrush(QColor(0, 0, 0));
    painter.setPen(Qt::black);
    painter.drawPath(d->positionedNeedle);

    // Draw line in middle of the needle for texture, use world matrix to
    // give correct scaling, translation, and rotation
    painter.setWorldMatrix(m);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(0, 0, 0, -84);

    // Disable transformations
    painter.setWorldMatrixEnabled(false);

    // Draw cover over the needle
    painter.setBrush(Qt::black);
    painter.drawPixmap(rect(), d->needleCoverPixmap, d->needleCoverPixmap.rect());
}

void S5WDial::setLabel(const QString &label)
{
    m_label = label;
    updateFace();
}

QString S5WDial::label() const
{
    return m_label;
}

void S5WDial::printOut()
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFileName("/tmp/dial-printout.pdf");
    printer.setFontEmbeddingEnabled(false);
    printer.setDocName("S5W Dial Graph Paper");
    printer.setCreator("S5WDial");

    QPainter painter;

    painter.begin(&printer);

    renderDrawing(painter, true);

    painter.setFont(QFont("Arial", 3));
    painter.setPen(QColor(0, 0, 0, 128));
    for(int i = -100; i <= 100; i += 10) {
        painter.drawLine(-100, i, 100, i);
        painter.drawLine(i, -100, i, 100);
        QString text(QString::number(i));
        QRectF horizLeft(-200, i - 30, 98, 60);
        QRectF horizRight(102, i - 30, 98, 60);
        QRectF vertTop(i - 100, -160, 200, 60);
        QRectF vertBot(i - 100, 102, 200, 60);

        painter.drawText(horizLeft, Qt::AlignVCenter | Qt::AlignRight, text);
        painter.drawText(horizRight, Qt::AlignVCenter | Qt::AlignLeft, text);
        painter.drawText(vertTop, Qt::AlignHCenter | Qt::AlignBottom, text);
        painter.drawText(vertBot, Qt::AlignHCenter | Qt::AlignTop, text);
    }

    painter.end();
}

QMatrix S5WDial::translationMatrix(int w, int h) const
{
    QMatrix m;

    int side = qMin(w, h); // Find shortest side

    // Adjust coordinates for ease of drawing.
    m.translate(w / 2, h / 2);
    m.scale(side / 200.0, side / 200.0);

    return m;
}

QPoint S5WDial::widgetToCoord(const QPoint &p) const
{
    QMatrix m = translationMatrix(width(), height()).inverted();
    return m.map(p);
}

void S5WDial::mouseMoveEvent(QMouseEvent *e)
{
    (void) e;
#ifdef SHOW_BEZIER
    if(e->buttons() & Qt::LeftButton) {
        m_ctrl1 = widgetToCoord(e->pos());
        update();
    }
    else if(e->buttons() & Qt::RightButton) {
        m_ctrl2 = widgetToCoord(e->pos());
        update();
    }
#endif
}

void S5WDial::mouseReleaseEvent(QMouseEvent *e)
{
    (void) e;
#ifdef SHOW_BEZIER
    if(e->button() == Qt::LeftButton)
        m_ctrl1 = widgetToCoord(e->pos());
    else
        m_ctrl2 = widgetToCoord(e->pos());

    update();
#endif
}

double S5WDial::minimum() const
{
    return m_minValue;
}

double S5WDial::value() const
{
    return m_setValue;
}

double S5WDial::tickInterval() const
{
    return m_tickStep;
}

double S5WDial::maximum() const
{
    return m_maxValue;
}

double S5WDial::tripPoint() const
{
    return m_redValue;
}

bool S5WDial::doTripOnIncreasing() const
{
    return m_redIsHigher;
}

void S5WDial::setTripPoint(double tripPoint)
{
    m_redValue = tripPoint;
    updateFace();
}

void S5WDial::setTripOnIncreasing(bool doTripOnIncreasing)
{
    m_redIsHigher = doTripOnIncreasing;
    updateFace();
}

void S5WDial::setMinimum(double min)
{
    if(min == m_minValue)
        return;

    m_minValue = min;

    double oldValue = value();
    m_dblValue = min; // Let's see what happens
    setValue(min); // Change current value first to force the dial to update
    setValue(oldValue);

    updateFace();
}

void S5WDial::setMaximum(double max)
{
    if(max == m_maxValue)
        return;

    m_maxValue = max;
    updateFace();
}

double S5WDial::degreesFromValue(double value) const
{
    double delta = maximum() - minimum();
    if(delta == 0.0)
        return 0;

    // For the purposes of this function, 0 degrees is pointing straight up,
    // so positive rotation is a lesser value and negative rotation is a
    // greater value. (FULL_RANGE / 2) is the full range of deflection either
    // way.
    double rotation = ((value - minimum()) / delta - 0.5) * FULL_RANGE;
    return rotation;
}

void S5WDial::resizeEvent(QResizeEvent *)
{
    d->positionedNeedle = scaledNeedlePath(currentRotation());
    createCachedBackground();
}

void S5WDial::createCachedBackground()
{
    QImage background = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    QColor trans(0, 0, 0, 0);

    background.fill(trans.rgba());

    QMatrix m = translationMatrix(width(), height());
    QPainter painter;

    // Cache the background
    painter.begin(&background);
    painter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);
    painter.setWorldMatrix(m);

    renderDrawingBackground(painter);
    painter.end();

    d->background = QPixmap::fromImage(background);

    // Do same thing for the needle cover.
    background.fill(trans.rgba());

    painter.begin(&background);
    painter.setRenderHint(QPainter::Antialiasing, m_antialiasingEnabled);
    painter.setWorldMatrix(m);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);

    painter.drawPath(d->needleCover);
    painter.end();

    d->needleCoverPixmap = QPixmap::fromImage(background);
}

void S5WDial::updateFace()
{
    createCachedBackground();
    update();
}

QPainterPath S5WDial::mapPath(const QPainterPath &p, const QMatrix &m) const
{
    QPainterPath path(p);

    // Iterate through every point in the path, map it using the matrix, and
    // then set the path to the mapped value.
    for(int i = 0; i < path.elementCount(); ++i) {
        QPointF mapped = m.map(path.elementAt(i));

        path.setElementPositionAt(i, mapped.x(), mapped.y());
    }

    return path;
}

QPainterPath S5WDial::scaledNeedlePath(double rotation) const
{
    QMatrix m = translationMatrix(width(), height());
    m.rotate(rotation);
    return mapPath(d->needle, m);
}

bool S5WDial::dropShadowEnabled() const
{
    return m_dropShadowEnabled;
}

void S5WDial::setDropShadowEnabled(bool enable)
{
    if(m_dropShadowEnabled == enable)
        return;

    m_dropShadowEnabled = enable;
    updateFace(); // Need to update face for cover shadow.
}

bool S5WDial::animationEnabled() const
{
    return m_animationEnabled;
}

void S5WDial::setAnimationEnabled(bool enable)
{
    if(m_animationEnabled == enable)
        return;

    m_animationEnabled = enable;

    if(!m_animationEnabled && m_animationTimer->isActive()) {
        m_animationTimer->stop();
        setValue(value());
    }
}

bool S5WDial::antialiasingEnabled() const
{
    return m_antialiasingEnabled;
}

void S5WDial::setAntialiasingEnabled(bool enable)
{
    if(m_antialiasingEnabled == enable)
        return;

    m_antialiasingEnabled = enable;
    updateFace();
}

// vim: set fileencoding=utf-8 encoding=utf-8 et sw=4 ts=8:
