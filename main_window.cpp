#include <QApplication>
#include <QTimer>
#include <QMouseEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QRegion>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QDebug>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <QWidget>
#include <QThread>
#include "main_window.h"
#include <X11/extensions/shape.h>
#include <QtX11Extras/QX11Info>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Init attributes.
    setMouseTracking(true);   // make MouseMove can response
    installEventFilter(this);  // add event filter

    firstMove = false;
    firstPressButton = false;
    firstReleaseButton = false;
    dragStartX = 0;
    dragStartY = 0;

    isPressButton = false;
    isReleaseButton = false;

    isKeyPress = false;

    recordX = 0;
    recordY = 0;
    recordWidth = 0;
    recordHeight = 0;

    drawDragPoint = false;

    recordButtonStatus = RECORD_BUTTON_NORMAL;

    countdownCounter = 0;

    resizeHandleBigImg = QImage("image/resize_handle_big.png");
    resizeHandleSmallImg = QImage("image/resize_handle_small.png");
    countdown1Img = QImage("image/countdown_1.png");
    countdown2Img = QImage("image/countdown_2.png");
    countdown3Img = QImage("image/countdown_3.png");
    recordIconNormalImg = QImage("image/record_icon_normal.png");
    recordIconHoverImg = QImage("image/record_icon_hover.png");
    recordIconPressImg = QImage("image/record_icon_press.png");
    recordStopImg = QImage("image/record_stop.png");

    // Get all windows geometry.
    WindowManager windowManager;
    QList<xcb_window_t> windows = windowManager.getWindows();
    rootWindowRect = windowManager.getRootWindowRect();

    for (int i = 0; i < windows.length(); i++) {
        windowRects.append(windowManager.getWindowRect(windows[i]));
    }

    setDragCursor();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (!firstMove) {
        QRectF tooltipRect((rootWindowRect.width - INIT_TOOLTIP_WIDTH) / 2, (rootWindowRect.height - INIT_TOOLTIP_HEIGHT) / 2, INIT_TOOLTIP_WIDTH, INIT_TOOLTIP_HEIGHT);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.8);
        QPainterPath path;
        path.addRoundedRect(tooltipRect, 8, 8);
        painter.fillPath(path, Qt::white);

        QString tooltipString = "点击录制窗口或者全屏\n拖动鼠标选择录制区域";
        QFont font = painter.font() ;
        font.setPointSize(11);
        painter.setFont(font);
        painter.setPen(QPen(QColor("#000000")));
        painter.drawText(tooltipRect, Qt::AlignCenter, tooltipString);
    }

    if (recordWidth > 0 && recordHeight > 0) {

        QRect backgroundRect = QRect(rootWindowRect.x, rootWindowRect.y, rootWindowRect.width, rootWindowRect.height);
        QRect frameRect = QRect(recordX, recordY, recordWidth - 1, recordHeight - 1);

        // Draw background.
        painter.setBrush(QBrush("#000000"));
        painter.setOpacity(0.8);

        painter.setClipping(true);
        painter.setClipRegion(QRegion(backgroundRect).subtracted(QRegion(frameRect)));
        painter.drawRect(backgroundRect);

        // Draw frame.
        QPen framePen(QColor("#01bdff"));
        framePen.setWidth(2);
        painter.setBrush(QBrush());  // clear brush
        painter.setPen(framePen);
        painter.drawRect(frameRect);

        // Reset clip.
        painter.setClipRegion(QRegion(backgroundRect));

        // Draw drag pint.
        if (recordButtonStatus == RECORD_BUTTON_NORMAL && drawDragPoint) {
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS + recordHeight / 2), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS + recordHeight / 2), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth / 2, recordY - DRAG_POINT_RADIUS), resizeHandleBigImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth / 2, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleBigImg);

            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS, recordY - DRAG_POINT_RADIUS + recordHeight / 2), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth, recordY - DRAG_POINT_RADIUS + recordHeight / 2), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth / 2, recordY - DRAG_POINT_RADIUS), resizeHandleSmallImg);
            painter.drawImage(QPoint(recordX - DRAG_POINT_RADIUS + recordWidth / 2, recordY - DRAG_POINT_RADIUS + recordHeight), resizeHandleSmallImg);
        }

        // Draw record panel.
        if (firstPressButton) {
            if (firstReleaseButton) {
                QString buttonString;
                if (recordButtonStatus == RECORD_BUTTON_NORMAL) {
                    int recordButtonX = recordX + (recordWidth - RECORD_BUTTON_AREA_WIDTH) / 2;
                    int recordButtonY = recordY + (recordHeight - RECORD_BUTTON_AREA_HEIGHT - RECORD_OPTIONS_AREA_HEIGHT - RECORD_OPTIONS_AREA_PADDING) / 2;
                    QRectF recordButtonRect(recordButtonX, recordButtonY, RECORD_BUTTON_AREA_WIDTH, RECORD_BUTTON_AREA_HEIGHT);

                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setOpacity(0.8);
                    QPainterPath path;
                    path.addRoundedRect(recordButtonRect, 8, 8);
                    painter.fillPath(path, Qt::white);

                    if (recordButtonState == BUTTON_STATE_NORMAL) {
                        painter.drawImage(QPoint(recordX + (recordWidth - recordIconNormalImg.width()) / 2, recordButtonY + RECORD_BUTTON_OFFSET_Y), recordIconNormalImg);
                    } else if (recordButtonState == BUTTON_STATE_HOVER) {
                        painter.drawImage(QPoint(recordX + (recordWidth - recordIconNormalImg.width()) / 2, recordButtonY + RECORD_BUTTON_OFFSET_Y), recordIconHoverImg);
                    } else if (recordButtonState == BUTTON_STATE_PRESS) {
                        painter.drawImage(QPoint(recordX + (recordWidth - recordIconNormalImg.width()) / 2, recordButtonY + RECORD_BUTTON_OFFSET_Y), recordIconPressImg);
                    }

                    QRectF recordStringRect(recordButtonX, recordButtonY + recordIconNormalImg.height(), RECORD_BUTTON_AREA_WIDTH, RECORD_BUTTON_AREA_HEIGHT - recordIconNormalImg.height());
                    QString recordString = "开始录制";
                    QFont font = painter.font() ;
                    font.setPointSize(11);
                    painter.setFont(font);
                    painter.setPen(QPen(QColor("#e34342")));
                    painter.drawText(recordStringRect, Qt::AlignCenter, recordString);

                    int recordOptionsX = recordX + (recordWidth - RECORD_BUTTON_AREA_WIDTH) / 2;
                    int recordOptionsY = recordY + (recordHeight + RECORD_BUTTON_AREA_HEIGHT - RECORD_OPTIONS_AREA_HEIGHT + RECORD_OPTIONS_AREA_PADDING) / 2;
                    QRectF recordOptionsRect(recordOptionsX, recordOptionsY, RECORD_BUTTON_AREA_WIDTH, RECORD_OPTIONS_AREA_HEIGHT);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setOpacity(0.8);
                    QPainterPath recordOptionsPath;
                    recordOptionsPath.addRoundedRect(recordOptionsRect, 8, 8);
                    painter.fillPath(recordOptionsPath, Qt::white);
                } else if (recordButtonStatus == RECORD_BUTTON_RECORDING) {
                    int buttonX, buttonY;
                    if (rootWindowRect.height - recordY - recordHeight > RECORD_STOP_BUTTON_HEIGHT) {
                        buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
                        buttonY = recordY + recordHeight;
                    } else {
                        buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
                        buttonY = recordY + recordHeight - RECORD_STOP_BUTTON_HEIGHT;
                    }
                    painter.drawImage(QRect(buttonX, buttonY, RECORD_STOP_BUTTON_WIDTH, RECORD_STOP_BUTTON_HEIGHT), recordStopImg);
                }

                painter.setClipping(false);

                // Draw record wait second.
                if (countdownCounter > 0) {
                    QRectF countdownRect(recordX + (recordWidth - COUNTDOWN_TOOLTIP_WIDTH) / 2,
                                         recordY + (recordHeight - COUNTDOWN_TOOLTIP_HEIGHT) / 2,
                                         COUNTDOWN_TOOLTIP_WIDTH,
                                         COUNTDOWN_TOOLTIP_HEIGHT);

                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setOpacity(0.8);
                    QPainterPath path;
                    path.addRoundedRect(countdownRect, 8, 8);
                    painter.fillPath(path, Qt::white);

                    int countdownX = recordX + (recordWidth - countdown1Img.width()) / 2;
                    int countdownY = recordY + (recordHeight - COUNTDOWN_TOOLTIP_HEIGHT) / 2 + COUNTDOWN_NUMBER_OFFSET_Y;

                    if (countdownCounter == 1) {
                        painter.drawImage(QPoint(countdownX, countdownY), countdown1Img);
                    } else if (countdownCounter == 2) {
                        painter.drawImage(QPoint(countdownX, countdownY), countdown2Img);
                    } else if (countdownCounter == 3) {
                        painter.drawImage(QPoint(countdownX, countdownY), countdown3Img);
                    }
                }
            }
        }
    }
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
#undef KeyPress
#undef KeyRelease
    if (event->type() == QEvent::KeyPress) {
        isKeyPress = true;
    } else if (event->type() == QEvent::KeyRelease) {
        isKeyPress = false;
    }

    int recordButtonX = recordX + (recordWidth - RECORD_BUTTON_AREA_WIDTH) / 2;
    int recordButtonY = recordY + (recordHeight - RECORD_BUTTON_AREA_HEIGHT - RECORD_OPTIONS_AREA_HEIGHT - RECORD_OPTIONS_AREA_PADDING) / 2;

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        dragStartX = mouseEvent->x();
        dragStartY = mouseEvent->y();
        if (!firstPressButton) {
            firstPressButton = true;
        } else {
            dragAction = getAction(event);

            dragRecordX = recordX;
            dragRecordY = recordY;
            dragRecordWidth = recordWidth;
            dragRecordHeight = recordHeight;

            if (firstReleaseButton) {
                int pressX = mouseEvent->x();
                int pressY = mouseEvent->y();

                int buttonX, buttonY;

                if (rootWindowRect.height - recordY - recordHeight > RECORD_STOP_BUTTON_HEIGHT) {
                    buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
                    buttonY = recordY + recordHeight;
                } else {
                    buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
                    buttonY = recordY + recordHeight - RECORD_STOP_BUTTON_HEIGHT;
                }

                if (pressX > buttonX && pressX < buttonX + RECORD_STOP_BUTTON_WIDTH && pressY > buttonY && pressY < buttonY + RECORD_STOP_BUTTON_HEIGHT) {
                    if (recordButtonStatus == RECORD_BUTTON_RECORDING) {
                        recordProcess.stopRecord();
                        QApplication::quit();
                    }
                }

                if (pressX > recordButtonX && pressX < recordButtonX + RECORD_BUTTON_AREA_WIDTH && pressY > recordButtonY && pressY < recordButtonY + RECORD_BUTTON_AREA_HEIGHT) {
                    recordButtonState = BUTTON_STATE_PRESS;

                    if (recordButtonStatus == RECORD_BUTTON_NORMAL) {
                        recordButtonStatus = RECORD_BUTTON_WAIT;

                        countdownCounter = 3;
                        showCountdownTimer = new QTimer(this);
                        connect(showCountdownTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));
                        showCountdownTimer->start(500);

                        recordProcess.setRecordInfo(recordX, recordY, recordWidth, recordHeight);
                        if (isKeyPress) {
                            recordProcess.setRecordType(RecordProcess::RECORD_TYPE_VIDEO);
                        } else {
                            recordProcess.setRecordType(RecordProcess::RECORD_TYPE_GIF);
                        }

                        resetCursor();

                        repaint();
                    }
                }
            }
        }

        isPressButton = true;
        isReleaseButton = false;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (!firstReleaseButton) {
            firstReleaseButton = true;

            updateCursor(event);
        }

        isPressButton = false;
        isReleaseButton = true;

        recordButtonState = BUTTON_STATE_NORMAL;

        repaint();
    } else if (event->type() == QEvent::MouseMove) {
        if (!firstMove) {
            firstMove = true;
        }

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int pressX = mouseEvent->x();
        int pressY = mouseEvent->y();

        if (pressX > recordButtonX && pressX < recordButtonX + RECORD_BUTTON_AREA_WIDTH && pressY > recordButtonY && pressY < recordButtonY + RECORD_BUTTON_AREA_HEIGHT) {
            recordButtonState = BUTTON_STATE_HOVER;

            repaint();
        }

        if (firstPressButton) {
            if (!firstReleaseButton) {
                if (isPressButton && !isReleaseButton) {
                    recordX = std::min(dragStartX, mouseEvent->x());
                    recordY = std::min(dragStartY, mouseEvent->y());
                    recordWidth = std::abs(dragStartX - mouseEvent->x());
                    recordHeight = std::abs(dragStartY - mouseEvent->y());

                    repaint();
                }
            } else if (isPressButton) {
                if (recordButtonStatus == RECORD_BUTTON_NORMAL) {
                    if (dragAction == ACTION_MOVE) {
                        recordX = std::max(std::min(dragRecordX + mouseEvent->x() - dragStartX, rootWindowRect.width - recordWidth), 1);
                        recordY = std::max(std::min(dragRecordY + mouseEvent->y() - dragStartY, rootWindowRect.height - recordHeight), 1);
                    } else if (dragAction == ACTION_RESIZE_TOP_LEFT) {
                        resizeTop(mouseEvent);
                        resizeLeft(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_TOP_RIGHT) {
                        resizeTop(mouseEvent);
                        resizeRight(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_BOTTOM_LEFT) {
                        resizeBottom(mouseEvent);
                        resizeLeft(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_BOTTOM_RIGHT) {
                        resizeBottom(mouseEvent);
                        resizeRight(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_TOP) {
                        resizeTop(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_BOTTOM) {
                        resizeBottom(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_LEFT) {
                        resizeLeft(mouseEvent);
                    } else if (dragAction == ACTION_RESIZE_RIGHT) {
                        resizeRight(mouseEvent);
                    }

                    repaint();
                }
            }

            updateCursor(event);

            bool drawPoint = getAction(event) != ACTION_MOVE;
            if (drawPoint != drawDragPoint) {
                drawDragPoint = drawPoint;
                repaint();
            }
        } else {
            for (int i = 0; i < windowRects.length(); i++) {
                int wx = windowRects[i].x;
                int wy = windowRects[i].y;
                int ww = windowRects[i].width;
                int wh = windowRects[i].height;
                int ex = mouseEvent->x();
                int ey = mouseEvent->y();
                if (ex > wx && ex < wx + ww && ey > wy && ey < wy + wh) {
                    recordX = wx;
                    recordY = wy;
                    recordWidth = ww;
                    recordHeight = wh;

                    repaint();

                    break;
                }
            }
        }
    }

    return false;
}

void MainWindow::showCountdown()
{
    repaint();

    if (countdownCounter <= 0) {
        showCountdownTimer->stop();

        recordButtonStatus = RECORD_BUTTON_RECORDING;
        repaint();

        updateMouseEventArea();

        resetCursor();

        recordProcess.start();
    }

    countdownCounter--;
}

void MainWindow::updateMouseEventArea()
{
    int buttonX, buttonY;
    if (rootWindowRect.height - recordY - recordHeight > RECORD_STOP_BUTTON_HEIGHT) {
        buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
        buttonY = recordY + recordHeight;
    } else {
        buttonX = recordX + recordWidth / 2 - RECORD_STOP_BUTTON_WIDTH / 2;
        buttonY = recordY + recordHeight - RECORD_STOP_BUTTON_HEIGHT;
    }

    XRectangle* reponseArea = new XRectangle;
    reponseArea->x = buttonX;
    reponseArea->y = buttonY;
    reponseArea->width = RECORD_STOP_BUTTON_WIDTH;
    reponseArea->height = RECORD_STOP_BUTTON_HEIGHT;

    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput, 0, 0, reponseArea ,1 ,ShapeSet, YXBanded);
}

void MainWindow::resizeTop(QMouseEvent *mouseEvent)
{
    int offsetY = mouseEvent->y() - dragStartY;
    recordY = std::max(std::min(dragRecordY + offsetY, dragRecordY + dragRecordHeight - RECORD_MIN_SIZE), 1);
    recordHeight = std::max(std::min(dragRecordHeight - offsetY, rootWindowRect.height), RECORD_MIN_SIZE);
}

void MainWindow::resizeBottom(QMouseEvent *mouseEvent)
{
    int offsetY = mouseEvent->y() - dragStartY;
    recordHeight = std::max(std::min(dragRecordHeight + offsetY, rootWindowRect.height), RECORD_MIN_SIZE);
}

void MainWindow::resizeLeft(QMouseEvent *mouseEvent)
{
    int offsetX = mouseEvent->x() - dragStartX;
    recordX = std::max(std::min(dragRecordX + offsetX, dragRecordX + dragRecordWidth - RECORD_MIN_SIZE), 1);
    recordWidth = std::max(std::min(dragRecordWidth - offsetX, rootWindowRect.width), RECORD_MIN_SIZE);
}

void MainWindow::resizeRight(QMouseEvent *mouseEvent)
{
    int offsetX = mouseEvent->x() - dragStartX;
    recordWidth = std::max(std::min(dragRecordWidth + offsetX, rootWindowRect.width), RECORD_MIN_SIZE);
}

int MainWindow::getAction(QEvent *event) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    int cursorX = mouseEvent->x();
    int cursorY = mouseEvent->y();

    if (cursorX > recordX - CURSOR_BOUND
        && cursorX < recordX + CURSOR_BOUND
        && cursorY > recordY - CURSOR_BOUND
        && cursorY < recordY + CURSOR_BOUND) {
        // Top-Left corner.
        return ACTION_RESIZE_TOP_LEFT;
    } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
               && cursorX < recordX + recordWidth + CURSOR_BOUND
               && cursorY > recordY + recordHeight - CURSOR_BOUND
               && cursorY < recordY + recordHeight + CURSOR_BOUND) {
        // Bottom-Right corner.
        return ACTION_RESIZE_BOTTOM_RIGHT;
    } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
               && cursorX < recordX + recordWidth + CURSOR_BOUND
               && cursorY > recordY - CURSOR_BOUND
               && cursorY < recordY + CURSOR_BOUND) {
        // Top-Right corner.
        return ACTION_RESIZE_TOP_RIGHT;
    } else if (cursorX > recordX - CURSOR_BOUND
               && cursorX < recordX + CURSOR_BOUND
               && cursorY > recordY + recordHeight - CURSOR_BOUND
               && cursorY < recordY + recordHeight + CURSOR_BOUND) {
        // Bottom-Left corner.
        return ACTION_RESIZE_BOTTOM_LEFT;
    } else if (cursorX > recordX - CURSOR_BOUND
               && cursorX < recordX + CURSOR_BOUND) {
        // Left.
        return ACTION_RESIZE_LEFT;
    } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
               && cursorX < recordX + recordWidth + CURSOR_BOUND) {
        // Right.
        return ACTION_RESIZE_RIGHT;
    } else if (cursorY > recordY - CURSOR_BOUND
               && cursorY < recordY + CURSOR_BOUND) {
        // Top.
        return ACTION_RESIZE_TOP;
    } else if (cursorY > recordY + recordHeight - CURSOR_BOUND
               && cursorY < recordY + recordHeight + CURSOR_BOUND) {
        // Bottom.
        return ACTION_RESIZE_BOTTOM;
    } else {
        return ACTION_MOVE;
    }
}

