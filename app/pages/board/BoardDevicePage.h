#ifndef BOARD_DEVICE_PAGE_H
#define BOARD_DEVICE_PAGE_H

#include <QWidget>

#include "BoardDeviceService.h"

class QCheckBox;
class QHideEvent;
class QLabel;
class QPushButton;
class QShowEvent;

class BoardDevicePage : public QWidget
{
    Q_OBJECT

public:
    explicit BoardDevicePage(QWidget *parent = nullptr);
    ~BoardDevicePage();

signals:
    void backRequested();

private slots:
    void returnToHome();

    void onAp3216cUpdated(const BoardAp3216cData &data);
    void onDht11Updated(const BoardDht11Data &data);
    void onIcm20608Updated(const BoardIcm20608Data &data);
    void onSr04Updated(const BoardSr04Data &data);
    void onSr501Updated(const BoardSr501Data &data);

    void onBuzzerUpdated(bool enabled);
    void onBuzzerButtonClicked();
    void onAutoAlarmChanged(int state);

    void onDeviceError(const QString &deviceName,
                       const QString &message);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QWidget *createCard(const QString &title,
                        QLabel **valueLabel);

private:
    BoardDeviceService *m_service;

    QLabel *m_titleLabel;
    QLabel *m_statusLabel;

    QLabel *m_ap3216cLabel;
    QLabel *m_dht11Label;
    QLabel *m_icm20608Label;
    QLabel *m_sr04Label;
    QLabel *m_sr501Label;
    QLabel *m_buzzerLabel;

    QPushButton *m_backButton;
    QPushButton *m_buzzerButton;
    QCheckBox *m_autoAlarmCheckBox;
};

#endif