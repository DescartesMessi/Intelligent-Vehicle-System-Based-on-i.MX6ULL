#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QIcon>
#include <QString>
#include <QWidget>

class QLabel;
class QHideEvent;
class QShowEvent;
class QTimer;
class QToolButton;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void audioRequested();
    void videoRequested();
    void reverseRequested();
    void sentinelRequested();
    void diagnosticRequested();
    void cameraRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void updateDateTime();
    void updateEnvironment();

private:
    QToolButton *createEntryButton(
        const QString &text,
        const QString &iconPath,
        const QIcon &fallbackIcon);

    QLabel *createPanelTitle(const QString &text);

private:
    QLabel *m_dateLabel;
    QLabel *m_timeLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_humidityLabel;
    QLabel *m_statusLabel;

    QTimer *m_clockTimer;
    QTimer *m_environmentTimer;
};

#endif