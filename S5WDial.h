#ifndef WIDGET_S5WDIAL
#define WIDGET_S5WDIAL

/*
 * S5WDial.h - part of DialWidget
 * Copyright © 2007 Michael Pyne <michael.pyne@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QWidget>
#include <QString>
#include <QRectF>

class QTimer;
class QMatrix;
class QPainterPath;
class QImage;

class S5WDial : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(double tripPoint READ tripPoint WRITE setTripPoint)
    Q_PROPERTY(bool doTripOnIncreasing READ doTripOnIncreasing WRITE setTripOnIncreasing)
    Q_PROPERTY(bool antialiasingEnabled READ antialiasingEnabled WRITE setAntialiasingEnabled)
    Q_PROPERTY(bool dropShadowEnabled READ dropShadowEnabled WRITE setDropShadowEnabled)
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled)
    Q_PROPERTY(double tickInterval READ tickInterval WRITE setTickInterval)

public:

    S5WDial(QWidget *parent, double divider);

    void setGreenBand(double low, double high);
    void setRedBand(double value, bool higherIsWorse = true);
    void setTickInterval(double interval);

    QString label() const;
    double minimum() const;
    double maximum() const;
    double value() const;
    double tickInterval() const;
    double tripPoint() const;

    bool antialiasingEnabled() const;
    bool animationEnabled() const;
    bool dropShadowEnabled() const;
    bool doTripOnIncreasing() const;

    QSize sizeHint() const;

// Constants for use in calculations.
    // Number of degrees from min to max.
    static const double FULL_RANGE;

    // Number of degrees to full scale. i.e. starting from 0 degrees (points
    // to the right), how many degress to rotate to be pointing full scale.
    static const double FULL_SCALE;

    // Number of degrees to rotate to be pointing at minimum scale.  We add
    // degrees since counter-clockwise is positive.
    static const double ZERO_SCALE;

    // Rectangle enclosing the circle for the face of the dial.
    static const QRectF FACE_RECT;

    // Rectangle enclosing the circle for the tick marks.
    static const QRectF TICK_MARKS_RECT;

    // Maximum amount to move the dial per second.
    static const double MAX_NEEDLE_SPEED;

    // Number of frames to animate per second.
    static const int FRAMES_PER_SECOND;

public slots:
    void setMaximum(double max);
    void setMinimum(double min);
    void setTripPoint(double tripPoint);
    void setTripOnIncreasing(bool doTripOnIncreasing);
    void setValue(double val);
    void setLabel(const QString &label);
    void setValue(int value);
    void display(double value);
    void setAntialiasingEnabled(bool enable);
    void setDropShadowEnabled(bool enable);
    void setAnimationEnabled(bool enable);

signals:
    void mouseTrack(QPoint pos);
    void valueChanged(double newVal);
    void valueChanged(int newVal);

protected:
    virtual void paintEvent(QPaintEvent *e);

    // Used for SHOW_BEZIER stuff.
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);

    inline double degreesFromValue(double value) const;

    // Convert to/from a delta-value (i.e. change of 5 degrees Celsius) to the
    // equivalent deflection on the meter in degrees.
    inline double deltaToDegrees(double delta) const;
    inline double degreesToDelta(double degrees) const;

private slots:
    void printOut();
    void moveNeedle();

private:
    inline double currentRotation() const;
    void renderDrawingBackground(QPainter &painter, bool expand = false);
    void renderDrawing(QPainter &painter, bool expand = false);
    void drawTickMarks(QPainter &painter);
    void drawBand(QPainter &painter, QColor color, double l, double r);
    QMatrix translationMatrix(int w, int h) const;
    QPoint widgetToCoord(const QPoint &p) const;
    QPainterPath scaledNeedlePath(double rotation) const;
    void createCachedBackground();
    void updateFace();

    // templated to allow functions and functors to work.
    template<typename T>
    QImage createShadowMask(T &fn, int radius = 1);

    void drawNeedleToPainter(QPainter &p, const QRect &r);
    QPainterPath mapPath(const QPainterPath &p, const QMatrix &m) const;

private: // members
    class Private;

    double m_tickStep;

    double m_greenLow, m_greenHigh;
    double m_redValue;
    bool m_redIsHigher;
    bool m_antialiasingEnabled, m_dropShadowEnabled;
    bool m_animationEnabled;

    QTimer *m_animationTimer;

    double m_dblValue; // The actual value at this time.
    double m_setValue; // The value which is set as true.  May not be the
                       // current value if the needle is taking time to catch
		       // up.
    double m_velocity; // Velocity of the needle, (deg/frame)
    double m_minValue, m_maxValue;
    double m_needleDelta;

    QString m_label;

    Private *d;

    double m_divider;
};

#endif
