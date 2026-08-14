#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>

class QLabel;
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

private slots:
    void updateDateTime();

private:
    QToolButton *createEntryButton(const QString &text,const QString &iconPath,const QIcon &fallbackIcon);
    QLabel *createPanelTitle(const QString &text);

private:
    QLabel *m_dateLabel;
    QLabel *m_timeLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_humidityLabel;
    QLabel *m_statusLabel;
    QTimer *m_clockTimer;
};

#endif