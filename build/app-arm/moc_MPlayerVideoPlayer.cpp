/****************************************************************************
** Meta object code from reading C++ file 'MPlayerVideoPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../services/video/MPlayerVideoPlayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MPlayerVideoPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MPlayerVideoPlayer_t {
    QByteArrayData data[26];
    char stringdata0[361];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MPlayerVideoPlayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MPlayerVideoPlayer_t qt_meta_stringdata_MPlayerVideoPlayer = {
    {
QT_MOC_LITERAL(0, 0, 18), // "MPlayerVideoPlayer"
QT_MOC_LITERAL(1, 19, 15), // "playbackStarted"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 8), // "filePath"
QT_MOC_LITERAL(4, 45, 13), // "pausedChanged"
QT_MOC_LITERAL(5, 59, 6), // "paused"
QT_MOC_LITERAL(6, 66, 15), // "playbackStopped"
QT_MOC_LITERAL(7, 82, 16), // "playbackFinished"
QT_MOC_LITERAL(8, 99, 17), // "fullscreenChanged"
QT_MOC_LITERAL(9, 117, 10), // "fullscreen"
QT_MOC_LITERAL(10, 128, 15), // "positionChanged"
QT_MOC_LITERAL(11, 144, 10), // "positionMs"
QT_MOC_LITERAL(12, 155, 15), // "durationChanged"
QT_MOC_LITERAL(13, 171, 10), // "durationMs"
QT_MOC_LITERAL(14, 182, 13), // "errorOccurred"
QT_MOC_LITERAL(15, 196, 7), // "message"
QT_MOC_LITERAL(16, 204, 16), // "onProcessStarted"
QT_MOC_LITERAL(17, 221, 15), // "onProcessOutput"
QT_MOC_LITERAL(18, 237, 17), // "onProcessFinished"
QT_MOC_LITERAL(19, 255, 8), // "exitCode"
QT_MOC_LITERAL(20, 264, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(21, 285, 6), // "status"
QT_MOC_LITERAL(22, 292, 14), // "onProcessError"
QT_MOC_LITERAL(23, 307, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(24, 330, 5), // "error"
QT_MOC_LITERAL(25, 336, 24) // "queryPlaybackInformation"

    },
    "MPlayerVideoPlayer\0playbackStarted\0\0"
    "filePath\0pausedChanged\0paused\0"
    "playbackStopped\0playbackFinished\0"
    "fullscreenChanged\0fullscreen\0"
    "positionChanged\0positionMs\0durationChanged\0"
    "durationMs\0errorOccurred\0message\0"
    "onProcessStarted\0onProcessOutput\0"
    "onProcessFinished\0exitCode\0"
    "QProcess::ExitStatus\0status\0onProcessError\0"
    "QProcess::ProcessError\0error\0"
    "queryPlaybackInformation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MPlayerVideoPlayer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       6,    0,   85,    2, 0x06 /* Public */,
       7,    0,   86,    2, 0x06 /* Public */,
       8,    1,   87,    2, 0x06 /* Public */,
      10,    1,   90,    2, 0x06 /* Public */,
      12,    1,   93,    2, 0x06 /* Public */,
      14,    1,   96,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    0,   99,    2, 0x08 /* Private */,
      17,    0,  100,    2, 0x08 /* Private */,
      18,    2,  101,    2, 0x08 /* Private */,
      22,    1,  106,    2, 0x08 /* Private */,
      25,    0,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::LongLong,   11,
    QMetaType::Void, QMetaType::LongLong,   13,
    QMetaType::Void, QMetaType::QString,   15,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   19,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void,

       0        // eod
};

void MPlayerVideoPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MPlayerVideoPlayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->playbackStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->pausedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->playbackStopped(); break;
        case 3: _t->playbackFinished(); break;
        case 4: _t->fullscreenChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->positionChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 6: _t->durationChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onProcessStarted(); break;
        case 9: _t->onProcessOutput(); break;
        case 10: _t->onProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 11: _t->onProcessError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 12: _t->queryPlaybackInformation(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MPlayerVideoPlayer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::playbackStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::pausedChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::playbackStopped)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::playbackFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::fullscreenChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)(qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::positionChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)(qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::durationChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MPlayerVideoPlayer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MPlayerVideoPlayer::errorOccurred)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MPlayerVideoPlayer::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_MPlayerVideoPlayer.data,
    qt_meta_data_MPlayerVideoPlayer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MPlayerVideoPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MPlayerVideoPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MPlayerVideoPlayer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MPlayerVideoPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void MPlayerVideoPlayer::playbackStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MPlayerVideoPlayer::pausedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MPlayerVideoPlayer::playbackStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MPlayerVideoPlayer::playbackFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MPlayerVideoPlayer::fullscreenChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MPlayerVideoPlayer::positionChanged(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MPlayerVideoPlayer::durationChanged(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MPlayerVideoPlayer::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