void MainWindow::updateCursor(QEvent *event)
{
    if (recordButtonStatus == RECORD_BUTTON_NORMAL) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int cursorX = mouseEvent->x();
        int cursorY = mouseEvent->y();

        if (cursorX > recordX - CURSOR_BOUND
            && cursorX < recordX + CURSOR_BOUND
            && cursorY > recordY - CURSOR_BOUND
            && cursorY < recordY + CURSOR_BOUND) {
            // Top-Left corner.
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
                   && cursorX < recordX + recordWidth + CURSOR_BOUND
                   && cursorY > recordY + recordHeight - CURSOR_BOUND
                   && cursorY < recordY + recordHeight + CURSOR_BOUND) {
            // Bottom-Right corner.
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
                   && cursorX < recordX + recordWidth + CURSOR_BOUND
                   && cursorY > recordY - CURSOR_BOUND
                   && cursorY < recordY + CURSOR_BOUND) {
            // Top-Right corner.
            QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        } else if (cursorX > recordX - CURSOR_BOUND
                   && cursorX < recordX + CURSOR_BOUND
                   && cursorY > recordY + recordHeight - CURSOR_BOUND
                   && cursorY < recordY + recordHeight + CURSOR_BOUND) {
            // Bottom-Left corner.
            QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        } else if (cursorX > recordX - CURSOR_BOUND
                   && cursorX < recordX + CURSOR_BOUND) {
            // Left.
            QApplication::setOverrideCursor(Qt::SizeHorCursor);
        } else if (cursorX > recordX + recordWidth - CURSOR_BOUND
                   && cursorX < recordX + recordWidth + CURSOR_BOUND) {
            // Right.
            QApplication::setOverrideCursor(Qt::SizeHorCursor);
        } else if (cursorY > recordY - CURSOR_BOUND
                   && cursorY < recordY + CURSOR_BOUND) {
            // Top.
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
        } else if (cursorY > recordY + recordHeight - CURSOR_BOUND
                   && cursorY < recordY + recordHeight + CURSOR_BOUND) {
            // Bottom.
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
        } else {
            if (isPressButton) {
                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
            } else {
                QApplication::setOverrideCursor(Qt::OpenHandCursor);
            }
        }
    }
}

void MainWindow::setDragCursor()
{
    QApplication::setOverrideCursor(Qt::CrossCursor);
}

void MainWindow::resetCursor()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}