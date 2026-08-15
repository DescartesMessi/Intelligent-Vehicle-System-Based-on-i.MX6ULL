#ifndef BOARD_DEVICE_SERVICE_H
#define BOARD_DEVICE_SERVICE_H

#include <QMetaType>
#include <QThread>
#include <QString>

#include <atomic>

struct BoardAp3216cData
{
    quint16 ir;
    quint16 als;
    quint16 ps;
    bool irValid;
    bool psValid;
    bool objectNear;
};

struct BoardDht11Data
{
    int temperatureInteger;
    int temperatureDecimal;
    int humidityInteger;
    int humidityDecimal;
};

struct BoardIcm20608Data
{
    double accelX;
    double accelY;
    double accelZ;

    double gyroX;
    double gyroY;
    double gyroZ;

    double temperature;
    quint8 whoAmI;
};

struct BoardSr04Data
{
    quint32 distanceMm;
    double distanceCm;
    quint32 pulseUs;
    bool valid;
    bool nearAlarm;
};

struct BoardSr501Data
{
    bool detected;
    bool valid;
    quint32 sequence;
};

class BoardDeviceService : public QThread
{
    Q_OBJECT

public:
    explicit BoardDeviceService(QObject *parent = nullptr);
    ~BoardDeviceService();

    void startMonitoring();
    void stopMonitoring();

    bool setBuzzer(bool enabled);
    bool buzzerEnabled() const;

signals:
    void ap3216cUpdated(const BoardAp3216cData &data);
    void dht11Updated(const BoardDht11Data &data);
    void icm20608Updated(const BoardIcm20608Data &data);
    void sr04Updated(const BoardSr04Data &data);
    void sr501Updated(const BoardSr501Data &data);

    void buzzerUpdated(bool enabled);

    void deviceError(const QString &deviceName,
                    const QString &message);

protected:
    void run() override;

private:
    std::atomic_bool m_stopRequested;
    std::atomic_bool m_buzzerEnabled;
};

Q_DECLARE_METATYPE(BoardAp3216cData)
Q_DECLARE_METATYPE(BoardDht11Data)
Q_DECLARE_METATYPE(BoardIcm20608Data)
Q_DECLARE_METATYPE(BoardSr04Data)
Q_DECLARE_METATYPE(BoardSr501Data)

#endif